#include "base.h"
#include "interface\interfacemanager.h"
#include "interface\interfacebase.h"
#include "interface\font.h"
#include "graphics.h"
#include "shader\shadermanager.h"
#include "shader\shader.h"
#include "config.h"
#include "texture.h"

#include "interface\interfacescreen.h"
#include "interface\interfacepanel.h"
#include "interface\interfacelabel.h"

#include <algorithm>
#include <boost\property_tree\ptree.hpp>
#include <boost\property_tree\info_parser.hpp>

CInterfaceManager::CInterfaceManager()
{
	m_vaoId = GL_INVALID_INDEX;
	m_vboId = GL_INVALID_INDEX;

	m_vertexCount = 0;
	m_indexCount = 0;
}
CInterfaceManager::~CInterfaceManager() {
}

bool CInterfaceManager::initialize()
{
	// Load the fonts
	if( !this->loadFonts() )
		return false;

	StartGLDebug( "CreateInterfaceBuffers" );

	// Create it if necessary
	if( m_vaoId == GL_INVALID_INDEX ) {
		glGenVertexArrays( 1, &m_vaoId );
		glGenBuffers( 1, &m_vboId );
	}
	glBindVertexArray( m_vaoId );
	// Vertex 
	glBindBuffer( GL_ARRAY_BUFFER, m_vboId );
	// Reallocate the new buffer of the proper size
	glVertexAttribIPointer( 0, 2, GL_UNSIGNED_SHORT, sizeof( InterfaceVertex ), (GLvoid*)offsetof( InterfaceVertex, pos ) );
	glVertexAttribIPointer( 1, 2, GL_UNSIGNED_SHORT, sizeof( InterfaceVertex ), (GLvoid*)offsetof( InterfaceVertex, size ) );
	glVertexAttribPointer( 2, 4, GL_UNSIGNED_SHORT, GL_TRUE, sizeof( InterfaceVertex ), (GLvoid*)offsetof( InterfaceVertex, texcoords ) );
	glVertexAttribIPointer( 3, 1, GL_BYTE, sizeof( InterfaceVertex ), (GLvoid*)offsetof( InterfaceVertex, textureId ) );
	glVertexAttribPointer( 4, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof( InterfaceVertex ), (GLvoid*)offsetof( InterfaceVertex, color ) );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glEnableVertexAttribArray( 2 );
	glEnableVertexAttribArray( 3 );
	glEnableVertexAttribArray( 4 );
	glBufferData( GL_ARRAY_BUFFER, MAX_QUADS_PER_BUFFER*sizeof( InterfaceVertex ), 0, GL_DYNAMIC_DRAW );

	EndGLDebug();

	// Create a test screen
	CInterfaceScreen *pTestScreen;
	CInterfacePanel *pTestPanel;
	CInterfaceLabel* pTestLabel;

	pTestScreen = new CInterfaceScreen();
	pTestScreen->initialize( L"DebugScreen" );
	pTestScreen->setSize( glm::ivec2( 100, 25 ) );
	pTestScreen->setPosition( glm::ivec2( 200, 0 ) );
	pTestScreen->activate();

	pTestPanel = new CInterfacePanel();
	pTestPanel->initialize( L"DebugPanel" );
	pTestPanel->setSize( glm::ivec2( 50, 50 ) );
	pTestPanel->setPosition( glm::ivec2( 10, 0 ) );
	pTestScreen->addChild( pTestPanel );
	pTestPanel->activate();

	pTestLabel = new CInterfaceLabel();
	pTestLabel->initialize( L"DebugLabel" );
	pTestLabel->setSize( glm::ivec2( 50, 15 ) );
	pTestLabel->setPosition( glm::ivec2( 70, 100 ) );
	pTestScreen->addChild( pTestLabel );
	pTestLabel->activate();

	this->addScreen( pTestScreen );

	return true;
}
void CInterfaceManager::destroy()
{
	// Delete the fonts
	for( auto it = m_fontMap.begin(); it != m_fontMap.end(); it++ ) {
		DestroyDelete( (*it).second );
	}
	// Delete the quads
	if( m_vaoId != GL_INVALID_INDEX ) {
		glDeleteBuffers( 1, &m_vboId );
		glDeleteVertexArrays( 1, &m_vaoId );
	}
	m_vboId = m_vaoId = GL_INVALID_INDEX;
	m_fontMap.clear();
}

bool CInterfaceManager::loadFontFromFile( bool systemFont, std::wstring const& fontId, std::wstring const& fontFile )
{
	CFont *pFont;

	// Check to make sure it there isnt one with the same name
	auto it = m_fontMap.find( fontId );
	if( it != m_fontMap.end() ) {
		PrintWarn( L"  Found repeat font id, ignoring (%s)\n", fontId.c_str() );
		return true;
	}

	// If not, add new font
	pFont = new CFont();
	if( !pFont->initialize( systemFont, fontFile ) )
		return false;

	m_fontMap.insert( std::pair<std::wstring, CFont*>( fontId, pFont ) );

	return true;
}
bool CInterfaceManager::loadFonts()
{
	boost::filesystem::path configPath;
	boost::property_tree::wptree infoTree, fontTree, systemFonts, gameFonts;

	PrintInfo( L"Loading fonts...\n" );

	// Get the config path
	configPath = GameFilesystem::ConstructPath( GAME_FONT_FILE, FILESYSTEM_DIR_FONTS );

	// Load the fonts
	try {
		PrintInfo( L"  Loading font definitions...\n" );
		boost::property_tree::read_info( configPath.string(), infoTree );

		// Get the proper tree for the operating system
		fontTree = infoTree.get_child( OPERATING_SYSTEM );
		// Get the system fonts
		systemFonts = fontTree.get_child( L"system_fonts" );
		gameFonts = fontTree.get_child( L"game_fonts" );

		// Load each font
		for( auto v : systemFonts ) {
			std::wstring fontId = v.first.c_str();
			std::wstring fontFile = v.second.get_value( L"NOFONT" );
			if( fontFile == L"NOFONT" ) {
				PrintWarn( L"  Ignoring font %s with no font file given\n", fontId.c_str() );
				continue;
			}
			PrintInfo( L"  Loading system font \'%s\'...\n", fontFile.c_str() );
			if( !this->loadFontFromFile( true, fontId, fontFile ) )
				return false;
		}
	}
	catch( const boost::property_tree::info_parser_error& e ) {
		PrintError( L"  Failed to load font definitions (%hs)\n", e.what() );
		return false;
	}
	catch( const boost::property_tree::ptree_bad_path& e ) {
		PrintError( L"  Font definitions file missing information (%hs)\n", e.what() );
		return false;
	}

	return true;
}

bool CInterfaceManager::addScreen( CInterfaceScreen *pScreen )
{
	// Check if it's already active
	if( std::find( m_activeScreens.begin(), m_activeScreens.end(), pScreen ) != m_activeScreens.end() ) {
		PrintWarn( L"Screen \'%s\' already active\n", pScreen->getName().c_str() );
		return false;
	}
	// Add it to the list
	m_activeScreens.push_back( pScreen );

	return true;
}
bool CInterfaceManager::removeScreen( CInterfaceScreen *pScreen )
{
	// Check if it's already active
	auto it = std::find( m_activeScreens.begin(), m_activeScreens.end(), pScreen );
	if( it == m_activeScreens.end() ) {
		PrintWarn( L"Screen \'%s\' was not active\n", pScreen->getName().c_str() );
		return false;
	}
	// Add it to the list
	m_activeScreens.erase( it );

	return true;
}

void CInterfaceManager::drawChildren( glm::mat4 interfaceOrthoMat, std::vector<CInterfaceBase*> children )
{
	std::vector<CInterfaceBase*> childsChildren;

	for( auto it = children.begin(); it != children.end(); it++ )
	{
		// Draw the child
		(*it)->onDraw();
		// Check if this children also has children, as we need to render those too
		if( (*it)->isParent() ) {
			CInterfaceParent *pParent = reinterpret_cast<CInterfaceParent*>((*it));
			childsChildren = pParent->getChildren();
			if( childsChildren.size() > 0 )
				this->drawChildren( interfaceOrthoMat, childsChildren );
		}
	}
}
void CInterfaceManager::draw( glm::mat4 interfaceOrthoMat )
{
	glm::mat4 mvpMatrix;
	CShaderProgram *pInterfaceProgram = CGame::getInstance().getGraphics()->getShaderManager()->getShaderProgram( L"interface" );
	std::vector<CInterfaceBase*> children;
	InterfaceVertex *pVerticesPtr;
	size_t quadCount;

	// Draw the quads to the buffer
	for( auto it = m_activeScreens.begin(); it != m_activeScreens.end(); it++ )
	{
		(*it)->onDraw();

		// Draw the screens children
		children = (*it)->getChildren();
		this->drawChildren( interfaceOrthoMat, children );
	}


	StartGLDebug( "FlushInterfaceArray" );

	glBindVertexArray( m_vaoId );
	glBindBuffer( GL_ARRAY_BUFFER, m_vboId );

	pVerticesPtr = (InterfaceVertex*)glMapBufferRange( GL_ARRAY_BUFFER, 0, MAX_QUADS_PER_BUFFER*sizeof( InterfaceVertex ), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );
	if( !pVerticesPtr ) {
		PrintWarn( L"Draw error\n" );
		EndGLDebug();
		return;
	}

	GLushort maxShort = (GLushort)(-1);

	// Form the vertex buffer
	for( size_t i = 0; i < m_interfaceQuads.size(); i++ )
	{
		pVerticesPtr[i].pos = glm::lowp_uvec2( m_interfaceQuads[i].absPos );
		pVerticesPtr[i].size = glm::lowp_uvec2( m_interfaceQuads[i].absSize );
		pVerticesPtr[i].texcoords = glm::lowp_uvec4( m_interfaceQuads[i].tex_start * (float)maxShort, m_interfaceQuads[i].tex_stop * (float)maxShort );
		pVerticesPtr[i].textureId = m_interfaceQuads[i].textureId;
		pVerticesPtr[i].color[0] = m_interfaceQuads[i].color.r;
		pVerticesPtr[i].color[1] = m_interfaceQuads[i].color.g;
		pVerticesPtr[i].color[2] = m_interfaceQuads[i].color.b;
	}
	quadCount = m_interfaceQuads.size();
	m_interfaceQuads.clear();

	pVerticesPtr = 0;
	glUnmapBuffer( GL_ARRAY_BUFFER );

	EndGLDebug();

	StartGLDebug( "DrawInterfaceBuffer" );

	// Bind interface program
	CGame::getInstance().getGraphics()->getShaderManager()->bind( pInterfaceProgram );

	// Setup matrices
	mvpMatrix = interfaceOrthoMat;
	glUniformMatrix4fv( pInterfaceProgram->getUniform( "MVPMatrix" ), 1, GL_FALSE, &mvpMatrix[0][0] );

	// Bind textures
	this->getFont( L"DEFAULT_FONT" )->getFontMap()->bind( 0 );

	//glEnable( GL_STENCIL_TEST );

	/*glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	glDepthMask( GL_FALSE );
	glStencilFunc( GL_ALWAYS, 1, 0xFF );
	glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
	glStencilMask( 0xFF );
	glClear( GL_STENCIL_BUFFER_BIT );

	glDrawArrays( GL_POINTS, 0, quadCount );*/

	/*glClear( GL_STENCIL_BUFFER_BIT );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask( GL_TRUE );
	glStencilFunc( GL_NEVER, 1, 0xFF );
	glStencilOp( GL_REPLACE, GL_KEEP, GL_KEEP );
	glStencilMask( 0xFF );*/

	glDrawArrays( GL_POINTS, 0, quadCount );

	//glDisable( GL_STENCIL_TEST );

	EndGLDebug();
}

void CInterfaceManager::drawQuad( InterfaceQuad interfaceQuad ) {
	m_interfaceQuads.push_back( interfaceQuad );
}

CFont* CInterfaceManager::getFont( const wchar_t* fontName )
{
	// Find it in the map
	auto it = m_fontMap.find( fontName );
	if( it == m_fontMap.end() ) {
		PrintWarn( L"Couldn't find font \'%s\'\n", fontName );
		return 0;
	}
	return (*it).second;
}