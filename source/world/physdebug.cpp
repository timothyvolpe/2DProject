#include "base.h"
#include "world\physdebug.h"

CPhysicsDebugDraw::CPhysicsDebugDraw()
{
	m_vaoId = 0;
	m_vboId = 0;

	StartGLDebug( "CreateDebugDraw" );

	glGenVertexArrays( 1, &m_vaoId );
	glBindVertexArray( m_vaoId );

	glGenBuffers( 1, &m_vboId );
	glBindBuffer( GL_ARRAY_BUFFER, m_vboId );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( DebugVertex ), (GLvoid*)offsetof( DebugVertex, pos ) );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( DebugVertex ), (GLvoid*)offsetof( DebugVertex, color ) );
	glBufferData( GL_ARRAY_BUFFER, 0, 0, GL_STREAM_DRAW );

	EndGLDebug();

	m_pVertexStorage = 0;
}
CPhysicsDebugDraw::~CPhysicsDebugDraw()
{
	if( m_vboId ) {
		glDeleteBuffers( 1, &m_vboId );
		m_vboId = 0;
	}
	if( m_vaoId ) {
		glDeleteVertexArrays( 1, &m_vaoId );
		m_vaoId = 0;
	}
}

void CPhysicsDebugDraw::DrawPolygon( const b2Vec2* vertices, int32 vertexCount, const b2Color& color )
{
	m_pVertexStorage = new DebugVertex[vertexCount];

	for( int32 i = 0; i < vertexCount; i++ ) {
		m_pVertexStorage[i].pos = glm::vec2( vertices[i].x, vertices[i].y );
		m_pVertexStorage[i].color = glm::vec3( color.r, color.g, color.b );
	}

	glBindVertexArray( m_vaoId );
	glBindBuffer( GL_ARRAY_BUFFER, m_vboId );
	glBufferData( GL_ARRAY_BUFFER, sizeof( DebugVertex )*vertexCount, m_pVertexStorage, GL_STREAM_DRAW );
	glDrawArrays( GL_LINE_LOOP, 0, vertexCount );

	SafeDeleteArray( m_pVertexStorage );
}
void CPhysicsDebugDraw::DrawSolidPolygon( const b2Vec2* vertices, int32 vertexCount, const b2Color& color )
{
	this->DrawPolygon( vertices, vertexCount, color );
}
void CPhysicsDebugDraw::DrawCircle( const b2Vec2& center, float32 radius, const b2Color& color )
{
}
void CPhysicsDebugDraw::DrawSolidCircle( const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color )
{
}
void CPhysicsDebugDraw::DrawSegment( const b2Vec2& p1, const b2Vec2& p2, const b2Color& color )
{
	m_pVertexStorage = new DebugVertex[2];

	m_pVertexStorage[0].pos = glm::vec2( p1.x, p1.y );
	m_pVertexStorage[0].color = glm::vec3( color.r, color.g, color.b );
	m_pVertexStorage[1].pos = glm::vec2( p2.x, p2.y );
	m_pVertexStorage[1].color = glm::vec3( color.r, color.g, color.b );

	glBindVertexArray( m_vaoId );
	glBindBuffer( GL_ARRAY_BUFFER, m_vboId );
	glBufferData( GL_ARRAY_BUFFER, sizeof( DebugVertex )*2, m_pVertexStorage, GL_STREAM_DRAW );
	glDrawArrays( GL_LINE_LOOP, 0, 2 );

	SafeDeleteArray( m_pVertexStorage );
}
void CPhysicsDebugDraw::DrawTransform( const b2Transform& xf )
{
}