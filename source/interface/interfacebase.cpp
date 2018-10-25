#include <algorithm>
#include "base.h"
#include "interface\interfacebase.h"
#include "graphics.h"

////////////////////
// CInterfaceBase //
////////////////////

CInterfaceBase::CInterfaceBase() {
	m_position = glm::ivec2( 0, 0 );
	m_size = glm::ivec2( 0, 0 );
	m_pParent = 0;
	m_name = L"";
	m_layer = LAYER_INTERFACE;
}
CInterfaceBase::~CInterfaceBase() {
}

bool CInterfaceBase::initialize( std::wstring name )
{
	m_name = name;

	if( !this->onCreate() )
		return false;

	return true;
}
void CInterfaceBase::destroy() {
	this->onDestroy();
}
bool CInterfaceBase::activate() {
	if( !this->onActivate() )
		return false;
	return true;
}

glm::ivec2 CInterfaceBase::getRelativePosition() const {
	return m_position;
}
glm::ivec2 CInterfaceBase::getAbsolutePosition() const
{
	if( m_pParent ) {
		return m_pParent->getAbsolutePosition() - m_position;
	}
	else
		return m_position;
}
void CInterfaceBase::setPosition( glm::ivec2 pos ) {
	glm::ivec2 oldPos = m_position;
	m_position = pos;
	this->onPositionChange( oldPos );
}
glm::ivec2 CInterfaceBase::getSize() const {
	return m_size;
}
void CInterfaceBase::setSize( glm::ivec2 size ) {
	glm::ivec2 oldSize = m_size;
	m_size = size;
	this->onSizeChange( oldSize );
}
std::wstring CInterfaceBase::getName() const {
	return m_name;
}
unsigned char CInterfaceBase::getLayer() const {
	return m_layer;
}
void CInterfaceBase::setLayer( unsigned char layer ) {
	m_layer = layer;
}

CInterfaceParent* CInterfaceBase::getParent() {
	return m_pParent;
}
void CInterfaceBase::setParent( CInterfaceParent *pParent, bool notifyParent )
{
	CInterfaceParent *pOldParent;

	pOldParent = m_pParent;
	m_pParent = pParent;
	if( pOldParent && notifyParent )
		pOldParent->removeChild( this, false );
	if( notifyParent && pParent )
		m_pParent->addChild( this, false );
	this->onParentChange( pOldParent );
}

//////////////////////
// CInterfaceParent //
//////////////////////

CInterfaceParent::CInterfaceParent() {
}
CInterfaceParent::~CInterfaceParent() {
}

bool CInterfaceParent::initialize( std::wstring name )
{
	if( !CInterfaceBase::initialize( name ) )
		return false;

	return true;
}
void CInterfaceParent::destroy()
{
	this->clearChildren();
	CInterfaceBase::destroy();
}

bool CInterfaceParent::addChild( CInterfaceBase *pChild, bool notifyChild )
{
	// Check if its already in the list
	if( std::find( m_children.begin(), m_children.end(), pChild ) != m_children.end() ) {
		PrintWarn( L"Parent %s already contains child %s\n", this->getName().c_str(), pChild->getName().c_str() );
		return false;
	}
	// Set parent
	m_children.push_back( pChild );
	if( notifyChild )
		pChild->setParent( this, false );

	return true;
}
bool CInterfaceParent::removeChild( CInterfaceBase *pChild, bool notifyChild )
{
	bool foundChild;

	// Check if we're actually the parent, then remove
	foundChild = false;
	for( auto it = m_children.begin(); it != m_children.end(); it++ ) {
		if( (*it) == pChild ) {
			m_children.erase( it );
			if( notifyChild )
				pChild->setParent( 0, false );
			foundChild = true;
			break;
		}
	}
	// We didn't find it
	if( !foundChild ) {
		PrintWarn( L"Parent %s does not contain child %s\n", this->getName().c_str(), pChild->getName().c_str() );
		return false;
	}

	return true;
}
void CInterfaceParent::clearChildren()
{
	for( auto it = m_children.begin(); it != m_children.end(); it++ ) {
		(*it)->setParent( 0, false );
	}
	m_children.clear();
}

std::vector<CInterfaceBase*> CInterfaceParent::getChildren() const {
	return m_children;
}