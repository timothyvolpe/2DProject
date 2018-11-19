#include "base.h"
#include "world\entities\entitystress.h"
#include "graphics.h"
#include "texture.h"
#include "texturemanager.h"
#include "world\world.h"
#include "world\spritemanager.h"
#include "renderutil.h"

CEntityStress::CEntityStress() {
	m_pFixture = 0;
	m_pBody = 0;
	m_dimensions = glm::vec2( 0.4f, 0.2f );
}
CEntityStress::~CEntityStress() {
}

bool CEntityStress::onCreate()
{
	CWorld *pWorld =  CGame::getInstance().getWorld();
	CTextureTilemap *pTilemap = pWorld->getSpriteBatchTilemap( SPRITE_BATCH_ITEMS );

	// Get texture and batch info
	if( pTilemap ) {
		unsigned short tileIndex = pTilemap->getTileIndex( L"dev\\key.png" );
		this->setSpriteBatch( pTilemap->getBatchId() );
		this->setSpriteTile( tileIndex, pTilemap->getTileCoords( tileIndex ) );
	}
	else
		return false;

	return true;
}
void CEntityStress::onDestroy()
{
	if( m_pBody ) {
		if( m_pFixture ) {
			m_pBody->DestroyFixture( m_pFixture );
			m_pFixture = 0;
		}
		CGame::getInstance().getWorld()->getPhysicalWorld()->DestroyBody( m_pBody );
		m_pBody = 0;
	}
}

bool CEntityStress::onActivate()
{
	TextureDescriptor textureDesc;
	b2BodyDef bodyDef;
	b2PolygonShape shapeDef;
	b2FixtureDef fixtureDef;

	// Create the physical body
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set( (float32)this->getPosition().x, (float32)this->getPosition().y );
	bodyDef.userData = 0;
	bodyDef.fixedRotation = false;
	m_pBody = CGame::getInstance().getWorld()->getPhysicalWorld()->CreateBody( &bodyDef );
	this->setUserdata( 0, this, m_pBody );
	// Shape
	shapeDef.SetAsBox( m_dimensions.x / 2.0f, m_dimensions.y / 2.0f );
	fixtureDef.shape = &shapeDef;
	fixtureDef.density = 1.0f;
	fixtureDef.restitution = 0.2f;
	m_pFixture = m_pBody->CreateFixture( &fixtureDef );

	this->setOrigin( m_dimensions / 2.0f );

	return true;
}

void CEntityStress::onDraw()
{
	float keyWidth, keyHeight;
	SpriteData sd;

	this->setPosition( m_pBody->GetPosition() );
	this->setRotation( m_pBody->GetAngle() * (180 / PI_CONSTANT) );

	keyWidth = 0.2f;
	keyHeight = 0.1f;

	sd.layer = LAYER_PLAYER;
	sd.position = this->getPosition();
	sd.rotation = this->getRotation();
	sd.size = m_dimensions;
	sd.texcoords = this->getTextureTileCoords();
	CGame::getInstance().getWorld()->getSpriteManager()->drawSprite( this->getBatchId(), sd );
}