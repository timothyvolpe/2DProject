#include "base.h"
#include "world\entities\entityplatform.h"
#include "texture.h"
#include "texturemanager.h"
#include "graphics.h"
#include "world\world.h"
#include "renderutil.h"

#include "interface\interfacemanager.h"
#include "interface\font.h"

CEntityPlatform::CEntityPlatform() {
	m_pPlatformTexture = 0;
	m_texturePath = L"dev\\crate01.png";
	m_pBody = 0;
	m_pFixture = 0;
	m_bStatic = false;
	m_dimensions = glm::vec2( 1.0f, 1.0f );
}
CEntityPlatform::~CEntityPlatform() {
}

bool CEntityPlatform::onCreate()
{
	this->setOrigin( m_dimensions / 2.0f );
	return true;
}
void CEntityPlatform::onDestroy()
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

bool CEntityPlatform::onActivate()
{
	TextureDescriptor textureDesc;
	b2BodyDef bodyDef;
	b2PolygonShape shapeDef;
	b2FixtureDef fixtureDef;
	float pixelsPerMeter = (float)CGame::getInstance().getGraphics()->getPixelsPerMeter();

	// Create the physical body
	if( m_bStatic )
		bodyDef.type = b2_staticBody;
	else
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
	m_pFixture = m_pBody->CreateFixture( &fixtureDef );

	// Load the texture
	textureDesc.magFilter = GL_NEAREST;
	textureDesc.minFilter = GL_NEAREST;
	textureDesc.wrapS = GL_CLAMP_TO_EDGE;
	textureDesc.wrapT = GL_CLAMP_TO_EDGE;
	textureDesc.useAlpha = !this->isOpaque();
	textureDesc.monochrome = false;

	if( m_texturePath == L"#FONT" ) {
		m_pPlatformTexture = CGame::getInstance().getInterfaceManager()->getFont( L"DEFAULT_FONT" )->getFontMap();
		return true;
	}
	else {
		m_pPlatformTexture = CGame::getInstance().getGraphics()->getTextureManager()->loadTexture2D( m_texturePath, textureDesc );
		if( !m_pPlatformTexture )
			return false;
	}
	return true;
}

void CEntityPlatform::setTexture( std::wstring texture )
{
	m_texturePath = texture;
}

void CEntityPlatform::onDraw()
{
	// Bind texture
	m_pPlatformTexture->bind(0);

	// Draw a box
	CRenderUtil::drawSpriteTextured( glm::vec2( 0.0f, 0.0f ), m_dimensions, glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f ) );
}

void CEntityPlatform::onUpdate( double deltaT )
{
	this->setPosition( m_pBody->GetPosition() );
	this->setRotation( m_pBody->GetAngle() * (180 / PI_CONSTANT) );
}

void CEntityPlatform::setStatic( bool isStatic ) {
	m_bStatic = isStatic;
}
void CEntityPlatform::setDimensions( glm::vec2 dim ) {
	m_dimensions = dim;
	this->setOrigin( m_dimensions / 2.0f );
}