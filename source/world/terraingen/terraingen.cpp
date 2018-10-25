#include "base.h"
#include "world\terraingen\terraingen.h"
#include "blocks\block.h"
#include "world\world.h"

int CTerrainGenerator::SurfaceLevel = 6;
int CTerrainGenerator::GrassDepth = 4;

CTerrainGenerator::CTerrainGenerator() {
	m_pWorldHandle = 0;
}
CTerrainGenerator::~CTerrainGenerator() {
}

bool CTerrainGenerator::initialize( CWorld *pWorld )
{
	m_pWorldHandle = pWorld;
	return true;
}
void CTerrainGenerator::destroy()
{
	m_pWorldHandle = 0;
}

CBlock* CTerrainGenerator::generateBlock( glm::ivec2 chunkPos, glm::ivec2 blockPos )
{
	
	if( chunkPos.y == SurfaceLevel ) {
		if( blockPos.y <= GrassDepth ) {
			if( blockPos.y == GrassDepth )
				return m_pWorldHandle->getBlock( BLOCK_ID_GRASS );
			else
				return m_pWorldHandle->getBlock( BLOCK_ID_DIRT );
		}
		else
			return 0;
	}
	else if( chunkPos.y < SurfaceLevel )
		return m_pWorldHandle->getBlock( BLOCK_ID_STONE );
	else
		return 0;

	return m_pWorldHandle->getBlock( BLOCK_ID_STONE );
}