#pragma once

#include <Box2D\Box2D.h>

class CContactListener : public b2ContactListener
{
public:
	CContactListener();
	~CContactListener();

	void BeginContact( b2Contact* pContact );
	void EndContact( b2Contact* pContact );
};