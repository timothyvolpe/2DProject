#include "base.h"
#include "world\chunk.h"
#include "world\world.h"
#include "graphics.h"
#include "texture.h"
#include "texturemanager.h"
#include "world\terraingen\terraingen.h"
#include "world\spritemanager.h"
#include "world\physlistener.h"
#include "blocks\block.h"

CChunk::CChunk() {
	m_position = glm::ivec2( 0, 0 );
	m_relativePosition = glm::ivec2( 0, 0 );

	memset( &m_blocks[0], 0, sizeof( m_blocks ) );
}
CChunk::~CChunk() {
}

bool CChunk::initialize()
{
	return true;
}
void CChunk::destroy()
{
	b2World *pPhysWorld = CGame::getInstance().getWorld()->getPhysicalWorld();

	memset( &m_blocks[0], 0, sizeof( m_blocks ) );
	m_chunkQuads.clear();
	
	// Clean up bodies
	assert( m_pChunkBodies.size() == m_pChunkFixtures.size() );
	for( size_t i = 0; i < m_pChunkBodies.size(); i++ )
	{
		PhysicsUserdata *pUserData;
		pUserData = reinterpret_cast<PhysicsUserdata*>(m_pChunkBodies[i]->GetUserData());
		m_pChunkBodies[i]->DestroyFixture( m_pChunkFixtures[i] );
		pPhysWorld->DestroyBody( m_pChunkBodies[i] );
		SafeDelete( pUserData );
	}
	m_pChunkBodies.clear();
	m_pChunkFixtures.clear();
}

bool CChunk::populateChunk()
{
	CWorld *pWorld = CGame::getInstance().getWorld();
	bool isBlockMeshed[CHUNK_WIDTH_BLOCKS][CHUNK_HEIGHT_BLOCKS];
	bool breakScan;

	// Clean up old data
	this->destroy();

	// Populate the chunks blocks
	for( size_t x = 0; x < CHUNK_WIDTH_BLOCKS; x++ ) {
		for( size_t y = 0; y < CHUNK_HEIGHT_BLOCKS; y++ ) {
			// Generate the block
			m_blocks[x][y] = pWorld->getTerrainGenerator()->generateBlock( this->getPosition(), glm::ivec2( x, y ) );
		}
	}

	memset( &isBlockMeshed[0], 0, sizeof( isBlockMeshed ) );

	// Generate an optimized mesh
	// Move left-right, top-bottom
	for( size_t y = 0; y < CHUNK_HEIGHT_BLOCKS; y++ )
	{
		for( size_t x = 0; x < CHUNK_WIDTH_BLOCKS; x++ )
		{
			// Skip air
			if( !m_blocks[x][y] )
				continue;
			// Skip meshed blocks
			if( isBlockMeshed[x][y] )
				continue;

			size_t x2, y2;
			// Scan to find the rightmost edge of the quad
			for( x2 = x+1; x2 < CHUNK_WIDTH_BLOCKS; x2++ )
			{
				// If next block isnt equal, edge was previous block
				if( m_blocks[x2][y] != m_blocks[x][y] ) {
					x2--;
					break;
				}
				isBlockMeshed[x2][y] = true;
			}

			// Scan to find the bottomost edge of the quad
			breakScan = false;
			for( y2 = y+1; y2 < CHUNK_HEIGHT_BLOCKS && !breakScan; y2++ )
			{
				for( size_t x3 = x; x3 < x2; x3++ )
				{
					if( m_blocks[x3][y2] != m_blocks[x][y] ) {
						y2--;
						breakScan = true;
						break;
					}
					isBlockMeshed[x3][y2] = true;
				}
			}

			// Create the quad
			ChunkQuad q;
			q.pBlock = m_blocks[x][y];
			q.relativePosition = glm::ivec2( x, y );
			q.size = glm::ivec2( x2 - x, y2 - y );
			m_chunkQuads.push_back( q );
		}
	}

	// Create physic bodies
	glm::vec2 posOffset = glm::vec2( this->getPosition() ) * glm::vec2( CHUNK_WIDTH_UNITS, CHUNK_HEIGHT_UNITS );
	m_pChunkBodies.reserve( m_chunkQuads.size() );
	m_pChunkFixtures.reserve( m_chunkQuads.size() );
	for( auto it = m_chunkQuads.begin(); it != m_chunkQuads.end(); it++ )
	{
		b2Body *pBody;
		b2Fixture *pFixture;
		b2BodyDef bd;
		b2PolygonShape sd;
		b2FixtureDef fd;
		PhysicsUserdata *pUserData;

		// Create body
		bd.type = b2_staticBody;
		bd.position.Set( (float32)(*it).relativePosition.x*CHUNK_BLOCK_SIZE + posOffset.x, (float32)(*it).relativePosition.y*CHUNK_BLOCK_SIZE + posOffset.y );
		bd.userData = 0;
		bd.fixedRotation = true;
		pBody = CGame::getInstance().getWorld()->getPhysicalWorld()->CreateBody( &bd );

		// Create fixture
		//sd.SetAsBox( (*it).size.x / 2.0f, (*it).size.y / 2.0f, b2Vec2( 0, 0 ), 0.0f );
		b2Vec2 boxSize( (*it).size.x * CHUNK_BLOCK_SIZE, (*it).size.y * CHUNK_BLOCK_SIZE );
		sd.SetAsBox( boxSize.x / 2.0f, boxSize.y / 2.0f, boxSize - b2Vec2( boxSize.x / 2.0f, boxSize.y / 2.0f ), 0.0f );
		fd.shape = &sd;
		fd.density = 1.0f;
		pFixture = pBody->CreateFixture( &fd );

		// Set userdata
		pUserData = new PhysicsUserdata();
		pUserData->type = USER_DATA_TYPE_CHUNKS;
		pUserData->pPhysicsObj = 0;
		pUserData->pRenderablePhysicsObj = 0;
		pBody->SetUserData( pUserData );

		m_pChunkBodies.push_back( pBody );
		m_pChunkFixtures.push_back( pFixture );
	}

	return true;
}

void CChunk::draw()
{
	glm::vec4 uvbounds;
	SpriteData sd;

	// Quad position is the center

	CGame::getInstance().getWorld()->getBlockTilemap()->bind(0);
	for( auto it = m_chunkQuads.begin(); it != m_chunkQuads.end(); it++ )
	{
		sd.layer = LAYER_CHUNKBLOCKS;
		sd.size = glm::vec2( CHUNK_BLOCK_SIZE * (*it).size.x, CHUNK_BLOCK_SIZE * (*it).size.y );
		sd.position = (glm::vec2( (*it).relativePosition.x*CHUNK_BLOCK_SIZE, (*it).relativePosition.y*CHUNK_BLOCK_SIZE ) + sd.size / 2.0f) + (glm::vec2( this->getPosition() ) * glm::vec2( CHUNK_WIDTH_UNITS, CHUNK_HEIGHT_UNITS ));
		sd.rotation = 0.0f;
		sd.texcoords = (*it).pBlock->getTextureTileCoords();
		sd.tileInfo[0] = (*it).size.x;
		sd.tileInfo[1] = (*it).size.y;
		CGame::getInstance().getWorld()->getSpriteManager()->drawSprite( (*it).pBlock->getBatchId(), sd );
	}
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