#pragma once

#define LUA_INTERFACEOBJ "Interface"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
};

void lua_register_interface( int scope, lua_State *pState );
int lua_interface_cleanup( lua_State *pState );

int luaf_interface_GetControl( lua_State *pState );

int luaf_interface_new( lua_State *pState );
int luaf_interface_delete( lua_State *pState );

int luaf_interface_activate( lua_State *pState );

static const struct luaL_Reg luac_interface_metatable[] = {	
	{ "__gc", luaf_interface_delete },
	{ "activate", luaf_interface_activate },
	{ NULL, NULL }
};

static const struct luaL_Reg luac_interface_static[] = {
	{ "new", luaf_interface_new },
	{ "GetControl", luaf_interface_GetControl },
	{ NULL, NULL }
};