#include "ca_script.h"

#include <automata/table.h>
#include <automata/world.h>
#include <utils/lua_utils.h>
#include <math.h>
#include <lua.hpp>
#include <iostream>

using namespace std;

CAScript::~CAScript() {
	delete[] rules;
	delete[] neighbors;
}

// Loading rules from a file will
// Get the neighbors possible positions
// Get the neighbors possible states
// Caculate all the output state of all possible neighbors' states' combinations
void CAScript::LoadScript() {
	SetNeighbors();
	SetRules();
}

uint8_t calculate_output_state(uint8_t *combination, unsigned int combination_len, uint8_t input_state) {
	lua_getglobal(World::L, "get_next_state"); // The function
	lua_newtable(World::L);
	for (int i = 0; i < combination_len; i++) {
		lua_add_to_table(World::L, i+1, combination[i]);
	}

	lua_pushinteger(World::L, (int)input_state); // The cell state
	lua_pcall(World::L, 2, 1, 0); // Also pops the function + arguments
	int next_state = lua_tointeger(World::L, -1);
	lua_pop(World::L, 1);
	
	return next_state;
}

void CAScript::SetNeighbors() {
	lua_getglobal(World::L, "NEIGHBORS_POS"); // Get the table
	unsigned int len = lua_get_len(World::L, -1);

	neighbors = new int[len][2];
	neighbors_num = len;

	for (unsigned int i = 1; i <= len; i++) {
		lua_get_at_index(World::L, i);

		for (unsigned int n = 1; n <= 2; n++) {
			lua_get_at_index(World::L, n);
			neighbors[i-1][n-1] = lua_get_integer(World::L, -1);
		}

		lua_pop(World::L, 1); // To clear the stack
	}
}

void increment_number(unsigned int len, unsigned int base, uint8_t *number) {
	number[0]++;
	for (int i = 0; i+1<len; i++) {
		if (number[i] > base-1) {
			number[i] = 0;
			number[i+1]++;
		}
	}
	if (number[len-1] > base-1) {
		throw "Number overflow";
	}
}

void CAScript::SetRules() {
	lua_getglobal(World::L, "STATES_NUM");
	states_num = lua_tointeger(World::L, -1);
	lua_pop(World::L, 1);

	neigh_comb_num = pow(states_num, neighbors_num);
	rules_num = pow(states_num, neighbors_num + 1);

	rules = new uint8_t[rules_num];
	uint8_t combination[neighbors_num]; // To store the actual combination of states
	for (uint8_t input_state = 0; input_state < states_num; input_state++) {
		// Reset the combination
		for (unsigned int i = 0; i < neighbors_num; i++) {
			combination[i] = 0;
		}
		
		for (unsigned int i = 0; i < neigh_comb_num; i++) {
			uint8_t output_state = calculate_output_state(combination, neighbors_num, input_state);
			unsigned int rule_index = i + input_state * neigh_comb_num;
			rules[rule_index] = output_state;
			
			// The last doesn't need to be incremented
			if (i < neigh_comb_num - 1) {
				increment_number(neighbors_num, states_num, combination);
			}
		}
	}
}

void CAScript::Step() {
	Table& curr_table = World::GetCurrentTable();
	Table& future_table = World::GetFutureTable();

	unsigned int& width = curr_table.width;
	unsigned int& height = curr_table.height;
	unsigned int& len = curr_table.len;

	for (unsigned int i = 0; i < len; i ++) {
		uint8_t& cell_state = curr_table.GetCellState(i);
		uint8_t& future_cell_state = future_table.GetCellState(i);

		// If the cell_state is invalid in the script, make the cell stay the original state
		if (cell_state >= states_num) {
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
			if (neig_cell_state >= states_num) { // If the neighbor cell state is invalid in the script, skip this cell
				valid_neighbors = false; 
				break;
			}
			rule_index += neig_cell_state * pow(states_num, j);
		}

		// If the neighbors had invalid states for the script, make the cell stay the original state
		if (!valid_neighbors) {
			future_cell_state = cell_state;
			continue;
		}

		// The cell states and the neighbors states are all valid
		future_cell_state = rules[rule_index];
	}

	World::SwapTables(); // Swap the tables for the next step
}