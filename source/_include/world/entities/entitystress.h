#pragma once

#include <Box2D\Box2D.h>
#include "entitybase.h"

class CTexture;

class CEntityStress : public CEntityRenderablePhysics
{
private:
	CTexture *m_pTexture;

	b2Fixture *m_pFixture;
	b2Body *m_pBody;

	glm::vec2 m_dimensions;
public:
	CEntityStress();
	~CEntityStress();

	bool onCreate();
	void onDestroy();

	bool onActivate();
	
	void onDraw();
};
