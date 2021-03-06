#pragma once

#define LUA_GAMEMETA "GameMetatable"
#define LUA_GAMEOBJ "Game"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

void lua_register_game( int scope, lua_State *pState );

int luaf_game_getresolution( lua_State *pState );
int luaf_game_getresolutionX( lua_State *pState );
int luaf_game_getresolutionY( lua_State *pState );
int luaf_game_isfullscreen( lua_State *pState );
int luaf_game_isborderless( lua_State *pState );

static const struct luaL_Reg luac_game_client[] = {
	{ "GetResolution", luaf_game_getresolution },
	{ "GetResolutionX", luaf_game_getresolutionX },
	{ "GetResolutionY", luaf_game_getresolutionY },
	{ "IsFullscreen", luaf_game_isfullscreen },
	{ "IsBorderless", luaf_game_isborderless },
	{ NULL, NULL }
};
static const struct luaL_Reg luac_game_server[] = {
	{ NULL, NULL }
};
static const struct luaL_Reg luac_game_shared[] = {
	{ NULL, NULL }
};