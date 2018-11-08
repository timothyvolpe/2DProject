#include "base.h"
#include "graphics.h"
#include "window.h"
#include "config.h"
#include "input.h"
#include "shader\shadermanager.h"
#include "shader\shader.h"
#include "world\world.h"
#include "texturemanager.h"
#include "interface\interfacemanager.h"

float CGraphics::getLayerPosition( unsigned char layer ) {
	return -(255 - layer)*LAYER_SIZE;
}

CGraphics::CGraphics() {
	m_pGameWindowHandle = 0;
	m_GLContext = 0;
	m_orthoMatrix = glm::mat4( 1.0f );
	m_interfaceOrthoMatrix = glm::mat4( 1.0f );
	m_pShaderManager = 0;
	m_pTextureManager = 0;

	m_pixelsPerMeter = 50;

	m_bWireframe = false;
}
CGraphics::~CGraphics() {
}

bool CGraphics::initialize()
{
	CGameConfig *pConfig = CGame::getInstance().getConfig();
	GLenum glewError;
	GLenum contextErr;

	// Store the game window
	m_pGameWindowHandle = CGame::getInstance().getWindow()->getSDLWindow();

	// Request GL version
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, GAME_GLVERSION_MAJ );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, GAME_GLVERSION_MIN );
	// Enable multisampling
	if( pConfig->m_bAntialiasing ) {
		SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
		if( pConfig->m_multisampleSamples != 2 && pConfig->m_multisampleSamples != 4 ) {
			PrintWarn( L"Invalid multisample sample count %d (valid values are 2 and 4), defaulting to 2\n", pConfig->m_multisampleSamples );
			pConfig->m_multisampleSamples = 2;
		}
		SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, pConfig->m_multisampleSamples );
	}
	// GL attributes
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

	// Create the GL context
	m_GLContext = SDL_GL_CreateContext( m_pGameWindowHandle );
	if( !m_GLContext ) {
		PrintError( L"Failed to create OpenGL context (%hs)", SDL_GetError() );
		return false;
	}

	// Check for errors in context creation
	while( contextErr = glGetError() ) {
		PrintError( L"Failed to create OpenGL context with code %d (\'%hs\')", contextErr, gluErrorString( contextErr ) );
		return false;
	}

	StartGLDebug( "SetupOpenGL" );

	// Initialize GLEW
	PrintInfo( L"Initializing GLEW...\n" );
	glewError = glewInit();
	if( glewError != GLEW_OK ) {
		PrintError( L"Failed to initialize GLEW (code: %d)", glewError );
		return false;
	}

	// Print some OpenGL info
	PrintInfo( L"  Requested OpenGL version %d.%d\n", GAME_GLVERSION_MAJ, GAME_GLVERSION_MIN );
	PrintInfo( L"  Using OpenGL version %hs\n  Using GLSL version %hs\n", glGetString( GL_VERSION ), glGetString( GL_SHADING_LANGUAGE_VERSION ) );
	PrintInfo( L"  Vendor: %hs\n  Renderer: %hs\n", glGetString( GL_VENDOR ), glGetString( GL_RENDERER ) );

	this->setupGraphics();

	// Initialize shaders
	m_pShaderManager = new CShaderManager();
	if( !m_pShaderManager->initialize() )
		return false;

	this->updateProjection();

	// Initialize texture manager
	m_pTextureManager = new CTextureManager();
	if( !m_pTextureManager->initialize() )
		return false;

	EndGLDebug();

	return true;
}
void CGraphics::destroy()
{
	// Destroy textures
	DestroyDelete( m_pTextureManager );
	// Destroy shader manager
	DestroyDelete( m_pShaderManager );

	// Delete the context
	if( m_GLContext ) {
		SDL_GL_DeleteContext( m_GLContext );
		m_GLContext = 0;
	}
	m_pGameWindowHandle = 0;
}

void CGraphics::setupGraphics()
{
	CGameConfig *pConfig = CGame::getInstance().getConfig();

	// VSync
	if( pConfig->m_bVsync )
		SDL_GL_SetSwapInterval( 1 );
	else
		SDL_GL_SetSwapInterval( 0 );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glEnable( GL_TEXTURE_2D );
	glEnable( GL_MULTISAMPLE );
	//glEnable( GL_CULL_FACE );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );
}

void CGraphics::update()
{
	CGameConfig *pConfig = CGame::getInstance().getConfig();
	CInput *pInput = CGame::getInstance().getInput();

	if( pInput->isKeyPress( pConfig->getKeybind( KEYBIND_TOGGLEWIREFRAME ) ) ) {
		m_bWireframe = !m_bWireframe;
		if( m_bWireframe )
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		else
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}
}
void CGraphics::draw()
{
	glm::mat4 viewMatrix;
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	viewMatrix = glm::lookAt( glm::vec3( 0.0f, 0.0f, 1.0f ), glm::vec3( 0.0f, 0.0f, 0.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );

	

	// Draw the world
	CGame::getInstance().getWorld()->draw( m_orthoMatrix );
	// Draw the interface
	CGame::getInstance().getInterfaceManager()->draw( m_interfaceOrthoMatrix );

	SDL_GL_SwapWindow( m_pGameWindowHandle );

	// Check for GL errors
#ifndef DEBUG_OPENGL
	if( !CheckGLError() ) {
		PrintWarn( L"Consider turning on GL debugging\n" );
	}
#endif
}

void CGraphics::updateProjection()
{
	CGameConfig *pConfig = CGame::getInstance().getConfig();
	m_orthoMatrix = glm::ortho( 0.0f, (float)pConfig->m_resolutionX / m_pixelsPerMeter, 0.0f, (float)pConfig->m_resolutionY / m_pixelsPerMeter, 0.0f, 256.0f );
	m_interfaceOrthoMatrix = glm::ortho( 0.0f, (float)pConfig->m_resolutionX, (float)pConfig->m_resolutionY, 0.0f, 0.0f, 256.0f );
	glViewport( 0, 0, pConfig->m_resolutionX, pConfig->m_resolutionY );
}

SDL_GLContext CGraphics::getGLContext() const {
	return m_GLContext;
}

glm::mat4 CGraphics::getOrthographicMatrix() const {
	return m_orthoMatrix;
}
glm::mat4 CGraphics::getInterfaceOrthoMatrix() const {
	return m_interfaceOrthoMatrix;
}
CShaderManager* CGraphics::getShaderManager() const {
	return m_pShaderManager;
}
CTextureManager* CGraphics::getTextureManager() const {
	return m_pTextureManager;
}
int CGraphics::getPixelsPerMeter() const {
	return m_pixelsPerMeter;
}
void CGraphics::setPixelsPerMeter( int pixelsPerMeter ) {
	m_pixelsPerMeter = pixelsPerMeter;
}