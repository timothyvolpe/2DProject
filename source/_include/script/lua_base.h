#pragma once

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

int luaf_base_include( lua_State *pState );

static const struct luaL_Reg luaf_base[] = {
	{ "include", luaf_base_include },
	{ NULL, NULL }
};