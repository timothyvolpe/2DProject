#pragma once

#include <vector>

//////////////////////
// CEntityArrayBase //
//////////////////////

class CEntityArrayBase
{
public:
	CEntityArrayBase();
	virtual ~CEntityArrayBase();

	virtual bool initialize() = 0;
	virtual void destroy() = 0;

	virtual void clearArray( bool destroyEntities ) = 0;
};

//////////////////
// CEntityArray //
//////////////////

template <class T>
class CEntityArray : public CEntityArrayBase
{
protected:
	typedef T EntityType;
	typedef std::vector<EntityType*> EntityList;

	EntityList m_entityList;
public:
	CEntityArray() {
	}
	virtual ~CEntityArray() {
	}

	bool initialize() {
		return true;
	}
	void destroy() {

	}

	EntityType* createEntity()
	{
		EntityType *pEntity;

		// Create the entity
		pEntity = new EntityType();
		if( !pEntity->initialize() )
			return NULL;
		if( !pEntity->onCreate() )
			return NULL;
		// Add it to the array
		m_entityList.push_back( pEntity );

		return pEntity;
	}

	void addEntity( EntityType *pEntity ) {
		m_entityList.push_back( pEntity );
	}
	void removeEntity( EntityType *pEntity ) {
		// Find it in the array and remove it
		for( EntityList::iterator it = m_entityList.begin(); it != m_entityList.end(); it++ ) {
			if( (*it) == pEntity ) {
				m_entityList.erase( (*it) );
				break;
			}
		}
		PrintWarn( L"Failed to remove entity from array because the entity was not found" );
	}

	void clearArray( bool destroyEntities )
	{
		// Delete entities
		if( destroyEntities ) {
			for( EntityList::iterator it = m_entityList.begin(); it != m_entityList.end(); it++ ) {
				(*it)->onDestroy();
				DestroyDelete( (*it) );
			}
		}
		m_entityList.clear();
	}
	
	EntityList& getEntityList() {
		return m_entityList;
	}

	unsigned int getEntityCount() const {
		return m_entityList.size();
	}
};

//////////////////////////
// CEntityArraySortable //
//////////////////////////

template <class T>
class CEntityArraySortable : public CEntityArray<T>
{
public:
	CEntityArraySortable() {
	}
	~CEntityArraySortable() {
	}
	
	void sortByLayer() {
		std::sort( m_entityList.begin(), m_entityList.end(), EntityRenderableLayerSort() );
	}
};