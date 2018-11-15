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

bool CShaderManager::loadPrograms()
{
	// Load test programs
	CShaderObject *pBaseVert, *pBaseFrag, *pDebugVert, *pDebugFrag, *pEnvVert, *pEnvFrag, *pIntVert, *pIntGeom, *pIntFrag;
	CShaderObject *pSpriteVert, *pSpriteGeom, *pSpriteFrag;
	std::vector<std::string> baseUniforms, debugUniforms, envUniforms, intUniforms, spriteUniforms;
	CShaderProgram *pBaseProgram, *pDebugProgram, *pEnvProgram, *pIntProgram, *pSpriteProgram;

	pBaseVert = new CShaderObject();
	if( !pBaseVert->initializeShader( L"base", GL_VERTEX_SHADER ) )
		return false;
	pBaseFrag = new CShaderObject();
	if( !pBaseFrag->initializeShader( L"base", GL_FRAGMENT_SHADER ) )
		return false;
	pDebugVert = new CShaderObject();
	if( !pDebugVert->initializeShader( L"debug", GL_VERTEX_SHADER ) )
		return false;
	pDebugFrag = new CShaderObject();
	if( !pDebugFrag->initializeShader( L"debug", GL_FRAGMENT_SHADER ) )
		return false;
	pEnvVert = new CShaderObject();
	if( !pEnvVert->initializeShader( L"environment", GL_VERTEX_SHADER ) )
		return false;
	pEnvFrag = new CShaderObject();
	if( !pEnvFrag->initializeShader( L"environment", GL_FRAGMENT_SHADER ) )
		return false;
	pIntVert = new CShaderObject();
	if( !pIntVert->initializeShader( L"interface", GL_VERTEX_SHADER ) )
		return false;
	pIntGeom = new CShaderObject();
	if( !pIntGeom->initializeShader( L"interface", GL_GEOMETRY_SHADER ) )
		return false;
	pIntFrag = new CShaderObject();
	if( !pIntFrag->initializeShader( L"interface", GL_FRAGMENT_SHADER ) )
		return false;
	pSpriteVert = new CShaderObject();
	if( !pSpriteVert->initializeShader( L"sprite", GL_VERTEX_SHADER ) )
		return false;
	pSpriteGeom = new CShaderObject();
	if( !pSpriteGeom->initializeShader( L"sprite", GL_GEOMETRY_SHADER ) )
		return false;
	pSpriteFrag = new CShaderObject();
	if( !pSpriteFrag->initializeShader( L"sprite", GL_FRAGMENT_SHADER ) )
		return false;

	// Base
	pBaseProgram = new CShaderProgram();
	baseUniforms.push_back( "MVPMatrix" );
	baseUniforms.push_back( "tex2dsampler" );
	if( !pBaseProgram->initializeProgram( L"base", pBaseVert, NULL, pBaseFrag, baseUniforms ) )
		return false;
	this->bind( pBaseProgram );
	glUniform1i( pBaseProgram->getUniform( "tex2dsampler" ), 0 );
	m_shaderPrograms.insert( std::pair<std::wstring, CShaderProgram*>( pBaseProgram->getName(), pBaseProgram ) );
	
	// Debug
	pDebugProgram = new CShaderProgram();
	debugUniforms.push_back( "MVPMatrix" );
	if( !pDebugProgram->initializeProgram( L"debug", pDebugVert, NULL, pDebugFrag, debugUniforms ) )
		return false;
	m_shaderPrograms.insert( std::pair<std::wstring, CShaderProgram*>( pDebugProgram->getName(), pDebugProgram ) );

	// Environment
	pEnvProgram = new CShaderProgram();
	envUniforms.push_back( "MVPMatrix" );
	envUniforms.push_back( "screenResolution" );
	envUniforms.push_back( "drawMode" );
	if( !pEnvProgram->initializeProgram( L"environment", pEnvVert, NULL, pEnvFrag, envUniforms ) )
		return false;
	m_shaderPrograms.insert( std::pair<std::wstring, CShaderProgram*>( pEnvProgram->getName(), pEnvProgram ) );

	// Interface
	pIntProgram = new CShaderProgram();
	intUniforms.push_back( "MVPMatrix" );
	if( !pIntProgram->initializeProgram( L"interface", pIntVert, pIntGeom, pIntFrag, intUniforms ) )
		return false;
	this->bind( pIntProgram );
	m_shaderPrograms.insert( std::pair<std::wstring, CShaderProgram*>( pIntProgram->getName(), pIntProgram ) );

	// Sprite
	pSpriteProgram = new CShaderProgram();
	spriteUniforms.push_back( "MVPMatrix" );
	if( !pSpriteProgram->initializeProgram( L"sprite", pSpriteVert, pSpriteGeom, pSpriteFrag, spriteUniforms ) )
		return false;
	this->bind( pSpriteProgram );
	m_shaderPrograms.insert( std::pair<std::wstring, CShaderProgram*>( pSpriteProgram->getName(), pSpriteProgram ) );


	DestroyDelete( pBaseVert );
	DestroyDelete( pBaseFrag );
	DestroyDelete( pDebugVert );
	DestroyDelete( pDebugFrag );
	DestroyDelete( pEnvVert );
	DestroyDelete( pEnvFrag );
	DestroyDelete( pIntVert );
	DestroyDelete( pIntGeom );
	DestroyDelete( pIntFrag );
	DestroyDelete( pSpriteVert );
	DestroyDelete( pSpriteGeom );
	DestroyDelete( pSpriteFrag );

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