#pragma once

#define WORLD_WIDTH_CHUNKS 16
#define WORLD_HEIGHT_CHUNKS 32

class CChunk;

class CChunkManager
{
public:
	typedef std::vector<std::vector<CChunk*>> ChunkList;
private:
	static glm::ivec2 RelativeToAbsolutePos( glm::ivec2 origin, CChunk *pChunk );

	int m_chunksRenderedX, m_chunksRenderedY;
	glm::ivec2 m_chunksOrigin;

	ChunkList m_chunksRendered;

	bool allocateChunks();
	void destroyChunks();

	void shiftChunks( glm::ivec2 movementVector );
public:
	CChunkManager();
	~CChunkManager();

	bool initialize();
	void destroy();

	bool populateChunks();

	void update( glm::vec2 bottomLeft );

	int getChunksRenderedX() const;
	void setChunksRenderedX( int renderx );
	int getChunkSRenderedY() const;
	void setChunksRenderedY( int rendery );

	glm::ivec2 getChunksOrigin() const;
	void setChunksOrigin( glm::ivec2 origin );

	ChunkList& getChunksRendered();
};