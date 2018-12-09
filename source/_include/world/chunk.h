#pragma once

#include <Box2D\Box2D.h>

#define CHUNK_WIDTH_BLOCKS 16
#define CHUNK_HEIGHT_BLOCKS 16
#define CHUNK_BLOCK_SIZE 0.5f // meters
#define CHUNK_WIDTH_UNITS (CHUNK_WIDTH_BLOCKS*CHUNK_BLOCK_SIZE)
#define CHUNK_HEIGHT_UNITS (CHUNK_HEIGHT_BLOCKS*CHUNK_BLOCK_SIZE)

class CWorld;
class CBlock;

class CTexture2D;

struct ChunkQuad {
	glm::ivec2 relativePosition;
	glm::ivec2 size;
	CBlock *pBlock;
};

class CChunk
{
private:
	CBlock* m_blocks[CHUNK_WIDTH_BLOCKS][CHUNK_HEIGHT_BLOCKS];

	glm::ivec2 m_position;
	glm::ivec2 m_relativePosition;

	std::vector<ChunkQuad> m_chunkQuads;
	std::vector<b2Body*> m_pChunkBodies;
	std::vector<b2Fixture*> m_pChunkFixtures;
public:
	CChunk();
	~CChunk();

	bool initialize();
	void destroy();

	bool populateChunk();

	void draw();

	glm::ivec2 getPosition() const;
	void setPosition( glm::ivec2 pos );
	glm::ivec2 getRelativePosition() const;
	void setRelativePosition( glm::ivec2 relpos );
};