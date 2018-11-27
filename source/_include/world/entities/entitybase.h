#pragma once

#include <glm\glm.hpp>
#include <Box2D\Box2D.h>
#include <string>

/////////////////
// CEntityBase //
/////////////////

class CEntityBase
{
private:
	static std::wstring m_name;

	glm::vec2 m_position;
	glm::vec2 m_origin;
	float m_rotation;

	bool m_bActivated;
public:
	static std::wstring getName();

	CEntityBase();
	virtual ~CEntityBase();

	bool initialize();
	void destroy();

	bool activate();

	virtual bool onCreate() = 0;
	virtual void onDestroy() = 0;
	virtual bool onActivate() { return true; }

	virtual void onPositionChange() {}
	virtual void onOriginChange() {}
	virtual void onRotationChange() {}

	virtual void onUpdate( double deltaT ) {}
	virtual void onPhysTick( double deltaT ) {}

	glm::vec2 getPosition() const;
	void setPosition( glm::vec2 pos );
	void setPosition( b2Vec2 pos );
	glm::vec2 getOrigin() const;
	void setOrigin( glm::vec2 origin );
	float getRotation() const;
	void setRotation( float rot );

	virtual bool isRenderable() { return false; }
};

///////////////////////
// CEntityRenderable //
///////////////////////

class CEntityRenderable : public CEntityBase
{
private:
	unsigned char m_layer;

	int m_spriteBatchId;
	unsigned short m_textureTileIndex;
	glm::lowp_uvec4 m_textureTileCoords;

	std::wstring m_texturePath;

	bool m_bOpaque;
public:
	CEntityRenderable();
	virtual ~CEntityRenderable();

	virtual void onDraw() = 0;

	virtual void onLayerChange() {}

	unsigned char getLayer() const;
	void setLayer( unsigned char layer );
	bool isOpaque() const;
	void setOpaque( bool opaque );

	bool isRenderable() { return true; }

	void setTexture( std::wstring texture, unsigned char batchCode );
	void setSpriteBatch( int batchId );
	void setSpriteTile( unsigned short index, glm::lowp_uvec4 coords );
	int getBatchId();
	unsigned short getTileIndex();
	glm::lowp_uvec4 getTextureTileCoords();
};

struct EntityRenderableLayerSort {
	bool operator()( const CEntityRenderable *pFirst, const CEntityRenderable *pSecond );
};

////////////////////////
// CEntityPhysicsBase //
////////////////////////

class CEntityPhysics;
class CEntityRenderablePhysics;

struct PhysicsUserdata {
	CEntityPhysics *pPhysicsObj;
	CEntityRenderablePhysics *pRenderablePhysicsObj;
};

class CEntityPhysicsBase
{
private:
	PhysicsUserdata *m_pUserdata;
protected:
	void setUserdata( CEntityPhysics *pPhysicsObj, CEntityRenderablePhysics *pRenderablePhysicsObj, b2Body *pBody );
public:
	CEntityPhysicsBase();
	~CEntityPhysicsBase();

	virtual void onPhysicsBeginContact( b2Fixture *pContactingFixture, CEntityPhysics *pPhysicsObj, CEntityRenderablePhysics *pRenderablePhysicsObj ) {}
	virtual void onPhysicsEndContact( b2Fixture *pContactingFixture, CEntityPhysics *pPhysicsObj, CEntityRenderablePhysics *pRenderablePhysicsObj ) {}
};

////////////////////
// CEntityPhysics //
////////////////////

class CEntityPhysics : public CEntityBase, public CEntityPhysicsBase
{
public:
	CEntityPhysics();
	~CEntityPhysics();
};

//////////////////////////////
// CEntityRenderablePhysics //
//////////////////////////////

class CEntityRenderablePhysics : public CEntityRenderable, public CEntityPhysicsBase
{
public:
	CEntityRenderablePhysics();
	~CEntityRenderablePhysics();
};