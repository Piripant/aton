#include <stdint.h>
#include <iostream>
#include <vector>
#include <math.h>
#include "world.h"
#include "table.h"
#include "../lua/lua.hpp"
#include "../utils/lua_utils.h"

World::World(unsigned int threads, uint8_t state, unsigned int height, unsigned int width)
{
	this->threads_num = threads;
	this->tables[0] = Table(state, height, width);
	this->tables[1] = Table(state, height, width);
	this->cur_table = 0;
}

void World::Resize(uint8_t state, unsigned int height, unsigned int width) {
	this->tables[0] = Table(state, height, width);
	this->tables[1] = Table(state, height, width);
}

Table& World::GetCurrentTable() {
	return this->tables[this->cur_table];
}

void World::Step() {
	this->StepThread(0);
	this->cur_table = (this->cur_table + 1) % 2; // Swap the tables for the next step
}

void World::StepThread(unsigned int start) {
	Table& curr_table = this->tables[this->cur_table];
	Table& future_table = this->tables[(this->cur_table + 1) % 2];

	unsigned int& width = curr_table.width;
	unsigned int& height = curr_table.height;
	unsigned int& len = curr_table.len;

	for (unsigned int i = start; i < len; i += this->threads_num) {
		uint8_t& cell_state = curr_table.GetCellState(i);
		uint8_t& future_cell_state = future_table.GetCellState(i);

		unsigned int rule_index = cell_state * neigh_comb_num;
		for (unsigned int j = 0; j < this->neighbors_num; j++) {
			#define x_index (((int)i % (int)width - neighbors[j][0] + (int)width) % (int)width)
			#define y_index (((int)i / (int)width - neighbors[j][1] + (int)height) % (int)height) * (int)width
			unsigned int neig_index = x_index + y_index;
			
			uint8_t& neig_cell_state = curr_table.GetCellState(neig_index);
			rule_index += neig_cell_state * pow(states_num, j);
		}

		future_cell_state = this->rules[rule_index];
	}
}

// Loading rules from a file will
// Get the neighbors possible positions
// Get the neighbors possible states
// Caculate all the output state of all possible neighbors' states' combinations
void World::LoadFromFile(char *file_name) {
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
	SetColors(L);

	lua_close(L);
}

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

void World::SetRules(lua_State *L) {
	lua_getglobal(L, "STATES_NUM");
	this->states_num = lua_tointeger(L, -1);
	lua_pop(L, 1);

	this->neigh_comb_num = pow(states_num, neighbors_num);
	this->rules_num = pow(states_num, neighbors_num + 1);

	this->rules = new uint8_t[rules_num];
	uint8_t combination[neighbors_num]; // To store the actual combination of states
	for (uint8_t input_state = 0; input_state < states_num; input_state++) {
		// Reset the combination
		for (unsigned int i = 0; i < neighbors_num; i++) {
			combination[i] = 0;
		}
		
		for (unsigned int i = 0; i < neigh_comb_num; i++) {
			uint8_t output_state = calculate_output_state(L, combination, neighbors_num, input_state);
			unsigned int rule_index = i + input_state * neigh_comb_num;
			this->rules[rule_index] = output_state;
			
			// The last doesn't need to be incremented
			if (i < neigh_comb_num - 1) {
				increment_number(neighbors_num, states_num, combination);
			}
		}
	}
}

void World::SetNeighborsFromLua(lua_State *L) {
	lua_getglobal(L, "NEIGHBORS_POS"); // Get the table
	unsigned int len = lua_get_len(L, -1);

	this->neighbors = new int[len][2];
	this->neighbors_num = len;

	for (unsigned int i = 1; i <= len; i++) {
		lua_get_at_index(L, i);

		for (unsigned int n = 1; n <= 2; n++) {
			lua_get_at_index(L, n);
			this->neighbors[i-1][n-1] = lua_get_integer(L, -1);
		}

		lua_pop(L, 1); // To clear the stack
	}
}

void World::SetColors(lua_State *L) {
	lua_getglobal(L, "COLORS");
	unsigned int len = lua_get_len(L, -1);
	if (len != this->states_num) {
		std::cout << "The number of colors doesn't match the number of states";
		return;
	}

	this->colors = new std::array<uint8_t, 3>[len];
	
	for (unsigned int i = 1; i <= len; i++) {
		lua_get_at_index(L, i);

		for (unsigned int n = 1; n <= 3; n++) {
			lua_get_at_index(L, n);
			this->colors[i-1][n-1] = lua_get_integer(L, -1);
		}

		lua_pop(L, 1);
	}
}