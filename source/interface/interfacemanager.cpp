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

#include <algorithm>
#include <boost\property_tree\ptree.hpp>
#include <boost\property_tree\info_parser.hpp>

CInterfaceManager::CInterfaceManager()
{
	m_vaoId = GL_INVALID_INDEX;
	m_vboId = GL_INVALID_INDEX;
	m_iboId = GL_INVALID_INDEX;

	m_vertexCount = 0;
	m_indexCount = 0;

	m_quadMaxBufferSize = 16384;
}
CInterfaceManager::~CInterfaceManager() {
}

bool CInterfaceManager::initialize()
{
	// Load the fonts
	if( !this->loadFonts() )
		return false;

	this->allocateQuadBuffer();

	// Create a test screen
	CInterfaceScreen *pTestScreen;
	CInterfacePanel *pTestPanel;

	pTestScreen = new CInterfaceScreen();
	pTestScreen->initialize( L"DebugScreen" );
	pTestScreen->setSize( glm::ivec2( 100, 25 ) );
	pTestScreen->setPosition( glm::ivec2( 200, 0 ) );
	pTestScreen->activate();

	pTestPanel = new CInterfacePanel();
	pTestPanel->initialize( L"DebugPanel" );
	pTestPanel->setSize( glm::ivec2( 20, 40 ) );
	pTestPanel->setPosition( glm::ivec2( 0, 0 ) );
	pTestScreen->addChild( pTestPanel );
	pTestPanel->activate();

	this->addScreen( pTestScreen );

	return true;
}
void CInterfaceManager::destroy() {
	// Delete the fonts
	for( auto it = m_fontMap.begin(); it != m_fontMap.end(); it++ ) {
		DestroyDelete( (*it).second );
	}
	// Delete the quads
	if( m_vaoId != GL_INVALID_INDEX ) {
		glDeleteBuffers( 1, &m_vboId );
		glDeleteBuffers( 1, &m_iboId );
		glDeleteVertexArrays( 1, &m_vaoId );
	}
	m_vboId = m_iboId = m_vaoId = GL_INVALID_INDEX;
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
	glm::mat4 mvpMatrix, modelMatrix;
	CShaderProgram *pInterfaceProgram = CGame::getInstance().getGraphics()->getShaderManager()->getShaderProgram( L"interface" );
	std::vector<CInterfaceBase*> children;

	// Do we need to flush the quads?
	if( m_interfaceQuads.size() > 0 )
		this->flushQuads();

	// Bind interface program
	CGame::getInstance().getGraphics()->getShaderManager()->bind( pInterfaceProgram );

	StartGLDebug( "DrawInterface" );
	
	// Setup matrices
	mvpMatrix = interfaceOrthoMat;

	glUniformMatrix4fv( pInterfaceProgram->getUniform( "MVPMatrix" ), 1, GL_FALSE, &mvpMatrix[0][0] );

	// Bind textures
	this->getFont( L"DEFAULT_FONT" )->getFontMap()->bind( 0 );

	glBindVertexArray( m_vaoId );
	glDrawArrays( GL_LINES_ADJACENCY, 0, m_vertexCount );
	//glDrawElements( GL_POINTS, m_indexCount, GL_UNSIGNED_INT, 0 );


	//glDrawArrays( GL_TRIANGLES, 0, 11 );

	// Render the active screens
 	for( auto it = m_activeScreens.begin(); it != m_activeScreens.end(); it++ )
	{
		glm::ivec2 interfacePos, interfaceSize;

		interfaceSize = (*it)->getSize();
		interfacePos = (*it)->getAbsolutePosition();

		(*it)->onDraw();

		/*glBegin( GL_TRIANGLES );
		{
			glColor3f( 1.0f, 0.0f, 0.0f );
			glVertex3i( interfacePos.x+interfaceSize.x, interfacePos.y, 200 );
			glColor3f( 1.0f, 0.0f, 0.0f );
			glVertex3i( interfacePos.x, interfacePos.y, 200 );
			glColor3f( 1.0f, 0.0f, 0.0f );
			glVertex3i( interfacePos.x, interfacePos.y+interfaceSize.y, 200 );
			glColor3f( 1.0f, 0.0f, 0.0f );
			glVertex3i( interfacePos.x, interfacePos.y+interfaceSize.y, 200 );
			glColor3f( 1.0f, 0.0f, 0.0f );
			glVertex3i( interfacePos.x+interfaceSize.x, interfacePos.y+interfaceSize.y, 200 );
			glColor3f( 1.0f, 0.0f, 0.0f );
			glVertex3i( interfacePos.x+interfaceSize.x, interfacePos.y, 200 );
		}
		glEnd();*/

		// Draw the screens children
		children = (*it)->getChildren();
		this->drawChildren( interfaceOrthoMat, children );
	}

	EndGLDebug();
}

void CInterfaceManager::allocateQuadBuffer()
{
	unsigned int *pIndices;
	unsigned int quadCount;

	StartGLDebug( "AllocateInterfaceQuadBuffer" );

	// Create it if necessary
	if( m_vaoId == GL_INVALID_INDEX ) {
		glGenVertexArrays( 1, &m_vaoId );
		glGenBuffers( 1, &m_vboId );
		glGenBuffers( 1, &m_iboId );
	}
	glBindVertexArray( m_vaoId );
	// Vertex 
	glBindBuffer( GL_ARRAY_BUFFER, m_vboId );
	// Reallocate the new buffer of the proper size
	glVertexAttribIPointer( 0, 2, GL_UNSIGNED_SHORT, sizeof( InterfaceVertex ), (GLvoid*)offsetof( InterfaceVertex, posx ) );
	glVertexAttribPointer( 1, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof( InterfaceVertex ), (GLvoid*)offsetof( InterfaceVertex, texu ) );
	glVertexAttribIPointer( 1, 1, GL_UNSIGNED_BYTE, sizeof( InterfaceVertex ), (GLvoid*)offsetof( InterfaceVertex, textureId ) );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glEnableVertexAttribArray( 2 );
	glBufferData( GL_ARRAY_BUFFER, m_quadMaxBufferSize, 0, GL_DYNAMIC_DRAW );
	// Index
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_iboId );
	// Fill it with new quad indices
	quadCount = (unsigned int)(m_quadMaxBufferSize / (sizeof( InterfaceVertex )*4));
	pIndices = new unsigned int[quadCount*6];
	unsigned int quadSequence[] = { 0,1,2,2,3,0 };
	// Copy the quad index sequence
	for( unsigned int i = 0; i < quadCount; i++ )
		memcpy( pIndices + i*6, &quadSequence, sizeof( quadSequence ) );
	// Move it into the buffer
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( unsigned int )*quadCount*6, pIndices, GL_STATIC_DRAW );
	delete[] pIndices;

	EndGLDebug();
}

void CInterfaceManager::addQuad( glm::ivec2 absolutePosition, glm::ivec2 absoluteSize, char textureId, glm::vec2 tex_start, glm::vec2 tex_stop )
{
	InterfaceQuad quad;
	quad.absPos = absolutePosition;
	quad.absSize = absoluteSize;
	quad.textureId = textureId;
	quad.tex_start = tex_start;
	quad.tex_stop = tex_stop;
	m_interfaceQuads.push_back( quad );
}
void CInterfaceManager::clearQuads() {
	m_interfaceQuads.clear();
}
void CInterfaceManager::flushQuads()
{
	InterfaceVertex *pVertices;
	unsigned int currentVertex;
	GLsizeiptr bufferSize;

	// Create the new data
	pVertices = new InterfaceVertex[m_interfaceQuads.size()*4];
	currentVertex = 0; 
	for( auto it = m_interfaceQuads.begin(); it != m_interfaceQuads.end(); it++ )
	{
		InterfaceVertex v1, v2, v3, v4;
		
		v1.posx = (GLushort)(*it).absPos.x;
		v1.posy = (GLushort)(*it).absPos.y;
		v1.textureId = (*it).textureId;
		//v1.layer = LAYER_INTERFACE;
		pVertices[currentVertex++] = v1;
		v2.posx = (GLushort)(*it).absPos.x+(*it).absSize.x;
		v2.posy = (GLushort)(*it).absPos.y;
		v2.textureId = (*it).textureId;
		//v2.layer = LAYER_INTERFACE;
		pVertices[currentVertex++] = v2;
		v3.posx = (GLushort)(*it).absPos.x;
		v3.posy = (GLushort)(*it).absPos.y+(*it).absSize.y;
		v3.textureId = (*it).textureId;
		//v3.layer = LAYER_INTERFACE;
		pVertices[currentVertex++] = v3;
		v4.posx = (GLushort)(*it).absPos.x+(*it).absSize.x;
		v4.posy = (GLushort)(*it).absPos.y+(*it).absSize.y;
		v4.textureId = (*it).textureId;
		//v4.layer = LAYER_INTERFACE;
		pVertices[currentVertex++] = v4;
	}
	m_vertexCount = currentVertex;
	m_indexCount = m_interfaceQuads.size()*6;

	// Push the new data
	bufferSize = sizeof( InterfaceVertex )*m_vertexCount;
	if( (unsigned int)bufferSize > m_quadMaxBufferSize ) {
		PrintError( L"Too many interface quads! Some will not be rendered\n" );
		bufferSize = m_quadMaxBufferSize;
	}
	StartGLDebug( "FlushInterfaceQuadBuffer" );
	glBindVertexArray( m_vaoId );
	// Invalidate old data
	glInvalidateBufferSubData( m_vboId, 0, bufferSize );
	// Send new data
	glBufferSubData( GL_ARRAY_BUFFER, 0, bufferSize, pVertices );
	EndGLDebug();

	delete[] pVertices;

	this->clearQuads();
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