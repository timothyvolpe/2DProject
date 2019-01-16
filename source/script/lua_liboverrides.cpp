#include "base.h"
#include "script\luamanager.h"
#include "script\lua_liboverrides.h"
#include <sstream>

int luaf_print( lua_State *pState )
{	
	int stackSize;

	// Print each argument
	stackSize = lua_gettop( pState );
	while( stackSize > 0 )
	{
		if( !CLuaManager::Print( pState, -stackSize ) ) {
			luaL_error( pState, "bad argument #%d to 'print' (unknown type)\n", stackSize );
			return 0;
		}
		PrintLua( L"\t" );
		stackSize--;
	}
	PrintLua( L"\n" );

	return 0;
}
int luaf_error( lua_State *pState )
{
	int argcount;
	const char *pStr;
	int level;
	lua_Debug dbgInfo;

	argcount = lua_gettop( pState );
	// Get the message
	pStr = luaL_checkstring( pState, 1 );
	// Check for level
	if( argcount > 1 ) {
		level = (int)luaL_checkinteger( pState, 2 );
		if( level < 0 )
			level = 0;
	}
	else
		level = 2;

	// Try to get stack info
	if( lua_getstack( pState, level , &dbgInfo ) == 0 )
		level = 0;
	else {
		if( lua_getinfo( pState, "nSl", &dbgInfo ) == 0 )
			level = 0;
		if( !dbgInfo.short_src || dbgInfo.currentline == -1 )
			level = 0;
	}

	// No debug information
	if( level == 0 ) {
		lua_pushstring( pState, pStr );
		lua_error( pState );
		return 0;
	}

	// Print Error
	std::stringstream ss;
	ss << dbgInfo.short_src << ":" << dbgInfo.currentline << ": " << pStr;
	lua_pushstring( pState, ss.str().c_str() );
	lua_error( pState );

	return 0;
}