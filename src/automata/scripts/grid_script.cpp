#include "grid_script.h"

#include <automata/table.h>
#include <automata/world.h>
#include <utils/lua_utils.h>
#include <lua.hpp>

#include <iostream>

int GetGrid(Table& grid, int x, int y) {
	if (x >= 0 && y >= 0 && grid.InBounds(x, y)) {
		return grid.GetCellState(x, y);
	}
	return -1;
}

static int GetCurrentGrid(lua_State *L) {
	int x = lua_tointeger(L, 1);
	int y = lua_tointeger(L, 2);

	Table& grid = World::GetCurrentTable();
	int state = GetGrid(grid, x, y);
	lua_pushinteger(L, state);
	return 1;
}

static int GetFutureGrid(lua_State *L) {
	int x = lua_tointeger(L, 1);
	int y = lua_tointeger(L, 2);

	Table& grid = World::GetFutureTable();
	int state = GetGrid(grid, x, y);
	lua_pushinteger(L, state);
	return 1;
}

void SetGrid(Table& grid, int x, int y, uint8_t state) {
	if (state <= 255 && x >= 0 && y >= 0 && grid.InBounds(x, y)) {
		grid.GetCellState(x, y) = state;
	}
}

static int SetCurrentGrid(lua_State *L) {
	int x = lua_tointeger(L, 1);
	int y = lua_tointeger(L, 2);
	uint8_t state = lua_tointeger(L, 3);

	Table& grid = World::GetCurrentTable();
	SetGrid(grid, x, y, state);
	return 0;
}

static int SetFutureGrid(lua_State *L) {
	int x = lua_tointeger(L, 1);
	int y = lua_tointeger(L, 2);
	uint8_t state = lua_tointeger(L, 3);
	
	Table& grid = World::GetFutureTable();
	SetGrid(grid, x, y, state);
	return 0;
}

static int SwapGrids(lua_State *L) {
	World::SwapTables();
	return 0;
}

static int GetHeight(lua_State *L) {
	lua_pushinteger(L, World::GetCurrentTable().height);
	return 1;
}

static int GetWidth(lua_State *L) {
	lua_pushinteger(L, World::GetCurrentTable().width);
	return 1;
}

void lua_addfunction(lua_State *L, const char *name, lua_CFunction f) {
	lua_pushstring(L, name);
	lua_pushcfunction(L, f);
	lua_settable(L, -3);
}

void GridScript::LoadScript() {
	lua_newtable(World::L);
	lua_addfunction(World::L, "set_current", SetCurrentGrid);
	lua_addfunction(World::L, "set_future", SetFutureGrid);
	lua_addfunction(World::L, "get_current", GetCurrentGrid);
	lua_addfunction(World::L, "get_future", GetFutureGrid);
	lua_addfunction(World::L, "get_height", GetHeight);
	lua_addfunction(World::L, "get_width", GetWidth);
	lua_addfunction(World::L, "swap", SwapGrids);
	lua_setglobal(World::L, "grids");
}

void GridScript::Step() {
	lua_getglobal(World::L, "world_step");
	lua_pcall(World::L, 0, 0, 0);
}