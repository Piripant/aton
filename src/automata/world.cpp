#include <stdint.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>
#include "world.h"
#include "table.h"
#include "../lua/lua.hpp"
#include "../utils/lua_utils.h"

using namespace std;

namespace World {
	std::array<Table, 2> tables;
	unsigned int cur_table;

	int (*neighbors)[2];
	unsigned int neighbors_num;

	std::thread *step_threads;

	bool *ready;
	std::mutex start_mtx;
	std::condition_variable start_cv;

	std::mutex* stop_mtx;
	std::condition_variable* stop_cv;
	bool *stopped;

	
	uint8_t *rules;
	std::array<uint8_t, 3> *colors;

	unsigned int rules_num;
	unsigned int neigh_comb_num;
	unsigned int script_states;
	unsigned int threads_num;

	void SetNeighborsFromLua(lua_State *L);
	void SetRules(lua_State *L);
}

void World::Initialize(unsigned int threads, uint8_t state, unsigned int width, unsigned int height)
{
	threads_num = threads;
	tables[0] = Table(state, width, height);
	tables[1] = Table(state, width, height);
	cur_table = 0;
	script_states = 0;
	neighbors_num = 0;

	// TODO: make so that this array is only for the overwritten colors
	colors = new std::array<uint8_t, 3>[256];

	step_threads = new std::thread[threads_num-1]; // Generate all the threads but one (which will be the main thread)
	stop_mtx = new std::mutex[threads_num-1];
	stop_cv = new std::condition_variable[threads_num-1];
	stopped = new bool[threads_num-1];
	ready = new bool[threads_num-1];
	for (unsigned int id = 0; id < threads_num-1; id++) {
		step_threads[id] = std::thread(StepThread, id, true);
	}
}

void World::Resize(uint8_t state, unsigned int width, unsigned int height) {
	tables[0] = Table(state, width, height);
	tables[1] = Table(state, width, height);
}

Table& World::GetCurrentTable() {
	return tables[cur_table];
}

// ----------------
// WORLD SIMULATION
// ----------------

void World::Step() {
	std::unique_lock<std::mutex> lck(start_mtx);
  	for (unsigned int id = 0; id < threads_num-1; id++) {
		ready[id] = true;
	}
  	start_cv.notify_all();
	lck.unlock();
	
	StepThread(threads_num-1, false);
	
	for (unsigned int id = 0; id < threads_num-1; id++) {
		std::unique_lock<std::mutex> lck(stop_mtx[id]);
		
		while (!stopped[id]) {
			stop_cv[id].wait(lck);
		}
		
		stopped[id] = false;
	}
	
	cur_table = (cur_table + 1) % 2; // Swap the tables for the next step
}

void World::StepThread(unsigned int id, bool is_thread) {
	// TODO: reuse the unique locks
	do {
		if (is_thread) {
			// Ready for the start signal
			std::unique_lock<std::mutex> lck(start_mtx);
			while (!ready[id]) {
				start_cv.wait(lck);
			}
		}

		Table& curr_table = tables[cur_table];
		Table& future_table = tables[(cur_table + 1) % 2];

		unsigned int& width = curr_table.width;
		unsigned int& height = curr_table.height;
		unsigned int& len = curr_table.len;

		for (unsigned int i = id; i < len; i += threads_num) {
			uint8_t& cell_state = curr_table.GetCellState(i);
			uint8_t& future_cell_state = future_table.GetCellState(i);

			// If the cell_state is invalid in the script, make the cell stay the original state
			if (cell_state >= script_states) {
				future_cell_state = cell_state;
				continue;
			}

			bool valid_neighbors = true;
			unsigned int rule_index = cell_state * neigh_comb_num;
			for (unsigned int j = 0; j < neighbors_num; j++) {
				#define x_index (((int)i % (int)width + neighbors[j][0] + (int)width) % (int)width)
				#define y_index (((int)i / (int)width + neighbors[j][1] + (int)height) % (int)height) * (int)width
				unsigned int neig_index = x_index + y_index;

				uint8_t& neig_cell_state = curr_table.GetCellState(neig_index);
				if (neig_cell_state >= script_states) { // If the neighbor cell state is invalid in the script, skip this cell
					valid_neighbors = false; 
					break;
				}
				rule_index += neig_cell_state * pow(script_states, j);
			}

			// If the neighbors had invalid states for the script, make the cell stay the original state
			if (!valid_neighbors) {
				future_cell_state = cell_state;
				continue;
			}

			// The cell states and the neighbors states are all valid
			future_cell_state = rules[rule_index];
		}

		
		if (is_thread) {
			std::unique_lock<std::mutex> lck(stop_mtx[id]);
			stopped[id] = true;
			stop_cv[id].notify_one();
			ready[id] = false;
		}
	} while (is_thread);
}

// ---------------
// SCRIPTS LOADING
// ---------------

uint8_t calculate_output_state(lua_State *L, uint8_t *combination, unsigned int combination_len, uint8_t input_state) {
	lua_getglobal(L, "get_next_state"); // The function
	lua_newtable(L);
	for (int i = 0; i < combination_len; i++) {
		lua_add_to_table(L, i+1, combination[i]);
	}

	lua_pushinteger(L, (int)input_state); // The cell state
	lua_pcall(L, 2, 1, 0); // Also pops the function + arguments
	int next_state = lua_tointeger(L, -1);
	lua_pop(L, 1);
	
	return next_state;
}

void World::SetNeighborsFromLua(lua_State *L) {
	lua_getglobal(L, "NEIGHBORS_POS"); // Get the table
	unsigned int len = lua_get_len(L, -1);

	neighbors = new int[len][2];
	neighbors_num = len;

	for (unsigned int i = 1; i <= len; i++) {
		lua_get_at_index(L, i);

		for (unsigned int n = 1; n <= 2; n++) {
			lua_get_at_index(L, n);
			neighbors[i-1][n-1] = lua_get_integer(L, -1);
		}

		lua_pop(L, 1); // To clear the stack
	}
}

void World::SetRules(lua_State *L) {
	lua_getglobal(L, "STATES_NUM");
	script_states = lua_tointeger(L, -1);
	lua_pop(L, 1);

	neigh_comb_num = pow(script_states, neighbors_num);
	rules_num = pow(script_states, neighbors_num + 1);

	cout << neigh_comb_num << endl;

	rules = new uint8_t[rules_num];
	uint8_t combination[neighbors_num]; // To store the actual combination of states
	for (uint8_t input_state = 0; input_state < script_states; input_state++) {
		// Reset the combination
		for (unsigned int i = 0; i < neighbors_num; i++) {
			combination[i] = 0;
		}
		
		for (unsigned int i = 0; i < neigh_comb_num; i++) {
			uint8_t output_state = calculate_output_state(L, combination, neighbors_num, input_state);
			unsigned int rule_index = i + input_state * neigh_comb_num;
			rules[rule_index] = output_state;
			
			// The last doesn't need to be incremented
			if (i < neigh_comb_num - 1) {
				increment_number(neighbors_num, script_states, combination);
			}
		}
	}
}

// Loading rules from a file will
// Get the neighbors possible positions
// Get the neighbors possible states
// Caculate all the output state of all possible neighbors' states' combinations
void World::LoadScript(char *file_name) {
	lua_State *L;

	L = luaL_newstate();

	luaL_openlibs(L);

	int status = luaL_loadfile(L, file_name);
	if (status) {
		fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
		exit(1);
	}

	int result = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (result) {
		fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
		exit(1);
	}

	lua_pop(L, 1); // Dump the script return value

	SetNeighborsFromLua(L);
	SetRules(L);

	lua_close(L);
}

// --------
// FILES IO
// --------

int buff_to_integer(char *buffer, unsigned int offset) {
	return *(int*)(buffer+offset);
	return static_cast<int>(static_cast<unsigned char>(buffer[offset+3]) << 24 |
							static_cast<unsigned char>(buffer[offset+2]) << 16 | 
							static_cast<unsigned char>(buffer[offset+1]) << 8 | 
							static_cast<unsigned char>(buffer[offset+0]));
}

void World::SaveWorld(std::string file_name) {
	ofstream save_file;
	Table& table = GetCurrentTable();
	
	save_file.open(file_name + ".atonw", ios::binary);

	save_file.write((const char*)&table.width, sizeof(table.width));
	save_file.write((const char*)&table.height, sizeof(table.height));

	for (unsigned int y = 0; y < table.width; y++) {
		for (unsigned int x = 0; x < table.height; x++) {
			uint8_t state = table.GetCellState(x, y);
			save_file.write((const char*)&state, sizeof(state));
		}
	}
	
	for (unsigned int i = 0; i < 256; i++) {
		uint8_t r = colors[i][0];
		uint8_t g = colors[i][1];
		uint8_t b = colors[i][2];

		save_file.write((const char*)&r, sizeof(r));
		save_file.write((const char*)&g, sizeof(g));
		save_file.write((const char*)&b, sizeof(b));
	}
	
	save_file.close();
}

bool World::LoadWorld(std::string file_name) {
	ifstream file(file_name + ".atonw", ios::binary);
	if (file) {
		file.seekg(0, file.end);
		int length = file.tellg();
		file.seekg(0, file.beg);
		char *buffer = new char[length];
		file.read(buffer, length);
		file.close();

		int width = buff_to_integer(buffer, 0);
		int height = buff_to_integer(buffer, 4);
		int world_len = width*height;

		Resize(0, width, height);
		Table& table = GetCurrentTable();

		unsigned int offset = 8;
		for (unsigned int i = offset; i < world_len+offset; i++) {
			table.GetCellState(i-offset) = static_cast<uint8_t>(buffer[i]);
		}

		offset += world_len;
		unsigned int color_i = 0;
		for (unsigned int i = offset; i < 256*3+offset; i += 3) {
			colors[color_i][0] = static_cast<uint8_t>(buffer[i]);
			colors[color_i][1] = static_cast<uint8_t>(buffer[i+1]);
			colors[color_i][2] = static_cast<uint8_t>(buffer[i+2]);
			color_i++;
		}
		
		delete[] buffer;
		return true;
	}
	
	return false;
}