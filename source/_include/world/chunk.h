#pragma once

#define CHUNK_WIDTH_BLOCKS 16
#define CHUNK_HEIGHT_BLOCKS 16
#define CHUNK_BLOCK_SIZE 0.5f // meters
#define CHUNK_WIDTH_UNITS (CHUNK_WIDTH_BLOCKS*CHUNK_BLOCK_SIZE)
#define CHUNK_HEIGHT_UNITS (CHUNK_HEIGHT_BLOCKS*CHUNK_BLOCK_SIZE)

class CWorld;
class CBlock;

class CTexture2D;

class CChunk
{
private:
	CBlock* m_blocks[CHUNK_WIDTH_BLOCKS][CHUNK_HEIGHT_BLOCKS];

	glm::ivec2 m_position;
	glm::ivec2 m_relativePosition;
public:
	CChunk();
	~CChunk();

	bool initialize();
	void destroy();

	bool populateChunk();

	void draw();
	void debugDraw();

	glm::ivec2 getPosition() const;
	void setPosition( glm::ivec2 pos );
	glm::ivec2 getRelativePosition() const;
	void setRelativePosition( glm::ivec2 relpos );
};