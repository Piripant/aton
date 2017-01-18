#pragma once
#include "../lua/lua.hpp"
#include <stdint.h>

inline void lua_get_at_index(lua_State *L, int index) {
    lua_pushinteger(L, index); // Push the index
	lua_gettable(L, -2); // Get the value at the index
}

inline void lua_add_to_table(lua_State *L, int index, int value) {
    lua_pushinteger(L, index); // The index
    lua_pushinteger(L, value); // The value
    lua_rawset(L, -3); // Set to the table
}

inline unsigned int lua_get_len(lua_State *L, int index) {
    lua_len(L, index);
	unsigned int len = lua_tointeger(L, index);
	lua_pop(L, 1);
    return len;
}

inline int lua_get_integer(lua_State *L, int index) {
	int result = lua_tointeger(L, -1);
	lua_pop(L, 1);
	return result;
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