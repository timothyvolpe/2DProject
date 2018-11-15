#include "base.h"
#include "world\spritemanager.h"


SpriteVertex CopySpriteData( SpriteData& data ) {
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
		glDeleteBuffers( 1, &(*it).vao );
		glDeleteVertexArrays( 1, &(*it).vertex_vbo );
	}
	m_batches.clear();
}

int CSpriteManager::createBatch()
{
	GLuint batchVao, batchVertexVbo;

	// Create the arrays
	StartGLDebug( "CreateSpriteBatch" );

	// Create it
	glGenVertexArrays( 1, &batchVao );
	glGenBuffers( 1, &batchVertexVbo );
	
	glBindVertexArray( batchVao );
	// Vertex 
	glBindBuffer( GL_ARRAY_BUFFER, batchVertexVbo );
	// Reallocate the new buffer of the proper size
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( SpriteVertex ), (GLvoid*)offsetof( SpriteVertex, pos ) );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof( SpriteVertex ), (GLvoid*)offsetof( SpriteVertex, size ) );
	glVertexAttribIPointer( 2, 1, GL_UNSIGNED_BYTE, sizeof( SpriteVertex ), (GLvoid*)offsetof( SpriteVertex, layer ) );
	glVertexAttribPointer( 3, 4, GL_UNSIGNED_SHORT, GL_TRUE, sizeof( SpriteVertex ), (GLvoid*)offsetof( SpriteVertex, texcoords ) );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glEnableVertexAttribArray( 2 );
	glEnableVertexAttribArray( 3 );
	glBufferData( GL_ARRAY_BUFFER, BATCH_CHUNK_SIZE*sizeof( SpriteVertex ), 0, GL_DYNAMIC_DRAW );

	EndGLDebug();

	SpriteBatch batch;
	batch.vao = batchVao;
	batch.vertex_vbo = batchVertexVbo;
	batch.invalid_count = 0;
	batch.vertex_count = 0;
	batch.chunk_count = 1;

	m_batches.push_back( batch );

	return m_batches.size()-1;
}

void CSpriteManager::drawSprite( int batchId, SpriteData spriteData )
{
	spriteData.invalid = false;
	m_batches[batchId].sprites.push_back( spriteData );
}

void CSpriteManager::update( double deltaT )
{
}

void CSpriteManager::draw()
{
	StartGLDebug( "DrawSprites" );

	// Flush and draw each batch
	for( auto it = m_batches.begin(); it != m_batches.end(); it++ )
	{
		// Map the buffer
		glBindVertexArray( (*it).vao );
		SpriteVertex* pVerticesPtr = (SpriteVertex*)glMapBufferRange( GL_ARRAY_BUFFER, 0, BATCH_CHUNK_SIZE*sizeof( SpriteVertex ), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );
		if( !pVerticesPtr ) {
			PrintWarn( L"Draw error\n" );
			return;
			EndGLDebug();
		}

		// Form the vertex array
		for( size_t i = 0; i < (*it).sprites.size(); i++ )
		{
			// If it's invalid skip it
			if( (*it).sprites[i].invalid )
				continue;
			pVerticesPtr[i] = CopySpriteData( (*it).sprites[i] );
		}

		glUnmapBuffer( GL_ARRAY_BUFFER );
		pVerticesPtr = 0;

		(*it).vertex_count = (*it).sprites.size();

		glDrawArrays( GL_POINTS, 0, (*it).vertex_count );

		(*it).sprites.clear();
	}

	EndGLDebug();
}

int CSpriteManager::getSpriteCount( int batchId )
{
	assert( batchId < (int)m_batches.size() );
	return m_batches[batchId].sprites.size();
}