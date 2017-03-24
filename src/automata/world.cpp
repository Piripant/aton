#include "world.h"

#include <stdint.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>
#include <automata/scripts/ca_script.h>
#include <automata/scripts/grid_script.h>

using namespace std;

namespace World {
	lua_State *L;
	Script *script;

	std::array<Table, 2> tables;
	unsigned int cur_table;
	
	std::array<uint8_t, 3> *colors;
}

void World::Initialize(uint8_t state, unsigned int width, unsigned int height)
{
	tables[0] = Table(state, width, height);
	tables[1] = Table(state, width, height);
	cur_table = 0;

	// TODO: make so that this array is only for the overwritten colors
	colors = new std::array<uint8_t, 3>[256];
	script = new Script();

	L = luaL_newstate();
	luaL_openlibs(L);
}

void World::Resize(uint8_t state, unsigned int width, unsigned int height) {
	tables[0] = Table(state, width, height);
	tables[1] = Table(state, width, height);
}

Table& World::GetCurrentTable() {
	return tables[cur_table];
}

Table& World::GetFutureTable() {
	return tables[(cur_table + 1) % 2];
}

void World::SwapTables() {
	cur_table = (cur_table + 1) % 2;
}

void World::LoadScript(string file_name) {
	lua_settop(L, 0); // Clear the stack

	int status = luaL_loadfile(L, file_name.c_str());
	if (status) {
		fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
		return;
	}

	int result = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (result) {
		fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
		return;
	}

	// lua_pop(L, 1); // Dump the script return value

	lua_getglobal(L, "SCRIPT_TYPE");
	string script_type = lua_tostring(L, -1);

	delete script;
	if (script_type == "CA") {
		script = new CAScript();
	}
	else if (script_type == "GRID") {
		script = new GridScript();
	}
	else {
		cout << "Script type unknown" << endl;
		script = new Script();
	}

	script->LoadScript();
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