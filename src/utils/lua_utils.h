#pragma once
#include <lua.hpp>
#include <stdint.h>
#include <string>

inline void luaU_get_at_index(lua_State *L, int index) {
	lua_pushinteger(L, index); // Push the index
	lua_gettable(L, -2); // Get the value at the index
}

inline void luaU_add_to_table(lua_State *L, int index, int value) {
	lua_pushinteger(L, index); // The index
	lua_pushinteger(L, value); // The value
	lua_rawset(L, -3); // Set to the table
}

inline unsigned int luaU_get_len(lua_State *L, int index) {
	lua_len(L, index);
	unsigned int len = lua_tointeger(L, index);
	lua_pop(L, 1);
	return len;
}

inline int luaU_get_integer(lua_State *L, int index) {
	int result = lua_tointeger(L, index);
	lua_pop(L, 1);
	return result;
}

inline std::string luaU_get_string(lua_State *L, int index) {
	std::string result = lua_tostring(L, index);
	lua_pop(L, 1);
	return result;
}