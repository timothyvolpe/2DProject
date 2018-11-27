#include "base.h"
#include "world\environment.h"
#include "graphics.h"
#include "texture.h"
#include "texturemanager.h"
#include "world\world.h"
#include "config.h"
#include "shader\shader.h"
#include "shader\shadermanager.h"
#include "renderutil.h"

EnvVertex CreateEnvVertex( glm::vec2 pos, glm::vec2 texcoords )
{
	EnvVertex vertex;
	vertex.pos = pos;
	vertex.texcoords = texcoords;
	return vertex;
}

CEnvironment::CEnvironment() {
	m_vaoId = 0;
	m_vboId = 0;
	m_iboId = 0;
	m_pMountainTexture = 0;
	m_pVertices = 0;
}
CEnvironment::~CEnvironment() {
}

bool CEnvironment::initialize()
{
	TextureDescriptor textureDesc;

	StartGLDebug( "CreateEnvironment" );

	glGenVertexArrays( 1, &m_vaoId );
	glBindVertexArray( m_vaoId );

	glGenBuffers( 1, &m_vboId );
	glBindBuffer( GL_ARRAY_BUFFER, m_vboId );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( EnvVertex ), (GLvoid*)offsetof( EnvVertex, pos ) );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof( EnvVertex ), (GLvoid*)offsetof( EnvVertex, texcoords ) );

	m_pVertices = new EnvVertex[ENV_QUAD_COUNT*4];
	glBufferData( GL_ARRAY_BUFFER, sizeof( EnvVertex ) * ENV_QUAD_COUNT * 4, m_pVertices, GL_STREAM_DRAW );

	glGenBuffers( 1, &m_iboId );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_iboId );

	// Indices
	unsigned int pIndices[] = { 0, 0, 0, 0, 0, 0,
								0, 0, 0, 0, 0, 0 };
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( unsigned int ) * ENV_QUAD_COUNT * 6, pIndices, GL_STATIC_DRAW );

	EndGLDebug();

	// Create the background texture
	// Load the texture
	textureDesc.magFilter = GL_NEAREST;
	textureDesc.minFilter = GL_NEAREST;
	textureDesc.wrapS = GL_REPEAT;
	textureDesc.wrapT = GL_REPEAT;
	textureDesc.useAlpha = true;
	m_pMountainTexture = CGame::getInstance().getGraphics()->getTextureManager()->loadTexture2D( L"env\\mountains2.png", textureDesc );
	if( !m_pMountainTexture )
		return false;

	return true;
}
void CEnvironment::destroy()
{
	DestroyDelete( m_pMountainTexture );
	SafeDelete( m_pVertices );

	if( m_iboId ) {
		glDeleteBuffers( 1, &m_iboId );
		m_iboId = 0;
	}
	if( m_vboId ) {
		glDeleteBuffers( 1, &m_vboId );
		m_vboId = 0;
	}
	if( m_vaoId ) {
		glDeleteVertexArrays( 1, &m_vaoId );
		m_vaoId = 0;
	}
}

void CEnvironment::draw( unsigned char worldLayer, glm::mat4 orthoMatrix )
{
	glm::mat4 modelMatrix, viewMatrix, mvpMatrix;
	float bgWidth, bgHeight;
	CShaderProgram *pEnvProgram = CGame::getInstance().getGraphics()->getShaderManager()->getShaderProgram( L"environment" );
	glm::vec2 screenResolution;

	bgWidth = (float)CGame::getInstance().getConfig()->m_resolutionX / (float)CGame::getInstance().getGraphics()->getPixelsPerMeter();
	bgHeight = (float)CGame::getInstance().getConfig()->m_resolutionY / (float)CGame::getInstance().getGraphics()->getPixelsPerMeter();
	screenResolution = glm::vec2( (float)CGame::getInstance().getConfig()->m_resolutionX, (float)CGame::getInstance().getConfig()->m_resolutionY );

	CGame::getInstance().getGraphics()->getShaderManager()->bind( pEnvProgram );

	m_pVertices[0] = CreateEnvVertex( glm::vec2( 0, 0 ), glm::vec2( 0, 0 ) );
	m_pVertices[1] = CreateEnvVertex( glm::vec2( 0, 0 ), glm::vec2( 0, 0 ) );
	m_pVertices[2] = CreateEnvVertex( glm::vec2( 0, 0 ), glm::vec2( 0, 0 ) );
	m_pVertices[3] = CreateEnvVertex( glm::vec2( 0, 0 ), glm::vec2( 0, 0 ) );

	StartGLDebug( "DrawEnvironment" );

	switch( worldLayer )
	{
	case ENV_LAYER_BACKGROUND:
		modelMatrix = glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.0f, 0.0f, CGraphics::getLayerPosition( LAYER_WORLD_BACKGROUND ) ) );
		mvpMatrix = orthoMatrix * viewMatrix * modelMatrix;
		glUniformMatrix4fv( pEnvProgram->getUniform( "MVPMatrix" ), 1, GL_FALSE, &mvpMatrix[0][0] );
		glUniform2fv( pEnvProgram->getUniform( "screenResolution" ), 1, &screenResolution[0] );

		// Draw the sky
		glUniform1i( pEnvProgram->getUniform( "drawMode" ), ENV_DRAWMODE_SKY );
		CRenderUtil::drawSprite( glm::vec2( 0.0f, 0.0f ), glm::vec2( bgWidth, bgHeight ) );

		// Draw the mountains
		m_pMountainTexture->bind( 0 );
		glUniform1i( pEnvProgram->getUniform( "drawMode" ), ENV_DRAWMODE_MOUNTAINS );
		CRenderUtil::drawSpriteTextured( glm::vec2( 0.0f, 0.0f ), glm::vec2( bgWidth, bgHeight ), glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f ) );

		break;
	case ENV_LAYER_FOREGROUND:
		break;
	}

	EndGLDebug();
}