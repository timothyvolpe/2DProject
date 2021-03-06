#pragma once

#include <SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <boost\timer\timer.hpp>

// FreeType Stuff

#define FT_HRES 64
#define FT_DPI 72

const char* GetFreetypeError( FT_Error err );

class CConsole;
class CGameConfig;
class CGameWindow;
class CInput;
class CGraphics;
class CWorld;
class CInterfaceManager;
class CLuaManager;

class CGame
{
private:
	FT_Library m_freeTypeLib;

	CConsole *m_pConsole;
	CGameConfig *m_pConfig;
	CGameWindow *m_pGameWindow;
	CInput *m_pInput;
	CGraphics *m_pGraphics;
	CWorld *m_pWorld;
	CInterfaceManager *m_pInterfaceManager;
	CLuaManager *m_pLuaManager;

	bool m_bRunning;

	boost::timer::cpu_timer m_gameTimer;
	double m_frameTimeMS;

	CGame();
	~CGame();

	bool startClient();
	bool startServer();
	bool gameLoop();

	void handleEvent( SDL_Event& pollEvent );
public:
	static CGame& getInstance() {
		static CGame instance;
		return instance;
	}

	CGame( CGame const& ) = delete;
	void operator=( CGame const& ) = delete;

	bool initialize();
	void destroy();

	bool startGame();

	CConsole* getConsole() const;
	CGameConfig* getConfig() const;
	CGameWindow* getWindow() const;
	CInput* getInput() const;
	CGraphics* getGraphics() const;
	CWorld* getWorld() const;
	CInterfaceManager* getInterfaceManager() const;
	CLuaManager* getLuaManager() const;

	FT_Library getFreeType();

	double getFrameTime() const;
};