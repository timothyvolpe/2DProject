#pragma once

class CTexture2D;

enum : unsigned char
{
	ENV_LAYER_BACKGROUND,
	ENV_LAYER_FOREGROUND
};

enum : int
{
	ENV_DRAWMODE_SKY = 0,
	ENV_DRAWMODE_MOUNTAINS = 1
};

class CEnvironment
{
private:
	CTexture2D *m_pMountainTexture;
public:
	CEnvironment();
	~CEnvironment();

	bool initialize();
	void destroy();

	void draw( unsigned char worldLayer, glm::mat4 orthoMatrix );
};