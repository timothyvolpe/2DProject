#include "base.h"
#include "world\spritemanager.h"


SpriteVertex CopySpriteVertex( SpriteData& data ) {
	SpriteVertex vertex;
	vertex.pos = data.position;
	vertex.size = data.size;
	vertex.layer = data.layer;
	vertex.texcoords = data.texcoords;
	return vertex;
}
CSpriteManager::CSpriteManager() {
}
CSpriteManager::~CSpriteManager() {
}

bool CSpriteManager::initialize()
{
	return true;
}
void CSpriteManager::destroy()
{
	// Delete the batches
	for( auto it = m_batches.begin(); it != m_batches.end(); it++ ) {
		glDeleteVertexArrays( 1, &(*it).vao );
		glDeleteBuffers( 1, &(*it).vertex_vbo );
	}
	m_batches.clear();
}

int CSpriteManager::createBatch()
{
	GLuint batchVao, batchVbo;

	// Create the arrays
	StartGLDebug( "CreateSpriteBatch" );

	// Create it
	glGenVertexArrays( 1, &batchVao );
	glGenBuffers( 1, &batchVbo );

	glBindVertexArray( batchVao );
	// Vertex 
	glBindBuffer( GL_ARRAY_BUFFER, batchVbo );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glEnableVertexAttribArray( 2 );
	glEnableVertexAttribArray( 3 );
	glVertexAttribPointer(	0, 2, GL_FLOAT,				GL_FALSE,	sizeof( SpriteVertex ), (GLvoid*)offsetof( SpriteVertex, pos ) );
	glVertexAttribPointer(	1, 2, GL_FLOAT,				GL_FALSE,	sizeof( SpriteVertex ), (GLvoid*)offsetof( SpriteVertex, size ) );
	glVertexAttribPointer(	2, 4, GL_UNSIGNED_SHORT,	GL_TRUE,	sizeof( SpriteVertex ), (GLvoid*)offsetof( SpriteVertex, texcoords ) );
	glVertexAttribIPointer( 3, 1, GL_UNSIGNED_BYTE,					sizeof( SpriteVertex ), (GLvoid*)offsetof( SpriteVertex, layer ) );
	glBufferData( GL_ARRAY_BUFFER, BATCH_CHUNK_SIZE*sizeof( SpriteVertex ), 0, GL_DYNAMIC_DRAW );

	EndGLDebug();

	SpriteBatch batch;
	batch.vao = batchVao;
	batch.vertex_vbo = batchVbo;
	batch.chunk_count = 1;

	m_batches.push_back( batch );

	return m_batches.size()-1;
}

void CSpriteManager::drawSprite( int batchId, SpriteData spriteData ) {
	m_batches[batchId].sprites.push_back( spriteData );
}

void CSpriteManager::update( double deltaT )
{
}

void CSpriteManager::draw()
{
	SpriteVertex* pVerticesPtr;

	StartGLDebug( "DrawSprites" );

	// Flush and draw each batch
	for( auto it = m_batches.begin(); it != m_batches.end(); it++ )
	{
		// Map the pos buffer
		glBindVertexArray( (*it).vao );
		pVerticesPtr = (SpriteVertex*)glMapBufferRange( GL_ARRAY_BUFFER, 0, BATCH_CHUNK_SIZE*sizeof( SpriteVertex ), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );
		if( !pVerticesPtr ) {
			PrintWarn( L"Draw error\n" );
			return;
			EndGLDebug();
		}

		// Form the vertex array
		for( size_t i = 0; i < (*it).sprites.size(); i++ ) {
			pVerticesPtr[i] = CopySpriteVertex( (*it).sprites[i] );
		}

		glUnmapBuffer( GL_ARRAY_BUFFER );
		pVerticesPtr = 0;

		glDrawArrays( GL_POINTS, 0, (*it).sprites.size() );

		(*it).sprites.clear();
	}

	EndGLDebug();
}

int CSpriteManager::getSpriteCount( int batchId )
{
	assert( batchId < (int)m_batches.size() );
	return m_batches[batchId].sprites.size();
}