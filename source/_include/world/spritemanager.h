#pragma once

#define BATCH_CHUNK_SIZE 4096

#pragma pack(push, 16)
typedef struct {
	glm::vec2 pos;
	float rot;
	glm::vec2 size;
	glm::lowp_uvec4 texcoords;
	unsigned char tileInfo[4];
} SpriteVertex;
#pragma pack(pop)

struct SpriteData
{
	glm::vec2 position;
	float rotation;
	glm::vec2 size;
	glm::lowp_uvec4 texcoords;
	unsigned char layer;
	unsigned char tileInfo[2];
};

struct SpriteBatch
{
	GLuint vao;
	GLuint vertex_vbo;
	int chunk_count;

	std::vector<SpriteData> sprites;
};

SpriteVertex CopySpriteVertex( SpriteData& data );

class CSpriteManager
{
private:
	std::vector<SpriteBatch> m_batches;
public:
	CSpriteManager();
	~CSpriteManager();

	bool initialize();
	void destroy();

	void drawSprite( int batchId, SpriteData spriteData );

	int createBatch();

	void update( double deltaT );
	void draw( int batchId );

	int getSpriteCount( int batchId );
};