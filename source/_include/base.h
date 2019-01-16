#pragma once

// Win32
#include <Windows.h>
// SDL
#include <SDL.h>
// GLEW
#include <GL\glew.h>
// GL
#include <gl\GL.h>
// GLM
#pragma warning( disable : 4616 )
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#pragma warning( default : 4616 )
// Filesystem
#include "filesystem.h"
// Console
#include "console.h"
// Game
#include "game.h"

// Math
#define PI_CONSTANT 3.14159265359f

// Game
#define GAME_TITLE L"A 2D Game"
#define GAME_WINDOW_TITLE "2D Game Project"
#define GAME_VERSION_MAJ 0
#define GAME_VERSION_MIN 2
#define GAME_GLVERSION_MAJ 3
#define GAME_GLVERSION_MIN 2
#define GAME_QUICKQUIT true

// OS
#if defined( _WIN32)
#define OPERATING_SYSTEM L"windows"
#elif defined(__linux__ )
#define OPERATING_SYSTEM L"linux"
#elif defined(__APPLE __)
#define OPERATING_SYSTEM L"osx"
#endif

// Memory
#define SafeDelete( x ) if( x ) { delete x; x = 0; }
#define DestroyDelete( x ) if( x ) { x->destroy(); SafeDelete( x ); }
#define SafeDeleteArray( x ) if( x ) { delete[] x; x = 0; }
// Messagebox
#ifdef _WIN32
#define DisplayMessageBox( msg ) MessageBox( 0, msg, GAME_TITLE, MB_OK | MB_ICONERROR | MB_DEFBUTTON1 )
#elif
#define DisplayMessageBox( msg )
#endif
// Console
#define GetConsole() CGame::getInstance().getConsole()

// Lib PNG
#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL

// Physics
#define METERS_PER_PIXEL 1/12.0f
#define PIXELS_PER_METER 12.0f

#define FIX true