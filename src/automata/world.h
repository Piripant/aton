#pragma once
#include <vector>
#include <array>
#include <tuple>
#include <lua.hpp>
#include <automata/table.h>
#include <automata/scripts/script.h>

namespace World {
	extern lua_State *L;
	
	extern Script *script;

	extern std::array<uint8_t, 3> *colors;
	// A vector of (int, string), where int is the state and string is the state's tag
	extern std::vector< std::tuple<uint8_t, std::string> > states_tags; 
	
	void Initialize(uint8_t state, unsigned int height, unsigned int width);
	
	Table& GetCurrentTable();
	Table& GetFutureTable();
	void SwapTables();

	void Resize(uint8_t state, unsigned int height, unsigned int width);
	void LoadScript(std::string file_name);
	
	void SaveWorld(std::string file_name);
	bool LoadWorld(std::string file_name);
}