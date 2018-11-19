#pragma once

#include <Box2D\Box2D.h>

#pragma pack(push, 4)
typedef struct {
	glm::vec2 pos;
	glm::vec3 color;
} DebugVertex;
#pragma pack(pop)

class CPhysicsDebugDraw : public b2Draw
{
private:
	GLuint m_vaoId;
	GLuint m_vboId;

	DebugVertex* m_pVertexStorage;
public:
	CPhysicsDebugDraw();
	~CPhysicsDebugDraw();

	void DrawPolygon( const b2Vec2* vertices, int32 vertexCount, const b2Color& color );
	void DrawSolidPolygon( const b2Vec2* vertices, int32 vertexCount, const b2Color& color );
	void DrawCircle( const b2Vec2& center, float32 radius, const b2Color& color );
	void DrawSolidCircle( const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color );
	void DrawSegment( const b2Vec2& p1, const b2Vec2& p2, const b2Color& color );
	void DrawTransform( const b2Transform& xf );
};