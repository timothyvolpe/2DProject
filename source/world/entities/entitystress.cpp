#include "base.h"
#include "world\entities\entitystress.h"
#include "graphics.h"
#include "texture.h"
#include "texturemanager.h"
#include "world\world.h"
#include "renderutil.h"

CEntityStress::CEntityStress() {
	m_pTexture = 0;
	m_pFixture = 0;
	m_pBody = 0;
	m_dimensions = glm::vec2( 0.4f, 0.2f );
}
CEntityStress::~CEntityStress() {
}

bool CEntityStress::onCreate()
{
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

	// Load the texture
	textureDesc.magFilter = GL_NEAREST;
	textureDesc.minFilter = GL_NEAREST;
	textureDesc.wrapS = GL_CLAMP_TO_EDGE;
	textureDesc.wrapT = GL_CLAMP_TO_EDGE;
	textureDesc.useAlpha = !this->isOpaque();
	textureDesc.monochrome = false;
	m_pTexture = CGame::getInstance().getGraphics()->getTextureManager()->loadTexture2D( L"dev\\key.png", textureDesc );
	if( !m_pTexture )
		return false;

	this->setOrigin( m_dimensions / 2.0f );

	return true;
}

void CEntityStress::onDraw()
{
	float keyWidth, keyHeight;

	m_pTexture->bind( 0 );

	this->setPosition( m_pBody->GetPosition() );
	this->setRotation( m_pBody->GetAngle() * (180 / PI_CONSTANT) );

	keyWidth = 0.2f;
	keyHeight = 0.1f;

	// Draw a box
	CRenderUtil::drawSpriteTextured( glm::vec2( 0.0f, 0.0f ), m_dimensions, glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f ) );
}