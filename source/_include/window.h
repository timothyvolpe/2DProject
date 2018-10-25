#pragma once
#include "base.h"

class CGameWindow
{
private:
	SDL_Window *m_pSDLWindow;
public:
	CGameWindow();
	~CGameWindow();

	bool initialize();
	void destroy();

	SDL_Window* getSDLWindow() const;
};