#pragma once

#include <glm\glm.hpp>

#include "perlin.h"

class CWorld;
class CBlock;

class CTerrainGenerator
{
private:
	static int SurfaceLevel;
	static int GrassDepth;

	CWorld *m_pWorldHandle;

	siv::PerlinNoise m_perlin;
public:
	CTerrainGenerator();
	~CTerrainGenerator();

	bool initialize( CWorld *pWorld );
	void destroy();

	CBlock* generateBlock( glm::ivec2 chunkPos, glm::ivec2 blockPos );
};