#include "base.h"
#include "world\entities\entityplayer.h"
#include "config.h"
#include "input.h"
#include "graphics.h"
#include "texture.h"
#include "texturemanager.h"
#include "world\world.h"
#include "world\spritemanager.h"
#include "renderutil.h"

float CEntityPlayer::BaseMovementSpeed = 5.0f;
float CEntityPlayer::BaseRunSpeed = 10.0f;
float CEntityPlayer::BaseWalkSpeed = 2.5f;

CEntityPlayer::CEntityPlayer()
{
	m_bControlledPlayer = true;

	m_fPlayerScale = 1.0f;
	m_fPlayerMaxSpeed = 6.0f;
	
	m_movementFlags = 0;
	m_iWalkableSurfacesContacting = 0;

	m_jumpCooldown = 0;
}
CEntityPlayer::~CEntityPlayer()
{
}

bool CEntityPlayer::onCreate()
{
	CWorld *pWorld =  CGame::getInstance().getWorld();
	CTextureTilemap *pTilemap = pWorld->getSpriteBatchTilemap( SPRITE_BATCH_LIVING );

	// Get texture and batch info
	if( pTilemap ) {
		unsigned short tileIndex = pTilemap->getTileIndex( L"dev\\player.png" );
		this->setSpriteBatch( pTilemap->getBatchId() );
		this->setSpriteTile( tileIndex, pTilemap->getTileCoords( tileIndex ) );
	}
	else
		return false;

	return true;
}
void CEntityPlayer::onDestroy()
{
	if( m_pPlayerBody ) {
		if( m_pPlayerFixture ) {
			m_pPlayerBody->DestroyFixture( m_pPlayerFixture );
			m_pPlayerFixture = 0;
		}
		if( m_pPlayerJumpSensor ) {
			m_pPlayerBody->DestroyFixture( m_pPlayerJumpSensor );
			m_pPlayerJumpSensor = 0;
		}
		CGame::getInstance().getWorld()->getPhysicalWorld()->DestroyBody( m_pPlayerBody );
		m_pPlayerBody = 0;
	}
}

bool CEntityPlayer::onActivate()
{
	b2BodyDef bodyDef;
	b2PolygonShape shapeDef;
	b2FixtureDef fixtureDef;
	float playerSize;

	// Initialize 
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set( (float32)this->getPosition().x, (float32)this->getPosition().y );
	bodyDef.userData = 0;
	bodyDef.fixedRotation = true;
	m_pPlayerBody = CGame::getInstance().getWorld()->getPhysicalWorld()->CreateBody( &bodyDef );
	this->setUserdata( 0, this, m_pPlayerBody );
	// Player fixture
	playerSize = PLAYER_SPRITE_SIZE * m_fPlayerScale;
	shapeDef.SetAsBox( playerSize / 2.0f, playerSize / 2.0f, b2Vec2( playerSize / 2.0f, playerSize / 2.0f ), 0.0f );
	fixtureDef.shape = &shapeDef;
	fixtureDef.density = 1.0f;
	m_pPlayerFixture = m_pPlayerBody->CreateFixture( &fixtureDef );
	m_pPlayerFixture->SetUserData( (void*)PLAYERFIXTUREID_BODY );
	// Jump sensor
	shapeDef.SetAsBox( 0.3f, 0.025f, b2Vec2( playerSize / 2.0f, 0.0f ), 0.0f );
	fixtureDef.isSensor = true;
	m_pPlayerJumpSensor = m_pPlayerBody->CreateFixture( &fixtureDef );
	m_pPlayerJumpSensor->SetUserData( (void*)PLAYERFIXTUREID_JUMPSENSOR );

	return true;
}

void CEntityPlayer::onDraw()
{
	float playerSize;
	glm::vec2 screenOffset;
	SpriteData sd;

	playerSize = PLAYER_SPRITE_SIZE * m_fPlayerScale;

	// Draw a box
	sd.layer = LAYER_PLAYER;
	sd.position = this->getPosition();
	sd.rotation = 0.0f;
	sd.size = glm::vec2( playerSize );
	sd.texcoords = this->getTextureTileCoords();
	CGame::getInstance().getWorld()->getSpriteManager()->drawSprite( this->getBatchId(), sd );
}

void CEntityPlayer::performMovement( double deltaT )
{
	CGameConfig *pConfig = CGame::getInstance().getConfig();
	CInput *pInput = CGame::getInstance().getInput();

	m_movementFlags = 0;
	if( pInput->isKeyHeld( pConfig->getKeybind( KEYBIND_JUMP ) ) )
		m_movementFlags |= MOVEMENTSTATE_JUMP;
	if( pInput->isKeyHeld( pConfig->getKeybind( KEYBIND_CROUCH ) ) )
		m_movementFlags |= MOVEMENTSTATE_CROUCH;
	if( pInput->isKeyHeld( pConfig->getKeybind( KEYBIND_MOVE_RIGHT ) ) )
		m_movementFlags |= MOVEMENTSTATE_RIGHT;
	if( pInput->isKeyHeld( pConfig->getKeybind( KEYBIND_MOVE_LEFT ) ) )
		m_movementFlags |= MOVEMENTSTATE_LEFT;
}
void CEntityPlayer::onUpdate( double deltaT )
{
	float playerSizeOffset;

	// Check for movement commands
	if( m_bControlledPlayer )
		this->performMovement( deltaT );
	// Move sprite to physics
	playerSizeOffset = (PLAYER_SPRITE_SIZE * m_fPlayerScale)/2;
	this->setPosition( m_pPlayerBody->GetPosition() + b2Vec2( playerSizeOffset, playerSizeOffset ) );
}
void CEntityPlayer::onPhysTick( double deltaT )
{
	b2Vec2 velocity, targetVelocity, impulse;
	float jumpImpulse;

	// Decrease the jump cooldown
	m_jumpCooldown -= deltaT;

	velocity = m_pPlayerBody->GetLinearVelocity();
	targetVelocity = b2Vec2( 0, 0 );

	if( m_movementFlags & MOVEMENTSTATE_JUMP && m_iWalkableSurfacesContacting > 0 && m_jumpCooldown <= 0 ) {
		jumpImpulse = m_pPlayerBody->GetMass() * 3.0f;
		m_pPlayerBody->ApplyLinearImpulse( b2Vec2( 0, jumpImpulse ), m_pPlayerBody->GetWorldCenter(), true );
		m_jumpCooldown = PLAYER_JUMP_COOLDOWN;
	}
	// Apply movement forces
	if( m_movementFlags & MOVEMENTSTATE_LEFT ) {
		if( velocity.x > -m_fPlayerMaxSpeed )
			targetVelocity.x += -0.2f;
	}
	if( m_movementFlags & MOVEMENTSTATE_RIGHT ) {
		if( velocity.x < m_fPlayerMaxSpeed )
			targetVelocity.x += 0.2f;
	}

	impulse = m_pPlayerBody->GetMass() * targetVelocity;
	m_pPlayerBody->ApplyLinearImpulse( impulse, m_pPlayerBody->GetWorldCenter(), true );
}

void CEntityPlayer::onPhysicsBeginContact( b2Fixture *pContactingFixture, CEntityPhysics *pPhysicsObj, CEntityRenderablePhysics *pRenderablePhysicsObj )
{
	int fixtureId;

	// Check the fixture ID
	fixtureId = (int)pContactingFixture->GetUserData();
	// Check if we're contacting a valid walkable surface
	if( fixtureId == PLAYERFIXTUREID_JUMPSENSOR ) {
		if( pRenderablePhysicsObj )
			m_iWalkableSurfacesContacting++;
	}
}
void CEntityPlayer::onPhysicsEndContact( b2Fixture *pContactingFixture, CEntityPhysics *pPhysicsObj, CEntityRenderablePhysics *pRenderablePhysicsObj )
{
	int fixtureId;

	// Check the fixture ID
	fixtureId = (int)pContactingFixture->GetUserData();
	// Check if we're leaving a valid walkable surface
	if( fixtureId == PLAYERFIXTUREID_JUMPSENSOR ) {
		if( pRenderablePhysicsObj )
			m_iWalkableSurfacesContacting--;
	}
}

bool CEntityPlayer::isControlledPlayer() const {
	return m_bControlledPlayer;
}
void CEntityPlayer::setAsControlledPlayer( bool isControlledPlayer ) {
	m_bControlledPlayer = isControlledPlayer;
}
float CEntityPlayer::getPlayerScale() const {
	return m_fPlayerScale;
}
void CEntityPlayer::setPlayerScale( float scale ) {
	m_fPlayerScale = scale;
}