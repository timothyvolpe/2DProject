#pragma once

#include <Box2D\Box2D.h>

enum
{
	USER_DATA_TYPE_UNKNOWN = 0,
	USER_DATA_TYPE_ENTITIES,
	USER_DATA_TYPE_CHUNKS
};

class CEntityPhysics;
class CEntityRenderablePhysics;

struct PhysicsUserdata {

	PhysicsUserdata() : pPhysicsObj( 0 ), pRenderablePhysicsObj( 0 ), safety( 12345 ) {}

	int safety;

	int type;
	CEntityPhysics *pPhysicsObj;
	CEntityRenderablePhysics *pRenderablePhysicsObj;
};

class CContactListener : public b2ContactListener
{
public:
	CContactListener();
	~CContactListener();

	void BeginContact( b2Contact* pContact );
	void EndContact( b2Contact* pContact );
};