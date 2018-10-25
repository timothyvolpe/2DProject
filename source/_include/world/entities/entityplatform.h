#pragma once

#include <Box2D\Box2D.h>
#include "entitybase.h"

class CTexture2D;

class CEntityPlatform : public CEntityRenderablePhysics
{
private:
	CTexture2D *m_pPlatformTexture;

	std::wstring m_texturePath;
	bool m_bStatic;
	glm::vec2 m_dimensions;

	b2Fixture *m_pFixture;
	b2Body *m_pBody;
public:
	CEntityPlatform();
	~CEntityPlatform();

	bool onCreate();
	void onDestroy();

	bool onActivate();

	void onDraw();

	void onUpdate( double deltaT );

	void setTexture( std::wstring texture );
	void setStatic( bool isStatic );
	void setDimensions( glm::vec2 dim );
};