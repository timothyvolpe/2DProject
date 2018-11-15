#include "base.h"
#include "shader\shadermanager.h"
#include "shader\shader.h"
#include "graphics.h"

CShaderManager::CShaderManager() {
	m_pCurrentlyBound = 0;
}
CShaderManager::~CShaderManager() {
}

bool CShaderManager::initialize()
{
	boost::filesystem::path compilePath, linkPath;

	// Clear log files
	compilePath = GameFilesystem::ConstructPath( COMPILE_LOG, FILESYSTEM_DIR_LOGS );
	linkPath = GameFilesystem::ConstructPath( LINK_LOG, FILESYSTEM_DIR_LOGS );
	// Open them to clear them
	std::ofstream compileStream( compilePath.string(), std::ios::out );
	compileStream.close();
	std::ofstream linkStream( linkPath.string(), std::ios::out );
	linkStream.close();

	if( !this->loadPrograms() )
		return false;

	return true;
}
void CShaderManager::destroy()
{
	// Destroy shader programs
	for( ProgramMap::iterator it = m_shaderPrograms.begin(); it != m_shaderPrograms.end(); it++ ) {
		DestroyDelete( (*it).second );
	}
	m_shaderPrograms.clear();
	m_pCurrentlyBound = 0;
}

bool CShaderManager::createAndLoadProgram( const wchar_t *pName, bool vertexShader, bool geomtryShader, bool fragmentShader, std::vector<std::string>& uniforms, std::vector<std::pair<int, std::string>> &attribLocations )
{
	CShaderObject *pVert, *pGeom, *pFrag;
	CShaderProgram *pProgram;

	pVert = 0;
	pGeom = 0;
	pFrag = 0;

	// Create the shader objects
	if( vertexShader ) {
		pVert = new CShaderObject();
		if( !pVert->initializeShader( pName, GL_VERTEX_SHADER ) )
			return false;
	}
	if( geomtryShader ) {
		pGeom = new CShaderObject();
		if( !pGeom->initializeShader( pName, GL_GEOMETRY_SHADER ) )
			return false;
	}
	if( fragmentShader ) {
		pFrag = new CShaderObject();
		if( !pFrag->initializeShader( pName, GL_FRAGMENT_SHADER ) )
			return false;
	}

	// Base
	pProgram = new CShaderProgram();
	if( !pProgram->initializeProgram( pName, pVert, pGeom, pFrag, uniforms, attribLocations ) )
		return false;
	m_shaderPrograms.insert( std::pair<std::wstring, CShaderProgram*>( pProgram->getName(), pProgram ) );

	DestroyDelete( pVert );
	DestroyDelete( pFrag );
	DestroyDelete( pGeom );

	return true;
}

bool CShaderManager::loadPrograms()
{
	std::vector<std::string> uniforms;
	std::vector<std::pair<int, std::string>> attribLocations;

	// Base
	uniforms.push_back( "MVPMatrix" );
	uniforms.push_back( "tex2dsampler" );
	if( !this->createAndLoadProgram( L"base", true, false, true, uniforms, attribLocations ) )
		return false;
	uniforms.clear();
	attribLocations.clear();
	this->bind( this->getShaderProgram( L"base" ) );
	glUniform1i( this->getShaderProgram( L"base" )->getUniform( "tex2dsampler" ), 0 );
	// Debug
	uniforms.push_back( "MVPMatrix" );
	if( !this->createAndLoadProgram( L"debug", true, false, true, uniforms, attribLocations ) )
		return false;
	uniforms.clear();
	attribLocations.clear();
	// Environment
	uniforms.push_back( "MVPMatrix" );
	uniforms.push_back( "screenResolution" );
	uniforms.push_back( "drawMode" );
	if( !this->createAndLoadProgram( L"environment", true, false, true, uniforms, attribLocations ) )
		return false;
	uniforms.clear();
	attribLocations.clear();
	// Interface
	uniforms.push_back( "MVPMatrix" );
	attribLocations.push_back( std::pair<int, std::string>( 0, "in_pos" ) );
	attribLocations.push_back( std::pair<int, std::string>( 1, "in_tex" ) );
	attribLocations.push_back( std::pair<int, std::string>( 2, "textureId" ) );
	if( !this->createAndLoadProgram( L"interface", true, true, true, uniforms, attribLocations ) )
		return false;
	uniforms.clear();
	attribLocations.clear();
	// Sprite
	uniforms.push_back( "MVPMatrix" );
	uniforms.push_back( "tex2dsampler" );
	attribLocations.push_back( std::pair<int, std::string>( 0, "in_pos" ) );
	attribLocations.push_back( std::pair<int, std::string>( 1, "in_size" ) );
	attribLocations.push_back( std::pair<int, std::string>( 2, "layer" ) );
	attribLocations.push_back( std::pair<int, std::string>( 3, "in_texcoords" ) );
	if( !this->createAndLoadProgram( L"sprite", true, true, true, uniforms, attribLocations ) )
		return false;
	uniforms.clear();
	attribLocations.clear();
	this->bind( this->getShaderProgram( L"sprite" ) );
	glUniform1i( this->getShaderProgram( L"sprite" )->getUniform( "tex2dsampler" ), 0 );

	return true;
}

void CShaderManager::bind( CShaderProgram *pProgram )
{
	if( m_pCurrentlyBound == pProgram )
		return;
	m_pCurrentlyBound = pProgram;
	m_pCurrentlyBound->bind();
}

CShaderProgram* CShaderManager::getShaderProgram( std::wstring name )
{
	if( m_shaderPrograms.find( name ) != m_shaderPrograms.end() )
		return m_shaderPrograms[name];
	else {
		PrintWarn( L"Could not find shader program \"%s\"\n", name.c_str() );
		return 0;
	}
}
CShaderProgram* CShaderManager::getCurrentlyBound() const {
	return m_pCurrentlyBound;
}