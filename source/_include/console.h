#pragma once
#include <string>
#include <iostream>

#define GAME_EXTERNAL_CONSOLE
#define LOG_FILE L"console.log"

#define DEBUG_OPENGL

enum : unsigned char
{
	CONSOLE_COLOR_DEFAULT = 0,
	CONSOLE_COLOR_WARNING,
	CONSOLE_COLOR_ERROR
};

#if defined( GAME_EXTERNAL_CONSOLE ) & defined( _WIN32 )
#define PrintInfo( msg, ... ) CGame::getInstance().getConsole()->printColor( L"", CONSOLE_COLOR_DEFAULT ); wprintf(msg, ##__VA_ARGS__ ); fwprintf( CGame::getInstance().getConsole()->getLogFile(), msg, ##__VA_ARGS__ )
#define PrintWarn( msg, ... ) CGame::getInstance().getConsole()->printColor( L"WARN: ", CONSOLE_COLOR_WARNING ); wprintf(msg, ##__VA_ARGS__ ); fwprintf( CGame::getInstance().getConsole()->getLogFile(), msg, ##__VA_ARGS__ )
#define PrintError( msg, ... ) CGame::getInstance().getConsole()->printColor( L"ERROR: ", CONSOLE_COLOR_ERROR ); wprintf(msg, ##__VA_ARGS__ ); fwprintf( CGame::getInstance().getConsole()->getLogFile(), msg, ##__VA_ARGS__ )
#elif
#define PrintInfo( msg, ... )
#define PrintWarn( msg, ... )
#define PrintError( msg, ... )
#endif

#if defined( _DEBUG )
#define CheckGLError() CGame::getInstance().getConsole()->checkGLError( false )
#else
#define CheckGLError()
#endif
#if defined( DEBUG_OPENGL )
#define StartGLDebug( name ) CGame::getInstance().getConsole()->startGLDebug( name )
#define EndGLDebug() CGame::getInstance().getConsole()->endGLDebug();
#else
#define StartGLDebug()
#define EndGLDebug()
#endif

class CConsole
{
private:
#ifdef _WIN32
	FILE *m_hCout, *m_hCerr;
	FILE *m_hLogFile;
#endif

	bool m_bFileLogging;

	void setColor( unsigned char color );

	const char *m_pGLDebugName;
public:
	CConsole();
	~CConsole();

	bool initialize();
	void destroy();

	void printColor( std::wstring msg, unsigned char color );

	void startGLDebug( const char *pName );
	void endGLDebug();

	bool checkGLError( bool debug );

#ifdef _WIN32
	FILE* getLogFile();
#endif
};