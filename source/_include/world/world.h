#pragma once

#include <map>
#include <Box2d\Box2D.h>

#include "entarray.h"

#define GRAVITY_CONSTANT -9.8f
#define PHYSICAL_WORLD_TICK (1.0f / 70.0f)
#define PHYSICAL_WORLD_VEL_IT 6//10
#define PHYSICAL_WORLD_POS_IT 2//8

class CEntityRenderable;
class CEntityPlayer;
class CEntityPlatform;
class CEntityStress;

class CContactListener;
class CPhysicsDebugDraw;

class CEnvironment;

class CTextureTilemap;

class CSpriteManager;

class CChunkManager;
class CBlock;
class CBlockTerrain;

class CTerrainGenerator;

enum : unsigned short
{
	BLOCK_ID_AIR = 0,
	BLOCK_ID_STONE = 1,
	BLOCK_ID_DIRT = 2,
	BLOCK_ID_GRASS = 3
};

enum : unsigned char
{
	PHYS_USERDATA_UNKNOWN = 0,
	PHYS_USERDATA_PLAYER,
	PHYS_USERDATA_ENTITY
};

enum : unsigned char
{
	MOVEMENTSTATE_JUMP		= 1 << 0,
	MOVEMENTSTATE_CROUCH	= 1 << 1,
	MOVEMENTSTATE_LEFT		= 1 << 2,
	MOVEMENTSTATE_RIGHT		= 1 << 3
};

class CWorld
{
private:
	typedef std::map<unsigned short, CBlock*> BlockMap;

	CEntityPlayer *m_pLocalPlayer;
	CEnvironment *m_pEnvironment;

	CSpriteManager *m_pSpriteManager;

	glm::vec2 m_cameraPosition;

	CEntityArraySortable<CEntityRenderable> *m_pDrawArray;
	CEntityArray<CEntityPlayer> *m_pPlayerArray;
	CEntityArray<CEntityPlatform> *m_pPlatformArray;
	CEntityArray<CEntityStress> *m_pStressArray;

	bool m_bDebugDraw;
	bool m_bFlyCamera;
	CPhysicsDebugDraw *m_pDebugDraw;
	CContactListener *m_pPhysListener;
	b2World *m_pPhysWorld;
	double m_timeSinceLastUpdate;

	CChunkManager *m_pChunkManager;

	BlockMap m_registeredBlocks;

	CTextureTilemap *m_pBlockTilemap;
	CTextureTilemap *m_pLivingTilemap;
	CTextureTilemap *m_pItemsTilemap;

	CBlockTerrain *m_pBlockStone;
	CBlockTerrain *m_pBlockDirt;
	CBlockTerrain *m_pBlockGrass;

	CTerrainGenerator *m_pTerrainGenerator;

	bool loadTilemaps();
	bool initBlocks();
	void destroyBlocks();

	void checkKeypresses( double deltaT );
public:
	CWorld();
	~CWorld();

	bool initialize();
	void destroy();

	template<class T>
	T* createEntity( CEntityArray<T> *pEntityArray ) {
		T* pEntity;
		pEntity = pEntityArray->createEntity();
		return pEntity;
	}
	template<class T>
	T* createRenderableEntity( CEntityArray<T> *pEntityArray ) {
		T* pEntity = this->createEntity<T>( pEntityArray );
		// Add it to the sorted array
		m_pDrawArray->addEntity( pEntity );
		return pEntity;
	}

	void draw( glm::mat4& orthoMat );
	void update( double deltaT );

	bool registerBlock( CBlock *pBlock );
	CBlock* getBlock( unsigned short id );

	CEntityPlayer *getLocalPlayer();
	b2World* getPhysicalWorld();
	CTerrainGenerator* getTerrainGenerator();
	CTextureTilemap* getBlockTilemap();
	glm::vec2 getCameraPosition() const;
	void setCameraPosition( glm::vec2 cameraPosition );
};