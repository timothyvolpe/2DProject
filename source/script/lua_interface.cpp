#include "base.h"
#include "script\lua_interface.h"
#include "script\luamanager.h"
#include "interface\interfacemanager.h"
#include "interface\interfacelabel.h"
#include "interface\interfacebase.h"
#include <sstream>

void lua_register_interface( int scope, lua_State *pState )
{
	if( scope == LUA_CLIENT )
	{
		//lua_register( pState, LUA_INTERFACEOBJ, luaf_interface_new );
		luaL_newlib( pState, luac_interface_static );
		lua_setglobal( pState, LUA_INTERFACEOBJ );
	}
}
int lua_interface_cleanup( lua_State *pState )
{
	return 0;
}

int luaf_interface_GetControl( lua_State *pState )
{
	std::string interfaceName;
	CInterfaceBase *pInterface;

	interfaceName = luaL_checkstring( pState, 1 );

	// Try  to find the control
	std::wstring ws;
	ws.assign( interfaceName.begin(), interfaceName.end() );
	pInterface = CGame::getInstance().getInterfaceManager()->getControlByName( ws );
	if( !pInterface )
	{
		std::stringstream ss;
		ss << "Could not find control with the name \'" << interfaceName.c_str() << "\'\n";
		lua_pushstring( pState, ss.str().c_str() );
		CLuaManager::Print( pState, -1 );
		lua_pushnil( pState );
		return 0;
	}

	// This function probably wont work

	// Push it as userdata
	//*static_cast<CInterfaceBase**>(lua_newuserdata( pState, sizeof( CInterfaceBase* ) )) = pInterface;

	return 1;
}

int luaf_interface_new( lua_State *pState )
{
	const char *pInterfaceType;
	std::string interfaceName;
	CInterfaceParent *pParent = 0;

	PrintInfo( L"Creating Interface!\n" );

	pInterfaceType = luaL_checkstring( pState, 1 );
	interfaceName = luaL_checkstring( pState, 2 );
	// Optional parent
	if( lua_isuserdata( pState, 3 ) )
	{
		CInterfaceBase *pPotentionalParent = *static_cast<CInterfaceBase**>(luaL_checkudata( pState, 3, LUA_INTERFACEOBJ ));
		if( pPotentionalParent->isParent() )
			pParent = static_cast<CInterfaceParent*>(pPotentionalParent);
		else {
			luaL_error( pState, "Interface control passed as parent is not a valid parent, ignoring\n" );
			pParent = 0;
		}
	}

	// Create userdata equal to new interface object
	CInterfaceBase **pInterface = static_cast<CInterfaceBase**>(lua_newuserdata( pState, sizeof( CInterfaceBase* ) ));
	if( strcmp( pInterfaceType, "Label" ) == 0 )
		(*pInterface) = new CInterfaceLabel();
	else {
		luaL_error( pState, "Invalid interface type \'%s\'\n", pInterfaceType );
		lua_pushnil( pState );
		return 1;
	}
	// Initialize the interface
	std::wstring ws;
	ws.assign( interfaceName.begin(), interfaceName.end() );
	(*pInterface)->initialize( std::wstring( ws ) );
	// Set optional parent
	if( pParent )
		(*pInterface)->setParent( pParent );

	// Create the metatable if we need to
	if( luaL_newmetatable( pState, LUA_INTERFACEOBJ ) ) {
		// Set meta methods
		luaL_setfuncs( pState, luac_interface_metatable, 0 );
		// Index points to itself
		lua_pushvalue( pState, -1 );
		lua_setfield( pState, -2, "__index" );
	}
	// Set userdata's metatable
	lua_setmetatable( pState, -2 );

	// Returns class userdata
	return 1;
}
int luaf_interface_delete( lua_State *pState )
{
	// Get the userdata
	CInterfaceBase *pInterface = *static_cast<CInterfaceBase**>(luaL_checkudata( pState, 1, LUA_INTERFACEOBJ ));
	// Cleanup interface
	DestroyDelete( pInterface );

	return 0;
}

int luaf_interface_activate( lua_State *pState )
{
	// Get the userdata
	CInterfaceBase *pInterface = *static_cast<CInterfaceBase**>(luaL_checkudata( pState, 1, LUA_INTERFACEOBJ ));

	// Check if it's already been activated
	if( pInterface->isActivated() ) {
		lua_pushboolean( pState, 1 );
		return 1;
	}

	// Register its name
	if( !CGame::getInstance().getInterfaceManager()->registerControl( pInterface ) )
		lua_pushboolean( pState, 0 );
	else
	{
		if( !pInterface->activate() )
			lua_pushboolean( pState, 0 );
		else
			lua_pushboolean( pState, 1 );
	}

	return 1;
}