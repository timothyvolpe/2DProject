#pragma once

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

// base
int luaf_print( lua_State *pState );
int luaf_error( lua_State *pState );

// base
static const struct luaL_Reg luaf_liboverrides[] ={
	{ "print", luaf_print },
	{ "error", luaf_error },
	{ NULL, NULL }
};