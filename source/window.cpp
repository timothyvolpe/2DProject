#include "game.h"
#include "config.h"
#include "window.h"

CGameWindow::CGameWindow() {
	m_pSDLWindow = 0;
}
CGameWindow::~CGameWindow() {
}

bool CGameWindow::initialize()
{
	CGameConfig *pGameConfig = CGame::getInstance().getConfig();
	int windowWidth, windowHeight;
	Uint32 windowFlags;

	// Create the game window
	windowWidth = pGameConfig->m_windowWidth;
	windowHeight = pGameConfig->m_windowHeight;
	// Setup flags
	windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
	if( pGameConfig->m_bBorderless )
		windowFlags |= SDL_WINDOW_BORDERLESS;
	if( pGameConfig->m_bFullscreen )
		windowFlags |= SDL_WINDOW_FULLSCREEN;
	m_pSDLWindow = SDL_CreateWindow( GAME_WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, windowFlags );
	if( !m_pSDLWindow ) {
		PrintError( L"Failed to create the game window (%hs)", SDL_GetError() );
		return false;
	}

	return true;
}
void CGameWindow::destroy()
{
	if( m_pSDLWindow ) {
		SDL_DestroyWindow( m_pSDLWindow );
		m_pSDLWindow = 0;
	}
}

SDL_Window* CGameWindow::getSDLWindow() const {
	return m_pSDLWindow;
}