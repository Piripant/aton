#pragma once
#include <vector>
#include <array>
#include "../lua/lua.hpp"
#include "table.h"

class World {
	protected:
		std::array<Table, 2> tables;
		unsigned int cur_table;

		int (*neighbors)[2];
		unsigned int neighbors_num;

		void SetNeighborsFromLua(lua_State *L);
		void SetRules(lua_State *L);
		void SetColors(lua_State *L);

	public:
		uint8_t *rules;
		std::array<uint8_t, 3> *colors;

		unsigned int rules_num;
		unsigned int neigh_comb_num;
		unsigned int states_num;
		unsigned int threads_num;

		World(unsigned int states, uint8_t state, unsigned int height, unsigned int width);
		Table& GetCurrentTable();
		void Resize(uint8_t state, unsigned int height, unsigned int width);
		void Step();
		void StepThread(unsigned int start);
		void LoadFromFile(char *filename);
};