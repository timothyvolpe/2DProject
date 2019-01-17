#include "base.h"
#include "script\luamanager.h"
#include "script\lua_liboverrides.h"
#include "script\lua_game.h"
#include "script\lua_base.h"
#include <algorithm>

void CLuaManager::HandleLuaError( int error, std::wstring file, lua_State *pState )
{
	assert( pState );

	switch( error )
	{
	case LUA_ERRSYNTAX:
		PrintError( L"Lua syntax error: %hs\n", (pState ? lua_tostring( pState, -1 ) : "") );
		break;
	case LUA_ERRMEM:
		PrintError( L"Lua encountered a memory allocation error when loading %s!\n", file.c_str() );
		break;
	case LUA_ERRGCMM:
		PrintError( L"Lua encountered a garbage collection error when loading %s!\n", file.c_str() );
		break;
	case LUA_ERRRUN:
		PrintError( L"Lua runtime error: %hs\n", (pState ? lua_tostring( pState, -1 ) : "") );
		break;
	case LUA_ERRFILE:
		PrintError( L"Lua could not open file specified: %hs\n", (pState ? lua_tostring( pState, -1 ) : "") );
		break;
	case LUA_OK:
		break;
	default:
		PrintError( L"Unknown lua error code %d with file %s\n", error, file.c_str() );
		break;
	}

	//  Clear the stack
	lua_settop( pState, 0 );
}
void CLuaManager::PrintStack( lua_State *pState )
{
	int stackSize = lua_gettop( pState );

	assert( pState );

	PrintInfo( L"Lua Stack:\n" );
	PrintInfo( L"  Index\tTop\n" );
	for( int i = 1; i <= stackSize; i++ ) {
		PrintInfo( L"    %d\t", i );
		Print( pState, -i, true );
		PrintInfo( L"\n" );
	}
}
bool CLuaManager::Print( lua_State *pState, int index, bool specials )
{
	int scope;
	int argType;
	bool booltype;
	const char* pStr;
	const void* pPtr;

	assert( pState );

	argType = lua_type( pState, index );

	// Check for client/server
	lua_getglobal( pState, "CLIENT" );
	if( lua_toboolean( pState, -1 ) )
		scope = LUA_CLIENT;
	else
		scope = LUA_SERVER;
	lua_pop( pState, 1 );

	// Print the normal types
	if( specials )
	{
		switch( argType )
		{
		case LUA_TNONE:
			PrintGendered( scope, L"NONE" );
			return true;
		case LUA_TUSERDATA:
			pPtr = lua_topointer( pState, index );
			PrintGendered( scope, L"userdata: %p", pPtr );
			return true;
		case LUA_TTHREAD:
			pPtr = lua_topointer( pState, index );
			PrintGendered( scope, L"thread: %p", pPtr );
			return true;
		case LUA_TLIGHTUSERDATA:
			pPtr = lua_topointer( pState, index );
			PrintGendered( scope, L"lightuserdata: %p", pPtr );
			return true;
		}
	}
	// Print the normal types
	switch( argType )
	{
	case LUA_TNIL:
		PrintGendered( scope, L"nil" );
		return true;
	case LUA_TBOOLEAN:
		booltype = (bool)lua_toboolean( pState, index );
		PrintGendered( scope, L"%hs", (booltype ? "true" : "false") );
		return true;
	case LUA_TNUMBER:
	case LUA_TSTRING:
		pStr = lua_tostring( pState, index );
		PrintGendered( scope, L"%hs", pStr );
		return true;
	case LUA_TTABLE:
		pPtr = lua_topointer( pState, index );
		PrintGendered( scope, L"table: %p", pPtr );
		return true;
	case LUA_TFUNCTION:
		pPtr = lua_topointer( pState, index );
		PrintGendered( scope, L"function: %p", pPtr );
		return true;
	default:
		return false;
	}
}

CLuaManager::CLuaManager()
{
	m_pLuaStateClient = 0;
	m_pLuaStateServer = 0;
}
CLuaManager::~CLuaManager() {
}

bool CLuaManager::initialize()
{
	PrintInfo( L"Setting up Lua...\n" );
	PrintInfo( L"  %hs\n", LUA_COPYRIGHT );
	PrintInfo( L"  %hs\n", LUA_AUTHORS );
	// Create client and server states
	m_pLuaStateClient = luaL_newstate();
	if( !m_pLuaStateClient ) {
		PrintError( L"Failed to create Lua state\n" );
		return false;
	}
	if( !this->setupScope( LUA_CLIENT, m_pLuaStateClient ) )
		return false;

	m_pLuaStateServer = luaL_newstate();
	if( !m_pLuaStateServer ) {
		PrintError( L"Failed to create Lua state\n" );
		return false;
	}
	if( !this->setupScope( LUA_SERVER, m_pLuaStateServer ) )
		return false;

	return true;
}
void CLuaManager::destroy()
{
	PrintInfo( L"Cleaning up Lua...\n" );
	// Destroy client and server states
	if( m_pLuaStateServer ) {
		lua_close( m_pLuaStateServer );
		m_pLuaStateServer = 0;
	}
	if( m_pLuaStateClient ) {
		lua_close( m_pLuaStateClient );
		m_pLuaStateClient = 0;
	}
}

bool CLuaManager::setupScope( int scope, lua_State *pState )
{
	assert( scope == LUA_CLIENT || scope == LUA_SERVER );
	assert( pState );

	// Load default libs
	luaL_requiref( pState, "_G", luaopen_base, 1 );
	luaL_requiref( pState, "table", luaopen_table, 1 );
	luaL_requiref( pState, "string", luaopen_string, 1 );
	luaL_requiref( pState, "math", luaopen_math, 1 );
	luaL_requiref( pState, "debug", luaopen_debug, 1 );
	luaL_requiref( pState, "package", luaopen_package, 1 );
	lua_pop( pState, 6 );

	// Override certain default lib functions
	// Also add new global functions
	lua_getglobal( pState, "_G" );
	luaL_setfuncs( pState, luaf_liboverrides, 0 );
	luaL_setfuncs( pState, luaf_base, 0 );
	lua_pop( pState, 1 );

	// Setup globals
	switch( scope )
	{
	case LUA_CLIENT:
		lua_pushboolean( pState, true );
		lua_setglobal( pState, "CLIENT" );
		lua_pushboolean( pState, false );
		lua_setglobal( pState, "SERVER" );
		break;
	case LUA_SERVER:
		lua_pushboolean( pState, false );
		lua_setglobal( pState, "CLIENT" );
		lua_pushboolean( pState, true );
		lua_setglobal( pState, "SERVER" );
		break;
	}

	// Setup Classes

	// Game
	luaL_newmetatable( pState, "GameMetatable" );
	luaL_getmetatable( pState, "GameMetatable" );
	if( scope == LUA_SERVER )
		luaL_setfuncs( pState, luac_game_metatable_server, 0 );
	else
		luaL_setfuncs( pState, luac_game_metatable_client, 0 );
	luaL_setfuncs( pState, luac_game_metatable_shared, 0 );
	lua_setglobal( pState, "Game" );
	lua_pop( pState, 1 );

	// Clear stack 
	lua_settop( pState, 0 );

	return true;
}

bool CLuaManager::runScripts( int scope )
{
	int luaError;
	lua_State *pCurState;
	std::vector<boost::filesystem::path> *pRegScriptsVect;

	assert( scope == LUA_CLIENT || scope == LUA_SERVER );
	assert( m_pLuaStateClient && m_pLuaStateServer );

	// Select the scope
	switch( scope )
	{
	case LUA_CLIENT:
		pCurState = m_pLuaStateClient;
		pRegScriptsVect = &m_regScriptPathsClient;
		break;
	case LUA_SERVER:
		pCurState = m_pLuaStateServer;
		pRegScriptsVect = &m_regScriptPathsServer;
		break;
	default:
		return false;
	}

	// Load each script
	for( auto it = (*pRegScriptsVect).begin(); it != (*pRegScriptsVect).end(); it++ )
	{
		int rescount;

		// Load the file but don't execute
		luaError = luaL_loadfilex( pCurState, (*it).string().c_str(), "t" );
		// Check for errors
		if( luaError != LUA_OK ) {
			this->HandleLuaError( luaError, (*it).wstring(), pCurState );
			return false;
		}

		// Execute the file
		luaError = lua_pcall( pCurState, 0, LUA_MULTRET, 0 );
		// Check for errors
		if( luaError != LUA_OK ) {
			this->HandleLuaError( luaError, (*it).wstring(), pCurState );
			return false;
		}
		// Output return values to the console
		rescount = lua_gettop( pCurState );
		for( int i = 1; i <= rescount; i++ ) {
			CLuaManager::Print( pCurState, -i, true );
			PrintLua( L"\n" );
		}
		lua_pop( pCurState, rescount );

		// Call the initializer
		lua_getglobal( pCurState, "Game" );
		lua_getfield( pCurState, -1, "Initialize" );
		if( !lua_isfunction( pCurState, -1 ) ) {
			PrintWarn( L"No entry point was found in Lua\n" );
		}
		else
		{
			luaError = lua_pcall( pCurState, 0, 0, 0 );
			// Check for errors
			if( luaError != LUA_OK ) {
				this->HandleLuaError( luaError, (*it).wstring(), pCurState );
				return false;
			}
		}
		lua_pop( pCurState, 2 );
	}

	return true;
}
bool CLuaManager::registerScript( int scope, boost::filesystem::path relpath )
{
	boost::filesystem::path fullPath; 

	assert( scope == LUA_CLIENT || scope == LUA_SERVER );
	assert( m_pLuaStateClient && m_pLuaStateServer );

	// Construct the appropriate path
	switch( scope )
	{
	case LUA_CLIENT:
		fullPath = GameFilesystem::ConstructPath( SCRIPTS_CLIENTDIR, FILESYSTEM_DIR_SCRIPTS );
		break;
	case LUA_SERVER:
		fullPath = GameFilesystem::ConstructPath( SCRIPTS_SERVERDIR, FILESYSTEM_DIR_SCRIPTS );
		break;
	default:
		return false;
	}
	fullPath /= relpath;
	// Make sure it exists
	if( !GameFilesystem::IsValidFile( fullPath ) ) {
		PrintError( L"Could not find lua script (%s)\n", fullPath.wstring().c_str() );
		return false;
	}

	switch( scope )
	{
	case LUA_CLIENT:
		// Add it to the vector if it isn't already in there
		if( std::find( m_regScriptPathsClient.begin(), m_regScriptPathsClient.end(), fullPath ) != m_regScriptPathsClient.end() ) {
			PrintWarn( L"Script already registered (%s)\n", fullPath.wstring().c_str() );
			return true;
		}
		m_regScriptPathsClient.push_back( fullPath );
		break;
	case LUA_SERVER:
		// Add it to the vector if it isn't already in there
		if( std::find( m_regScriptPathsServer.begin(), m_regScriptPathsServer.end(), fullPath ) != m_regScriptPathsServer.end() ) {
			PrintWarn( L"Script already registered (%s)\n", fullPath.wstring().c_str() );
			return true;
		}
		m_regScriptPathsServer.push_back( fullPath );
		break;
	default:
		return false;
	}

	return true;
}