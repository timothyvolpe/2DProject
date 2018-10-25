#include "base.h"
#include "game.h"

// This is all handled in the CMake files now

// SDL 2.0.3
/*#pragma comment( lib, "SDL2.lib" )
#pragma comment( lib, "SDL2main.lib" )
// GLEW
#ifdef _DEBUG
#pragma comment( lib, "glew32sd.lib" )
#else
#pragma comment( lib, "glew32s.lib" )
#endif
// GL
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )
// Lib PNG & zlib
#ifdef _DEBUG
#pragma comment( lib, "libpngd.lib" )
#pragma comment( lib, "zlib/zlibd.lib" )
#else
#pragma comment( lib, "libpng.lib" )
#pragma comment( lib, "zlib/zlib.lib" )
#endif
// Box2D
#pragma comment( lib, "Box2D.lib" )
// FreeType
#pragma comment( lib, "freetype.lib" )*/

int CALLBACK WinMain( HINSTANCE hInstacne, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	CGame& gameInstance = CGame::getInstance();

	// Initialize
	if( !gameInstance.initialize() )
		return -1;

	// Start the game
	if( !gameInstance.startGame() )
		return -2;
	// Destroy the game
	gameInstance.destroy();

	return 0;
}