#pragma once

#include <glm\glm.hpp>

class CWorld;
class CBlock;

class CTerrainGenerator
{
private:
	static int SurfaceLevel;
	static int GrassDepth;

	CWorld *m_pWorldHandle;
public:
	CTerrainGenerator();
	~CTerrainGenerator();

	bool initialize( CWorld *pWorld );
	void destroy();

	CBlock* generateBlock( glm::ivec2 chunkPos, glm::ivec2 blockPos );
};