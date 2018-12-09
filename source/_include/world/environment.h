#pragma once

#define ENV_QUAD_COUNT 2

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

#pragma pack(push, 4)
typedef struct {
	glm::vec2 pos, texcoords;
	unsigned char layer;
	unsigned char unused[7];
} EnvVertex;
#pragma pack(pop)

EnvVertex CreateEnvVertex( glm::vec2 pos, glm::vec2 texcoords, unsigned char layer );

class CEnvironment
{
private:
	GLuint m_vaoId;
	GLuint m_vboId, m_iboId;

	CTexture2D *m_pMountainTexture;

	EnvVertex *m_pVertices;
public:
	CEnvironment();
	~CEnvironment();

	bool initialize();
	void destroy();

	void draw( unsigned char worldLayer, glm::mat4 orthoMatrix );
};