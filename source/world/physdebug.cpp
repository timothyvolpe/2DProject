#include "base.h"
#include "world\physdebug.h"

CPhysicsDebugDraw::CPhysicsDebugDraw()
{
}
CPhysicsDebugDraw::~CPhysicsDebugDraw()
{
}

void CPhysicsDebugDraw::DrawPolygon( const b2Vec2* vertices, int32 vertexCount, const b2Color& color )
{
	glBegin( GL_LINE_STRIP );
	{
		for( int i = 0; i < vertexCount; i++ ) {
			glColor3f( color.r, color.g, color.b );
			glVertex2f( vertices[i].x, vertices[i].y );
		}
	}
	glEnd();
}
void CPhysicsDebugDraw::DrawSolidPolygon( const b2Vec2* vertices, int32 vertexCount, const b2Color& color )
{
	/*glBegin( GL_POINTS );
	{
		for( int i = 0; i < vertexCount; i+=6 )
		{
			glColor3f( color.r, color.g, color.b );
			glVertex2f( vertices[i].x, vertices[i].y );

			glColor3f( color.r, color.g, color.b );
			glVertex2f( vertices[i+1].x, vertices[i+1].y );

			glColor3f( color.r, color.g, color.b );
			glVertex2f( vertices[i+2].x, vertices[i+2].y );

			glColor3f( color.r, color.g, color.b );
			glVertex2f( vertices[i+3].x, vertices[i+3].y );

			glColor3f( color.r, color.g, color.b );
			glVertex2f( vertices[i+4].x, vertices[i+4].y );

			glColor3f( color.r, color.g, color.b );
			glVertex2f( vertices[i+5].x, vertices[i+5].y );
		}
	}
	glEnd();*/
	glBegin( GL_LINE_STRIP );
	{
		for( int i = 0; i < vertexCount; i++ ) {
			glColor3f( color.r, color.g, color.b );
			glVertex2f( vertices[i].x, vertices[i].y );
		}
		glColor3f( color.r, color.g, color.b );
		glVertex2f( vertices[0].x, vertices[0].y );
	}
	glEnd();
}
void CPhysicsDebugDraw::DrawCircle( const b2Vec2& center, float32 radius, const b2Color& color )
{
}
void CPhysicsDebugDraw::DrawSolidCircle( const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color )
{
}
void CPhysicsDebugDraw::DrawSegment( const b2Vec2& p1, const b2Vec2& p2, const b2Color& color )
{
	glBegin( GL_LINE );
	{
		glColor3f( color.r, color.g, color.b );
		glVertex2f( p1.x, p1.y );

		glColor3f( color.r, color.g, color.b );
		glVertex2f( p2.x, p2.y );
	}
	glEnd();
}
void CPhysicsDebugDraw::DrawTransform( const b2Transform& xf )
{
}