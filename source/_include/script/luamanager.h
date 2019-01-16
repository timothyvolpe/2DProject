#pragma once

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include <boost\filesystem.hpp>
#include <vector>

#define SCRIPTS_CLIENTDIR L"client"
#define SCRIPTS_SERVERDIR L"server"

enum {
	LUA_CLIENT = 1,
	LUA_SERVER = 2
};

// Prints different color for server/client
#define PrintGendered( scope, msg, ... ) \
	if( scope == LUA_SERVER ) { PrintLuaServer( msg, ##__VA_ARGS__ ); } \
	else { PrintLua( msg, ##__VA_ARGS__ ); }


class CLuaManager
{
private:
	lua_State* m_pLuaStateClient;
	lua_State* m_pLuaStateServer;

	bool setupScope( int scope, lua_State *pState );

	std::vector<boost::filesystem::path> m_regScriptPathsClient;
	std::vector<boost::filesystem::path> m_regScriptPathsServer;
public:
	static void HandleLuaError( int error, std::wstring file, lua_State *pState );
	static void PrintStack( lua_State *pState );
	static bool Print( lua_State *pState, int index, bool specials = false );

	CLuaManager();
	~CLuaManager();

	bool initialize();
	void destroy();

	bool runScripts( int scope );
	bool registerScript( int scope, boost::filesystem::path relpath );
};