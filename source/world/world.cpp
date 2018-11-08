#include "graphics.h"
#include "base.h"
#include "shader\shader.h"
#include "shader\shadermanager.h"
#include "world\world.h"
#include "config.h"
#include "input.h"
#include "texture.h"

#include "world\entities\entitybase.h"
#include "world\entities\entityplayer.h"
#include "world\entities\entityplatform.h"
#include "world\entities\entitystress.h"

#include "world\physdebug.h"
#include "world\physlistener.h"

#include "world\environment.h"

#include "world\chunkmanager.h"
#include "world\chunk.h"
#include "blocks\block.h"
#include "blocks\blockterrain.h"

#include "world\terraingen\terraingen.h"

CWorld::CWorld()
{
	m_pPlatformArray = 0;
	m_pStressArray = 0;
	m_pPlayerArray = 0;

	m_cameraPosition = glm::vec2( 0, 0 );

	m_pLocalPlayer = 0;
	m_pDrawArray = 0;
	m_pPhysListener = 0;
	m_pPhysWorld = 0;
	m_timeSinceLastUpdate = 0.0;
	m_pChunkManager = 0;
	m_pEnvironment = 0;
	m_pTerrainGenerator = 0;
	m_pBlockTilemap = 0;

	m_pBlockStone = 0;
	m_pBlockDirt = 0;
	m_pBlockGrass = 0;

	m_bDebugDraw = false;
	m_bFlyCamera = false;
}
CWorld::~CWorld() {
}

bool CWorld::initialize()
{
	CEntityPlatform *pCurrentPlatform;

	// Debug drawer
	m_pDebugDraw = new CPhysicsDebugDraw();
	// Create the contact listener
	m_pPhysListener = new CContactListener();
	// Create the physical world
	m_pPhysWorld = new b2World( b2Vec2( 0.0f, GRAVITY_CONSTANT ) );
	m_pPhysWorld->SetAllowSleeping( true );
	m_pPhysWorld->SetContinuousPhysics( true );
	m_pPhysWorld->SetContactListener( m_pPhysListener );
	m_pPhysWorld->SetDebugDraw( m_pDebugDraw );
	// Setup debug drawer
	m_pDebugDraw->SetFlags( b2Draw::e_shapeBit );

	m_cameraPosition = glm::vec2( CHUNK_HEIGHT_UNITS*6, CHUNK_HEIGHT_UNITS*6 );

	// Create the environment
	m_pEnvironment = new CEnvironment();
	if( !m_pEnvironment->initialize() )
		return false;

	// Create the draw array
	m_pDrawArray = new CEntityArraySortable<CEntityRenderable>();
	if( !m_pDrawArray->initialize() )
		return false;

	// Entity array
	m_pPlayerArray = new CEntityArray<CEntityPlayer>();
	if( !m_pPlayerArray->initialize() )
		return false;
	// Entity array
	m_pPlatformArray = new CEntityArray<CEntityPlatform>();
	if( !m_pPlatformArray->initialize() )
		return false;
	// Entity array
	m_pStressArray = new CEntityArray<CEntityStress>();
	if( !m_pStressArray->initialize() )
		return false;

	// Create texture tilemaps
	m_pBlockTilemap = new CTextureTilemap();
	if( !m_pBlockTilemap->initialize() )
		return false;
	if( !this->loadTilemaps() )
		return false;
	// Create blocks
	if( !this->initBlocks() )
		return false;
	// Create terrain manager
	m_pTerrainGenerator = new CTerrainGenerator();
	if( !m_pTerrainGenerator->initialize( this ) )
		return false;
	// Create chunk manager
	m_pChunkManager = new CChunkManager();
	m_pChunkManager->setChunksOrigin( glm::ivec2( (int)floor( m_cameraPosition.x / CHUNK_WIDTH_UNITS ), (int)floor( m_cameraPosition.y / CHUNK_WIDTH_UNITS ) ) );
	if( !m_pChunkManager->initialize() )
		return false;
	// Load chunks
	m_pChunkManager->populateChunks();

	// CLIENT-SIDE ONLY:
	// Spawn local player
	m_pLocalPlayer = this->createRenderableEntity<CEntityPlayer>( m_pPlayerArray );
	if( !m_pLocalPlayer )
		return false;
	m_pLocalPlayer->setPosition( glm::vec2( CHUNK_HEIGHT_UNITS*6+2, CHUNK_HEIGHT_UNITS*6+5 ) );
	m_pLocalPlayer->activate();

	pCurrentPlatform = this->createRenderableEntity<CEntityPlatform>( m_pPlatformArray );
	if( !pCurrentPlatform )
		return false;
	pCurrentPlatform->setPosition( glm::vec2( CHUNK_HEIGHT_UNITS*6+4, CHUNK_HEIGHT_UNITS*6+6 ) );
	pCurrentPlatform->activate();

	pCurrentPlatform = this->createRenderableEntity<CEntityPlatform>( m_pPlatformArray );
	if( !pCurrentPlatform )
		return false;
	pCurrentPlatform->setPosition( glm::vec2( CHUNK_HEIGHT_UNITS*6+4.25f, CHUNK_HEIGHT_UNITS*6+36 ) );
	pCurrentPlatform->setLayer( 255 );
	pCurrentPlatform->setTexture( L"dev\\crate02.png" );
	pCurrentPlatform->setOpaque( false );
	pCurrentPlatform->activate();

	pCurrentPlatform = this->createRenderableEntity<CEntityPlatform>( m_pPlatformArray );
	if( !pCurrentPlatform )
		return false;
	pCurrentPlatform->setPosition( glm::vec2( CHUNK_HEIGHT_UNITS*6+4, CHUNK_HEIGHT_UNITS*6+2 ) );
	pCurrentPlatform->setDimensions( glm::vec2( 10.0f, 1.0f ) );
	pCurrentPlatform->setStatic( true ); 
	pCurrentPlatform->activate();

	pCurrentPlatform = this->createRenderableEntity<CEntityPlatform>( m_pPlatformArray );
	if( !pCurrentPlatform )
		return false;
	pCurrentPlatform->setPosition( glm::vec2( CHUNK_HEIGHT_UNITS*6+10, CHUNK_HEIGHT_UNITS*6+4.5 ) );
	pCurrentPlatform->setDimensions( glm::vec2( 5.0f, 1.0f ) );
	pCurrentPlatform->setStatic( true );
	pCurrentPlatform->activate();

	pCurrentPlatform = this->createRenderableEntity<CEntityPlatform>( m_pPlatformArray );
	if( !pCurrentPlatform )
		return false;
	pCurrentPlatform->setPosition( glm::vec2( CHUNK_HEIGHT_UNITS*6+12, CHUNK_HEIGHT_UNITS*6+6.5 ) );
	pCurrentPlatform->setDimensions( glm::vec2( 5.0f, 1.0f ) );
	pCurrentPlatform->setStatic( true );
	pCurrentPlatform->activate();

	pCurrentPlatform = this->createRenderableEntity<CEntityPlatform>( m_pPlatformArray );
	if( !pCurrentPlatform )
		return false;
	pCurrentPlatform->setPosition( glm::vec2( CHUNK_HEIGHT_UNITS*6-2, CHUNK_HEIGHT_UNITS*6+6 ) );
	pCurrentPlatform->setDimensions( glm::vec2( 5.0f, 5.0f ) );
	pCurrentPlatform->setTexture( L"#FONT" );
	pCurrentPlatform->setStatic( true );
	pCurrentPlatform->activate();

	for( unsigned int i = 0; i < 10; i++ ) {
		CEntityStress *pStressTest = this->createRenderableEntity<CEntityStress>( m_pStressArray );
		if( !pStressTest )
			return false;
		pStressTest->setPosition( glm::vec2( CHUNK_HEIGHT_UNITS*6+1 + i * 0.1f, CHUNK_HEIGHT_UNITS*6+20 + ( i > 5 ? 0.2f : 0.0f ) ) );
		pStressTest->setOpaque( false );
		pStressTest->activate();
	}

	return true;
}
void CWorld::destroy()
{
	// Delete chunks
	DestroyDelete( m_pChunkManager );
	// Delete terrain generator
	DestroyDelete( m_pTerrainGenerator );
	// Delete blocks
	this->destroyBlocks();
	// Delete texture tilemaps
	DestroyDelete( m_pBlockTilemap );
	// Delete arrays
	m_pDrawArray->clearArray( false );
	DestroyDelete( m_pDrawArray );
	m_pPlayerArray->clearArray( true );
	DestroyDelete( m_pPlayerArray );
	m_pPlatformArray->clearArray( true );
	DestroyDelete( m_pPlatformArray );
	m_pStressArray->clearArray( true );
	DestroyDelete( m_pStressArray );
	m_pLocalPlayer = 0;
	// Destroy world
	SafeDelete( m_pPhysWorld );
	SafeDelete( m_pPhysListener );
}

bool CWorld::loadTilemaps()
{
	// Block tiles
	m_pBlockTilemap->addTile( L"env\\stone.png" ); // 0
	m_pBlockTilemap->addTile( L"env\\dirt.png" ); // 1
	m_pBlockTilemap->addTile( L"env\\grass.png" ); // 2
	if( !m_pBlockTilemap->generateTilemap( DEFAULT_TILEMAPSIZE, DEFAULT_TILESIZE ) )
		return false;

	return true;
}
bool CWorld::initBlocks()
{
	m_pBlockStone = new CBlockTerrain( BLOCK_ID_STONE );
	m_pBlockStone->setTextureIndex( TEXTURE_INDEX_STONE );
	this->registerBlock( m_pBlockStone );
	m_pBlockDirt = new CBlockTerrain( BLOCK_ID_DIRT );
	m_pBlockDirt->setTextureIndex( TEXTURE_INDEX_DIRT );
	this->registerBlock( m_pBlockDirt );
	m_pBlockGrass = new CBlockTerrain( BLOCK_ID_GRASS );
	m_pBlockGrass->setTextureIndex( TEXTURE_INDEX_GRASS );
	this->registerBlock( m_pBlockGrass );

	return true;
}
void CWorld::destroyBlocks()
{
	SafeDelete( m_pBlockStone );
	SafeDelete( m_pBlockDirt );
	SafeDelete( m_pBlockGrass );
}

void CWorld::draw( glm::mat4& orthoMat )
{
	CShaderProgram *pBaseProgram = CGame::getInstance().getGraphics()->getShaderManager()->getShaderProgram( L"base" );
	CShaderProgram *pDebugProgram = CGame::getInstance().getGraphics()->getShaderManager()->getShaderProgram( L"debug" );
	glm::mat4 modelMatrix, viewMatrix, mvpMatrix;

	// Center camera on local player, if exists
	if( m_pLocalPlayer && !m_bFlyCamera )
	{
		glm::vec2 screenOffset;
		float playerSize;

		playerSize = PLAYER_SPRITE_SIZE * m_pLocalPlayer->getPlayerScale();
		screenOffset = glm::vec2( CGame::getInstance().getConfig()->m_resolutionX / 2.0f / CGame::getInstance().getGraphics()->getPixelsPerMeter() - (playerSize/2.0f), CGame::getInstance().getConfig()->m_resolutionY / 2.0f / CGame::getInstance().getGraphics()->getPixelsPerMeter() - (playerSize/2.0f) );
		this->setCameraPosition( m_pLocalPlayer->getPosition() - screenOffset );
	}

	// Calculate view matrix
	viewMatrix = glm::translate( glm::mat4( 1.0f ), glm::vec3( -m_cameraPosition, 0.0f ) );
	// Update chunks
	m_pChunkManager->update( glm::ivec2( m_cameraPosition ) );

	// Sort the draw list
	m_pDrawArray->sortByLayer();

	// Draw the environment background
	m_pEnvironment->draw( ENV_LAYER_BACKGROUND, orthoMat );

	StartGLDebug( "DrawWorld" );

	CGame::getInstance().getGraphics()->getShaderManager()->bind( pBaseProgram );
	// Render chunks
	for( auto it = m_pChunkManager->getChunksRendered().begin(); it != m_pChunkManager->getChunksRendered().end(); it++ )
	{
		for( auto it2 = (*it).begin(); it2 != (*it).end(); it2++ )
		{
			modelMatrix = glm::translate( glm::mat4( 1.0f ), glm::vec3( (*it2)->getPosition() * glm::ivec2( CHUNK_WIDTH_UNITS, CHUNK_HEIGHT_UNITS ), CGraphics::getLayerPosition( LAYER_CHUNKBLOCKS ) ) );
			mvpMatrix = orthoMat * viewMatrix * modelMatrix;
			glUniformMatrix4fv( pBaseProgram->getUniform( "MVPMatrix" ), 1, GL_FALSE, &mvpMatrix[0][0] );
			(*it2)->draw();
		}
	}
	// Render each object
	for( auto it = m_pDrawArray->getEntityList().begin(); it != m_pDrawArray->getEntityList().end(); it++ )
	{
		// Translate
		modelMatrix = glm::translate( glm::mat4( 1.0f ), glm::vec3( (*it)->getPosition()-(*it)->getOrigin(), CGraphics::getLayerPosition( (*it)->getLayer() ) ) );
		// Rotate about the origin
		modelMatrix = glm::translate( modelMatrix, glm::vec3( (*it)->getOrigin(), 0.0f ) );
		modelMatrix = glm::rotate( modelMatrix, (*it)->getRotation(), glm::vec3( 0.0f, 0.0f, 1.0f ) );
		modelMatrix = glm::translate( modelMatrix, -glm::vec3( (*it)->getOrigin(), 0.0f ) );

		mvpMatrix = orthoMat * viewMatrix * modelMatrix;
		glUniformMatrix4fv( pBaseProgram->getUniform( "MVPMatrix" ), 1, GL_FALSE, &mvpMatrix[0][0] );
		(*it)->onDraw();
	}

	// Debug drawing
	if( m_bDebugDraw )
	{
		CGame::getInstance().getGraphics()->getShaderManager()->bind( pDebugProgram );
		// Chunks
		for( auto it = m_pChunkManager->getChunksRendered().begin(); it != m_pChunkManager->getChunksRendered().end(); it++ )
		{
			for( auto it2 = (*it).begin(); it2 != (*it).end(); it2++ )
			{
				modelMatrix = glm::translate( glm::mat4( 1.0f ), glm::vec3( (*it2)->getPosition() * glm::ivec2( CHUNK_WIDTH_UNITS, CHUNK_HEIGHT_UNITS ), CGraphics::getLayerPosition( LAYER_CHUNKBLOCKS+1 ) ) );
				mvpMatrix = orthoMat * viewMatrix * modelMatrix;
				glUniformMatrix4fv( pDebugProgram->getUniform( "MVPMatrix" ), 1, GL_FALSE, &mvpMatrix[0][0] );
				(*it2)->debugDraw();
			}
		}

		// Physics
		modelMatrix = glm::mat4( 1.0f );
		mvpMatrix = orthoMat * viewMatrix * modelMatrix;
		glUniformMatrix4fv( pDebugProgram->getUniform( "MVPMatrix" ), 1, GL_FALSE, &mvpMatrix[0][0] );
		m_pPhysWorld->DrawDebugData();
	}

	EndGLDebug();

}

void CWorld::checkKeypresses( double deltaT )
{
	CGameConfig *pConfig = CGame::getInstance().getConfig();
	CInput *pInput = CGame::getInstance().getInput();
	float distance;

	// Debug stuff
	if( pInput->isKeyPress( pConfig->getKeybind( KEYBIND_TOGGLEDEBUGDRAW ) ) )
		m_bDebugDraw = !m_bDebugDraw;
	if( pInput->isKeyPress( pConfig->getKeybind( KEYBIND_TOGGLEFLY ) ) ) {
		m_bFlyCamera = !m_bFlyCamera;
		if( m_pLocalPlayer )
			m_pLocalPlayer->setAsControlledPlayer( !m_bFlyCamera );
	}

	// Camera/player movement
	if( m_bFlyCamera )
	{
		if( pInput->isKeyHeld( pConfig->getKeybind( KEYBIND_WALK ) ) )
			distance = (float)(CEntityPlayer::BaseWalkSpeed*deltaT);
		else if( pInput->isKeyHeld( pConfig->getKeybind( KEYBIND_RUN ) ) )
			distance = (float)(CEntityPlayer::BaseRunSpeed*deltaT);
		else
			distance = (float)(CEntityPlayer::BaseMovementSpeed*deltaT);

		if( pInput->isKeyHeld( pConfig->getKeybind( KEYBIND_JUMP ) ) )
			m_cameraPosition += glm::vec2( 0, distance );
		if( pInput->isKeyHeld( pConfig->getKeybind( KEYBIND_CROUCH ) ) )
			m_cameraPosition -= glm::vec2( 0, distance );
		if( pInput->isKeyHeld( pConfig->getKeybind( KEYBIND_MOVE_RIGHT ) ) ) 
			m_cameraPosition += glm::vec2( distance, 0 );
		if( pInput->isKeyHeld( pConfig->getKeybind( KEYBIND_MOVE_LEFT ) ) )
			m_cameraPosition -= glm::vec2( distance, 0 );
	}
}
void CWorld::update( double deltaT )
{
	this->checkKeypresses( deltaT );

	// Only update if we've exceeded PHYSICAL_WORLD_TICK
	m_timeSinceLastUpdate += deltaT;
	if( m_timeSinceLastUpdate >= PHYSICAL_WORLD_TICK )
	{
		m_pPhysWorld->Step( PHYSICAL_WORLD_TICK, PHYSICAL_WORLD_VEL_IT, PHYSICAL_WORLD_POS_IT );
		m_pPhysWorld->ClearForces();
		m_timeSinceLastUpdate = 0;
		// Physics tick objects
		for( auto it = m_pDrawArray->getEntityList().begin(); it != m_pDrawArray->getEntityList().end(); it++ )
			(*it)->onPhysTick( deltaT );
	}
	// Update objects
	for( auto it = m_pDrawArray->getEntityList().begin(); it != m_pDrawArray->getEntityList().end(); it++ )
		(*it)->onUpdate( deltaT );
}

bool CWorld::registerBlock( CBlock *pBlock )
{
	// Make sure its not already registered
	if( m_registeredBlocks.find( pBlock->getBlockId() ) != m_registeredBlocks.end() ) {
		PrintWarn( L"Failed to register block because id %d was already taken\n", pBlock->getBlockId() );
		return false;
	}
	m_registeredBlocks.insert( std::pair<unsigned short, CBlock*>( pBlock->getBlockId(), pBlock ) );
	return true;
}
CBlock* CWorld::getBlock( unsigned short id ) {
	// Find it in the map
	if( m_registeredBlocks.find( m_pBlockStone->getBlockId() ) != m_registeredBlocks.end() )
		return m_registeredBlocks[id];
	return 0;
}

CEntityPlayer* CWorld::getLocalPlayer() {
	return m_pLocalPlayer;
}
b2World* CWorld::getPhysicalWorld() {
	return m_pPhysWorld;
}
CTerrainGenerator* CWorld::getTerrainGenerator() {
	return m_pTerrainGenerator;
}
CTextureTilemap* CWorld::getBlockTilemap() {
	return m_pBlockTilemap;
}
glm::vec2 CWorld::getCameraPosition() const {
	return m_cameraPosition;
}
void CWorld::setCameraPosition( glm::vec2 cameraPosition ) {
	m_cameraPosition = cameraPosition;
}