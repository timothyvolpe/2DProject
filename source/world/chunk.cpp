#include "base.h"
#include "world\chunk.h"
#include "world\world.h"
#include "graphics.h"
#include "texture.h"
#include "texturemanager.h"
#include "world\terraingen\terraingen.h"
#include "world\spritemanager.h"
#include "blocks\block.h"
#include "renderutil.h"

CChunk::CChunk() {
	m_position = glm::ivec2( 0, 0 );
	m_relativePosition = glm::ivec2( 0, 0 );
}
CChunk::~CChunk() {
}

bool CChunk::initialize()
{
	
	TextureDescriptor textureDesc;

	// Create the test texture
	// Load the texture
	textureDesc.magFilter = GL_LINEAR;
	textureDesc.minFilter = GL_LINEAR;
	textureDesc.wrapS = GL_CLAMP_TO_EDGE;
	textureDesc.wrapT = GL_CLAMP_TO_EDGE;
	textureDesc.useAlpha = false;
	
	return true;
}
void CChunk::destroy()
{
}

bool CChunk::populateChunk()
{
	CWorld *pWorld = CGame::getInstance().getWorld();

	// Populate the chunks blocks
	for( size_t x = 0; x < CHUNK_WIDTH_BLOCKS; x++ ) {
		for( size_t y = 0; y < CHUNK_HEIGHT_BLOCKS; y++ ) {
			// Generate the block
			m_blocks[x][y] = pWorld->getTerrainGenerator()->generateBlock( this->getPosition(), glm::ivec2( x, y ) );
		}
	}
	return true;
}

void CChunk::draw()
{
	glm::vec4 uvbounds;
	SpriteData sd;

	CGame::getInstance().getWorld()->getBlockTilemap()->bind(0);
	for( size_t x = 0; x < CHUNK_WIDTH_BLOCKS; x++ )
	{
		for( size_t y = 0; y < CHUNK_HEIGHT_BLOCKS; y++ )
		{
			if( !m_blocks[x][y] )
				continue;

			// Draw a box
			sd.layer = LAYER_PLAYER;
			sd.position = (glm::vec2( x*CHUNK_BLOCK_SIZE, y*CHUNK_BLOCK_SIZE ) - glm::vec2( CHUNK_BLOCK_SIZE / 2.0f, CHUNK_BLOCK_SIZE / 2.0f )) + glm::vec2( this->getPosition() ) * glm::vec2( CHUNK_WIDTH_UNITS, CHUNK_HEIGHT_UNITS );
			sd.rotation = 0.0f;
			sd.size = glm::vec2( CHUNK_BLOCK_SIZE, CHUNK_BLOCK_SIZE );
			sd.texcoords = m_blocks[x][y]->getTextureTileCoords();
			CGame::getInstance().getWorld()->getSpriteManager()->drawSprite( m_blocks[x][y]->getBatchId(), sd );
		}
	}
}

void CChunk::debugDraw()
{
	/*glBegin( GL_LINE_STRIP );
	{
		glColor3f( 1.0f, 0.0f, 0.0f );
		glVertex2f( 0, 0 );
		glColor3f( 1.0f, 0.0f, 0.0f );
		glVertex2f( CHUNK_WIDTH_UNITS, 0 );
		glColor3f( 1.0f, 0.0f, 0.0f );
		glVertex2f( CHUNK_WIDTH_UNITS, CHUNK_HEIGHT_UNITS );
		glColor3f( 1.0f, 0.0f, 0.0f );
		glVertex2f( 0, CHUNK_HEIGHT_UNITS );
		glColor3f( 1.0f, 0.0f, 0.0f );
		glVertex2f( 0, 0 );
	}
	glEnd();*/
}

glm::ivec2 CChunk::getPosition() const {
	return m_position;
}
void CChunk::setPosition( glm::ivec2 pos ) {
	m_position = pos;
}
glm::ivec2 CChunk::getRelativePosition() const {
	return m_relativePosition;
}
void CChunk::setRelativePosition( glm::ivec2 relpos ) {
	m_relativePosition = relpos;
}