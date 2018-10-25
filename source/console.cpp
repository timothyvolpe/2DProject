#include "base.h"
#include "console.h"

CConsole::CConsole() {
#ifdef _WIN32
	m_hCout = 0;
	m_hCerr = 0;
	m_hLogFile = 0;
#endif
	m_pGLDebugName = "";
}
CConsole::~CConsole() {
}

void CConsole::setColor( unsigned char color )
{
#if defined( GAME_EXTERNAL_CONSOLE ) && defined( _WIN32 )
	switch( color )
	{
	case CONSOLE_COLOR_DEFAULT:
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F );
		break;
	case CONSOLE_COLOR_WARNING:
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN );
		break;
	case CONSOLE_COLOR_ERROR:
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_INTENSITY | FOREGROUND_RED );
		break;
	default:
		break;
	}
#endif
}

bool CConsole::initialize()
{
	boost::filesystem::path logPath;

	logPath = GameFilesystem::ConstructPath( LOG_FILE, FILESYSTEM_DIR_LOGS );

#if defined( GAME_EXTERNAL_CONSOLE ) && defined( _WIN32 )
	// Allocate the console
	AllocConsole();
	// Redirect cout to the console
	freopen_s( &m_hCout, "CONOUT$", "w", stdout );
	freopen_s( &m_hCerr, "CONOUT$", "w", stderr );

	// Open log file
	fopen_s( &m_hLogFile, logPath.string().c_str(), "w" );
	if( !m_hLogFile )
		m_bFileLogging = true;
	else
		m_bFileLogging = false;
#endif

	return true;
}
void CConsole::destroy()
{
#if defined( GAME_EXTERNAL_CONSOLE ) && defined( _WIN32 )
	// Close log file
	if( m_hLogFile ) {
		fclose( m_hLogFile );
		m_hLogFile = NULL;
	}
	// Destroy the out handle
	if( m_hCout ) {
		fclose( m_hCout );
		m_hCout = NULL;
	}
	if( m_hCerr ) {
		fclose( m_hCerr );
		m_hCerr = NULL;
	}

	FreeConsole();
#endif
}

void CConsole::printColor( std::wstring msg, unsigned char color )
{
#ifdef GAME_EXTERNAL_CONSOLE
	this->setColor( color );
	std::wcout << msg.c_str();
#endif
}

void CConsole::startGLDebug( const char *pName ) {
	GLenum err = glGetError();
	if( err != 0 ) {
		PrintError( L"Uncaught error at startGLDebug\n" );
	}
	m_pGLDebugName = pName;
}
void CConsole::endGLDebug() {
	this->checkGLError( true );
	m_pGLDebugName = "";
}

bool CConsole::checkGLError( bool debug )
{
	GLenum err = glGetError();
	if( err != 0 ) {
		if( !debug ) {
			PrintError( L"OpenGL error occured (code: %i, \'%hs\')\n", err, gluErrorString( err ) );
		}
		else {
			PrintError( L"[DEBUG][%hs] OpenGL error occured (code: %i, \'%hs\')\n", m_pGLDebugName, err, gluErrorString( err ) );
		}
		return false;
	}
	return true;
}

#ifdef _WIN32
FILE* CConsole::getLogFile() {
	return m_hLogFile;
}
#endif