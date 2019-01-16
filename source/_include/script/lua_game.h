#pragma once

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

static const struct luaL_Reg luac_game_metatable_client[] = {
	{ NULL, NULL }
};
static const struct luaL_Reg luac_game_metatable_server[] = {
	{ NULL, NULL }
};
static const struct luaL_Reg luac_game_metatable_shared[] = {
	{ NULL, NULL }
};