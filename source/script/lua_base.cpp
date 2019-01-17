#include "base.h"
#include "script\lua_base.h"
#include "script\luamanager.h"
#include "filesystem.h"

int luaf_base_include( lua_State *pState )
{
	const char *pFilepath;
	lua_Debug dbgInfo;
	bool useRoot;
	boost::filesystem::path fullPath;
	int luaError, argcount;

	// Get the file path
	pFilepath = luaL_checkstring( pState, 1 );

	// Generate the file path from the current path
	useRoot = false;
	if( lua_getstack( pState, 1, &dbgInfo ) == 0 )
		useRoot = true;
	else {
		if( lua_getinfo( pState, "S", &dbgInfo ) == 0 )
			useRoot = true;
		if( !dbgInfo.source )
			useRoot = true;
		if( dbgInfo.source[0] != '@' )
			useRoot = true;
	}

	if( !useRoot ) {
		fullPath = dbgInfo.source+1; // remove @
		fullPath = fullPath.parent_path(); // get parent dir
		fullPath /= pFilepath;
	}
	else
		fullPath = pFilepath;

	// Clear the stack
	lua_settop( pState, 0 );
	// Load and execute the file
	luaError = luaL_loadfilex( pState, fullPath.string().c_str(), "t" );
	if( luaError != LUA_OK ) {
		CLuaManager::HandleLuaError( luaError, fullPath.wstring(), pState );
		return 0;
	}
	// Execute the file
	luaError = lua_pcall( pState, 0, LUA_MULTRET, 0 );
	// Check for errors
	if( luaError != LUA_OK ) {
		CLuaManager::HandleLuaError( luaError, fullPath.wstring(), pState );
		return 0;
	}
	// Return the outputs of pcall
	argcount = lua_gettop( pState );
	return argcount;
}