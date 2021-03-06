#include "base.h"
#include "world\entities\entitybase.h"
#include "world\world.h"
#include "texture.h"
#include "world\physlistener.h"

/////////////////
// CEntityBase //
/////////////////

const wchar_t * CEntityBase::getName() {
	return m_pName;
}

CEntityBase::CEntityBase() {
	m_position = glm::vec2( 0.0f, 0.0f );
	m_origin = glm::vec2( 0.0f, 0.0f );
	m_rotation = 0.0f;
	m_bActivated = false;
	m_pName = L"unknown";
}
CEntityBase::~CEntityBase() {
}

bool CEntityBase::initialize()
{
	return true;
}
void CEntityBase::destroy()
{
}

bool CEntityBase::activate()
{
	m_bActivated = true;
	if( !this->onActivate() )
		return false;
	return true;
}

glm::vec2 CEntityBase::getPosition() const {
	return m_position;
}
void CEntityBase::setPosition( glm::vec2 pos ) {
	m_position = pos;
	this->onPositionChange();
}
void CEntityBase::setPosition( b2Vec2 pos ) {
	glm::vec2 glmpos = glm::vec2( pos.x, pos.y );
	this->setPosition( glmpos );
}
glm::vec2 CEntityBase::getOrigin() const {
	return m_origin;
}
void CEntityBase::setOrigin( glm::vec2 origin ) {
	m_origin = origin;
	this->onOriginChange();
}
float CEntityBase::getRotation() const {
	return m_rotation;
}
void CEntityBase::setRotation( float rot ) {
	m_rotation = rot;
	this->onRotationChange();
}


///////////////////////
// CEntityRenderable //
///////////////////////

CEntityRenderable::CEntityRenderable() : CEntityBase() {
	m_layer = 127;
	m_bOpaque = true;

	m_spriteBatchId = -1;
	m_textureTileIndex = 0;

	m_texturePath = L"ERROR";
}
CEntityRenderable::~CEntityRenderable() {
}

unsigned char CEntityRenderable::getLayer() const {
	return m_layer;
}
void CEntityRenderable::setLayer( unsigned char layer ) {
	m_layer = layer;
	this->onLayerChange();
}
bool CEntityRenderable::isOpaque() const {
	return m_bOpaque;
}
void CEntityRenderable::setOpaque( bool opaque ) {
	m_bOpaque = opaque;
}

void CEntityRenderable::setTexture( std::wstring texture, unsigned char batchCode )
{
	CWorld *pWorld =  CGame::getInstance().getWorld();
	CTextureTilemap *pTilemap = pWorld->getSpriteBatchTilemap( batchCode );
	unsigned short tileIndex;

	// Get texture and batch info
	assert( pTilemap );
	m_texturePath = texture;
	tileIndex = pTilemap->getTileIndex( m_texturePath );
	this->setSpriteBatch( pTilemap->getBatchId() );
	this->setSpriteTile( tileIndex, pTilemap->getTileCoords( tileIndex ) );
	
}
void CEntityRenderable::setSpriteBatch( int batchId ) {
	m_spriteBatchId = batchId;
}
void CEntityRenderable::setSpriteTile( unsigned short index, glm::lowp_uvec4 coords ) {
	m_textureTileIndex = index;
	m_textureTileCoords = coords;
}
int CEntityRenderable::getBatchId() { return m_spriteBatchId; }
unsigned short CEntityRenderable::getTileIndex() { return m_textureTileIndex; }
glm::lowp_uvec4 CEntityRenderable::getTextureTileCoords() { return m_textureTileCoords; }

bool EntityRenderableLayerSort::operator()( const CEntityRenderable *pFirst, const CEntityRenderable *pSecond ) {
	return (pFirst->getLayer() < pSecond->getLayer());
}

////////////////////////
// CEntityPhysicsBase //
////////////////////////

CEntityPhysicsBase::CEntityPhysicsBase() {
	m_pUserdata = new PhysicsUserdata();
}
CEntityPhysicsBase::~CEntityPhysicsBase() {
	SafeDelete( m_pUserdata );
}
void CEntityPhysicsBase::setUserdata( CEntityPhysics *pPhysicsObj, CEntityRenderablePhysics *pRenderablePhysicsObj, b2Body *pBody ) {
	m_pUserdata->type = USER_DATA_TYPE_ENTITIES;
	m_pUserdata->pPhysicsObj = pPhysicsObj;
	m_pUserdata->pRenderablePhysicsObj = pRenderablePhysicsObj;
	pBody->SetUserData( m_pUserdata );
}

////////////////////
// CEntityPhysics //
////////////////////

CEntityPhysics::CEntityPhysics() : CEntityBase(), CEntityPhysicsBase() {
}
CEntityPhysics::~CEntityPhysics() {
}

//////////////////////////////
// CEntityRenderablePhysics //
//////////////////////////////

CEntityRenderablePhysics::CEntityRenderablePhysics() : CEntityRenderable(), CEntityPhysicsBase() {
}
CEntityRenderablePhysics::~CEntityRenderablePhysics() {
}