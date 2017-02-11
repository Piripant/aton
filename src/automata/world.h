#pragma once
#include <vector>
#include <array>
#include "../lua/lua.hpp"
#include "table.h"
#include <thread>
#include <condition_variable>
#include <mutex>

namespace World {
	extern uint8_t *rules;
	extern std::array<uint8_t, 3> *colors;

	extern unsigned int rules_num;
	extern unsigned int neigh_comb_num;
	extern unsigned int script_states;
	extern unsigned int threads_num;

	void Initialize(unsigned int states, uint8_t state, unsigned int height, unsigned int width);
	Table& GetCurrentTable();
	void Resize(uint8_t state, unsigned int height, unsigned int width);
	void Step();
	void StepThread(unsigned int id, bool wait);
	void LoadScript(char *filename);
	
	void SaveWorld(std::string file_name);
	bool LoadWorld(std::string file_name);
};