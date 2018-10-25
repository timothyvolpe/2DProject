#pragma once

#define LAYER_SIZE 0.1f

#include <sdl.h>
#include <glm\glm.hpp>

class CShaderManager;
class CTextureManager;

enum : unsigned char
{
	LAYER_BASE = 0,
	LAYER_WORLD_BACKGROUND = 1,
	LAYER_CHUNKBLOCKS = 10,
	LAYER_DEFAULT = 127,
	LAYER_PLAYER = 160,
	LAYER_INTERFACE = 200,
	LAYER_WORLD_FOREGROUND = 250
};

class CGraphics
{
private:
	SDL_Window *m_pGameWindowHandle;
	SDL_GLContext m_GLContext;

	CShaderManager *m_pShaderManager;
	CTextureManager *m_pTextureManager;

	void setupGraphics();

	glm::mat4 m_orthoMatrix;
	glm::mat4 m_interfaceOrthoMatrix;

	int m_pixelsPerMeter;
	bool m_bWireframe;
public:
	static float getLayerPosition( unsigned char layer );

	CGraphics();
	~CGraphics();

	bool initialize();
	void destroy();

	void draw();
	void update();

	void updateProjection();

	SDL_GLContext getGLContext() const;
	glm::mat4 getOrthographicMatrix() const;
	glm::mat4 getInterfaceOrthoMatrix() const;
	CShaderManager* getShaderManager() const;
	CTextureManager* getTextureManager() const;
	int getPixelsPerMeter() const;
	void setPixelsPerMeter( int pixelsPerMeter );
};
