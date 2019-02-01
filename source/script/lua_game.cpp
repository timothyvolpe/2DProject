#include "base.h"
#include "config.h"
#include "script\luamanager.h"
#include "script\lua_game.h"

void lua_register_game( int scope, lua_State *pState )
{
	luaL_newmetatable( pState, LUA_GAMEMETA );
	lua_pushvalue( pState, -1 ); // push a copy of the metatable
	lua_setfield( pState, -1, "__index" ); // __index = copy of meta table
	
	// Only use one table, functions will be in metatable
	if( scope == LUA_SERVER )
		luaL_setfuncs( pState, luac_game_server, 0 );
	else
		luaL_setfuncs( pState, luac_game_client, 0 );
	luaL_setfuncs( pState, luac_game_shared, 0 );
	lua_setglobal( pState, LUA_GAMEOBJ );
}

int luaf_game_getresolution( lua_State *pState )
{
	int x, y;
	
	x = CGame::getInstance().getConfig()->m_resolutionX;
	y = CGame::getInstance().getConfig()->m_resolutionY;
	lua_pushinteger( pState, x );
	lua_pushinteger( pState, y );

	return 2;
}
int luaf_game_getresolutionX( lua_State *pState )
{
	int x;

	x = CGame::getInstance().getConfig()->m_resolutionX;
	lua_pushinteger( pState, x );

	return 1;
}
int luaf_game_getresolutionY( lua_State *pState )
{
	int y;

	y = CGame::getInstance().getConfig()->m_resolutionY;
	lua_pushinteger( pState, y );

	return 1;
}
int luaf_game_isfullscreen( lua_State *pState )
{
	bool fullscreen;

	fullscreen = CGame::getInstance().getConfig()->m_bFullscreen;
	lua_pushboolean( pState, (int)fullscreen );

	return 1;
}
int luaf_game_isborderless( lua_State *pState )
{
	bool borderless;

	borderless = CGame::getInstance().getConfig()->m_bBorderless;
	lua_pushboolean( pState, (int)borderless );

	return 1;
}