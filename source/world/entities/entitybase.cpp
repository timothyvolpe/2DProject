#include "base.h"
#include "world\entities\entitybase.h"
#include "world\world.h"

/////////////////
// CEntityBase //
/////////////////

std::wstring CEntityBase::m_name = L"unknown";
std::wstring CEntityBase::getName() {
	return m_name;
}

CEntityBase::CEntityBase() {
	m_position = glm::vec2( 0.0f, 0.0f );
	m_origin = glm::vec2( 0.0f, 0.0f );
	m_rotation = 0.0f;
	m_bActivated = false;
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

CEntityRenderable::CEntityRenderable() {
	m_layer = 127;
	m_bOpaque = true;
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

bool EntityRenderableLayerSort::operator()( const CEntityRenderable *pFirst, const CEntityRenderable *pSecond ) {
	return (pFirst->getLayer() < pSecond->getLayer());
}

////////////////////////
// CEntityPhysicsBase //
////////////////////////

CEntityPhysicsBase::CEntityPhysicsBase() {
	m_pUserdata = new PhysicsUserdata();
	m_pUserdata->pPhysicsObj = 0;
	m_pUserdata->pRenderablePhysicsObj = 0;
}
CEntityPhysicsBase::~CEntityPhysicsBase() {
	SafeDelete( m_pUserdata );
}
void CEntityPhysicsBase::setUserdata( CEntityPhysics *pPhysicsObj, CEntityRenderablePhysics *pRenderablePhysicsObj, b2Body *pBody ) {
	m_pUserdata->pPhysicsObj = pPhysicsObj;
	m_pUserdata->pRenderablePhysicsObj = pRenderablePhysicsObj;
	pBody->SetUserData( m_pUserdata );
}

////////////////////
// CEntityPhysics //
////////////////////

CEntityPhysics::CEntityPhysics() {
}
CEntityPhysics::~CEntityPhysics() {
}

//////////////////////////////
// CEntityRenderablePhysics //
//////////////////////////////

CEntityRenderablePhysics::CEntityRenderablePhysics() {
}
CEntityRenderablePhysics::~CEntityRenderablePhysics() {
}