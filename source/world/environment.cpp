#include "base.h"
#include "world\environment.h"
#include "graphics.h"
#include "texture.h"
#include "texturemanager.h"
#include "world\world.h"
#include "config.h"
#include "shader\shader.h"
#include "shader\shadermanager.h"

EnvVertex CreateEnvVertex( glm::vec2 pos, glm::vec2 texcoords, unsigned char layer )
{
	EnvVertex vertex;
	vertex.pos = pos;
	vertex.texcoords = texcoords;
	vertex.layer = layer;
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
	glVertexAttribIPointer( 2, 1, GL_UNSIGNED_BYTE, sizeof( EnvVertex ), (GLvoid*)offsetof( EnvVertex, layer ) );

	m_pVertices = new EnvVertex[ENV_QUAD_COUNT*4];
	glBufferData( GL_ARRAY_BUFFER, sizeof( EnvVertex ) * ENV_QUAD_COUNT * 4, m_pVertices, GL_STREAM_DRAW );

	glGenBuffers( 1, &m_iboId );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_iboId );

	// Indices
	unsigned int pIndices[] = { 0, 1, 2, 2, 1, 3,
								0, 1, 2, 2, 1, 3 };
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

	StartGLDebug( "DrawEnvironment" );

	CGame::getInstance().getGraphics()->getShaderManager()->bind( pEnvProgram );

	// Draw quads
	m_pVertices[0] = CreateEnvVertex( glm::vec2( 0, 0 ), glm::vec2( 0, 1 ), worldLayer );
	m_pVertices[1] = CreateEnvVertex( glm::vec2( bgWidth, 0 ), glm::vec2( 1, 1 ), worldLayer );
	m_pVertices[2] = CreateEnvVertex( glm::vec2( 0, bgHeight ), glm::vec2( 0, 0 ), worldLayer );
	m_pVertices[3] = CreateEnvVertex( glm::vec2( bgWidth, bgHeight ), glm::vec2( 1, 0 ), worldLayer );
	glBindBuffer( GL_ARRAY_BUFFER, m_vboId );
	glBufferData( GL_ARRAY_BUFFER, sizeof( EnvVertex ) * ENV_QUAD_COUNT * 4, &m_pVertices[0], GL_STREAM_DRAW );

	glBindVertexArray( m_vaoId );
	// Render quads
	modelMatrix = glm::mat4( 1.0f );
	mvpMatrix = orthoMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv( pEnvProgram->getUniform( "MVPMatrix" ), 1, GL_FALSE, &mvpMatrix[0][0] );
	glUniform2fv( pEnvProgram->getUniform( "screenResolution" ), 1, &screenResolution[0] );
	switch( worldLayer )
	{
	case ENV_LAYER_BACKGROUND:

		// Draw the sky
		glUniform1i( pEnvProgram->getUniform( "drawMode" ), ENV_DRAWMODE_SKY );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)(6 * sizeof(unsigned int)) );

		// Draw the mountains
		glUniform1i( pEnvProgram->getUniform( "drawMode" ), ENV_DRAWMODE_MOUNTAINS );
		m_pMountainTexture->bind( 0 );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

		break;
	case ENV_LAYER_FOREGROUND:
		break;
	}

	EndGLDebug();
}