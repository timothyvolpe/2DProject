#include "base.h"
#include "game.h"
#include "config.h"
#include "window.h"
#include "input.h"
#include "graphics.h"
#include "world\world.h"
#include "interface\interfacemanager.h"
#include "script\luamanager.h"

const char* GetFreetypeError( FT_Error err )
{
#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  case e: return s;
#define FT_ERROR_START_LIST     switch (err) {
#define FT_ERROR_END_LIST       }
#include FT_ERRORS_H
	return "(Unknown error)";
}

CGame::CGame()
{
	m_freeTypeLib = 0;

	m_pConsole = 0;
	m_pConfig = 0;
	m_pGameWindow = 0;
	m_pInput = 0;
	m_pGraphics = 0;
	m_pWorld = 0;
	m_pInterfaceManager = 0;
	m_pLuaManager = 0;

	m_frameTimeMS = 0.0;

	m_bRunning = false;
}

CGame::~CGame() {
}

bool CGame::initialize()
{
	SDL_version sdlCompiled, sdlLinked;
	FT_Error freeTypeError;
	int freeTypeVerMaj, freeTypeVerMin, freeTypeVerPatch;

	// Start the game timer
	m_gameTimer.start();

	// Validate the directories
	if( !GameFilesystem::ValidateGameDir( FILESYSTEM_DIR_LOGS ) )
		return false;
	if( !GameFilesystem::ValidateGameDir( FILESYSTEM_DIR_CONFIG ) )
		return false;
	if( !GameFilesystem::ValidateGameDir( FILESYSTEM_DIR_SHADER ) )
		return false;
	if( !GameFilesystem::ValidateGameDir( FILESYSTEM_DIR_TEXTURES ) )
		return false;

	// Initialize the console
	m_pConsole = new CConsole();
	if( !m_pConsole->initialize() ) {
		DisplayMessageBox( L"Failed to initialize game console!" );
		return false;
	}

	// Print header
	PrintInfo( L"\n\t%s\n", GAME_TITLE );
	PrintInfo( L"\tCreated by Timothy Volpe\n" );
	PrintInfo( L"\tCopyright 2018-2019 (c)\n" );
	PrintInfo( L"\tVersion %d.%d\n\n", GAME_VERSION_MAJ, GAME_VERSION_MIN );

	// Initialize SDL
	SDL_VERSION( &sdlCompiled );
	PrintInfo( L"Initializing SDL (compiled with %d.%d.%d)...\n", sdlCompiled.major, sdlCompiled.minor, sdlCompiled.patch );
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		PrintError( L"Failed to initialize SDL library" );
		return false;
	}
	SDL_GetVersion( &sdlLinked );
	PrintInfo( L"Successfully initialized SDL (linked with %d.%d.%d)\n", sdlLinked.major, sdlLinked.minor, sdlLinked.patch );

	// Initialize FreeType
	PrintInfo( L"Initializing FreeType...\n" );
	freeTypeError = FT_Init_FreeType( &m_freeTypeLib );
	if( freeTypeError ) {
		PrintError( L"Failed to initialize FreeType (code: %d, see fterrdef.h for more info)\n", freeTypeError );
		return false;
	}
	FT_Library_Version( m_freeTypeLib, &freeTypeVerMaj, &freeTypeVerMin, &freeTypeVerPatch );
	PrintInfo( L"  FreeType version %d.%d.%d\n", freeTypeVerMaj, freeTypeVerMin, freeTypeVerPatch );

	// Create the config
	m_pConfig = new CGameConfig();
	if( !m_pConfig->initialize() )
		return false;
	// Create the game window
	m_pGameWindow = new CGameWindow();
	if( !m_pGameWindow->initialize() )
		return false;
	// Create the input
	
	m_pInput = new CInput();
	// Create the graphics
	m_pGraphics = new CGraphics();
	if( !m_pGraphics->initialize() )
		return false;
	
	// Create interface manager
	m_pInterfaceManager = new CInterfaceManager();
	if( !m_pInterfaceManager->initialize() )
		return false;
	// Create the world
	m_pWorld = new CWorld();
	if( !m_pWorld->initialize() )
		return false;

	// Initialize Lua
	m_pLuaManager = new CLuaManager();
	if( !m_pLuaManager->initialize() )
		return false;

	return true;
}
void CGame::destroy()
{
	// Destroy Lua
	DestroyDelete( m_pLuaManager );
	// Destroy world
	DestroyDelete( m_pWorld );
	// Destroy interface
	DestroyDelete( m_pInterfaceManager );
	// Destroy graphics
	DestroyDelete( m_pGraphics );
	// Delete the input
	SafeDelete( m_pInput );
	// Destroy the game window
	DestroyDelete( m_pGameWindow );
	// Destroy and save the config
	DestroyDelete( m_pConfig );

	// Quit SDL
	SDL_Quit();

	// Destroy the console
	DestroyDelete( m_pConsole );
}

bool CGame::startClient()
{
	PrintInfo( L"Starting client...\n" );

	// Register client sided lua scripts
	if( !m_pLuaManager->registerScript( LUA_CLIENT, "initClient.lua" ) ) {
		PrintError( L"initClient.lua is missing!\n" );
		return false;
	}
	// Load client sided lua scripts
	m_pLuaManager->runScripts( LUA_CLIENT );

	return true;
}
bool CGame::startServer()
{
	PrintInfo( L"Starting server...\n" );

	// Register client sided lua scripts
	if( !m_pLuaManager->registerScript( LUA_SERVER, "initServer.lua" ) ) {
		PrintError( L"initServer.lua is missing!\n" );
		return false;
	}
	// Load client sided lua scripts
	m_pLuaManager->runScripts( LUA_SERVER );

	return true;
}
bool CGame::startGame()
{
	// Start the client
	if( !this->startClient() )
		return false;
	// Start the server
	if( !this->startServer() )
		return false;
	// Enter the game loop
	if( !this->gameLoop() )
		return false;

	return true;
}

bool CGame::gameLoop()
{
	SDL_Event pollEvent;
	boost::chrono::microseconds currentFrameTimeUS;

	m_bRunning = true;
	while( m_bRunning )
	{
		while( SDL_PollEvent( &pollEvent ) )
			this->handleEvent( pollEvent );

		if( GAME_QUICKQUIT )
		{
			if( m_pInput->isKeyPress( SDL_SCANCODE_ESCAPE ) ) {
				SDL_Event quitEvent;
				quitEvent.type = SDL_QUIT;
				quitEvent.user.code = 0;
				SDL_PushEvent( &quitEvent );
			}
		}

		m_pGraphics->draw();
		m_pGraphics->update();

		// Get game time in microseconds
		currentFrameTimeUS = boost::chrono::duration_cast<boost::chrono::microseconds>( boost::chrono::nanoseconds( m_gameTimer.elapsed().wall ) );
		// Convert to double precision milliseconds
		auto currentFrameTimeMill = boost::chrono::duration_cast<boost::chrono::duration<double, boost::ratio<1, 1000>>>( currentFrameTimeUS );

		m_frameTimeMS = currentFrameTimeMill.count();

		m_gameTimer.start();

		m_pWorld->update( m_frameTimeMS );
		m_pInput->update();
	}

	return true;
}
void CGame::handleEvent( SDL_Event& pollEvent )
{
	switch( pollEvent.type )
	{
	case SDL_QUIT:
		PrintInfo( L"Quitting...\n" );
		m_bRunning = false;
		break;
	case SDL_KEYDOWN:
	case SDL_KEYUP:
	case SDL_MOUSEMOTION:
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		m_pInput->handleEvent( pollEvent );
	}
}

CConsole* CGame::getConsole() const {
	return m_pConsole;
}
CGameConfig* CGame::getConfig() const {
	return m_pConfig;
}
CGameWindow* CGame::getWindow() const {
	return m_pGameWindow;
}
CInput* CGame::getInput() const {
	return m_pInput;
}
CGraphics* CGame::getGraphics() const {
	return m_pGraphics;
}
CWorld* CGame::getWorld() const {
	return m_pWorld;
}
CInterfaceManager* CGame::getInterfaceManager() const {
	return m_pInterfaceManager;
}
CLuaManager* CGame::getLuaManager() const {
	return m_pLuaManager;
}

FT_Library CGame::getFreeType() {
	return m_freeTypeLib;
}

double CGame::getFrameTime() const {
	return m_frameTimeMS;
}