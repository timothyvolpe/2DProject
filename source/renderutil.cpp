#include "base.h"
#include "renderutil.h"

void CRenderUtil::drawSprite( glm::vec2 localPosition, glm::vec2 dimensions )
{
	glBegin( GL_TRIANGLES );
	{
		glVertex2f( localPosition.x, localPosition.y );
		glVertex2f( localPosition.x+dimensions.x, localPosition.y );
		glVertex2f( localPosition.x, localPosition.y+dimensions.y );
		glVertex2f( localPosition.x, localPosition.y+dimensions.y );
		glVertex2f( localPosition.x+dimensions.x, localPosition.y );
		glVertex2f( localPosition.x+dimensions.x, localPosition.y+dimensions.y );
	}
	glEnd();
}
void CRenderUtil::drawSpriteColored( glm::vec2 localPosition, glm::vec2 dimensions, glm::vec3 color )
{
	glBegin( GL_TRIANGLES );
	{
		glColor3f( color.r, color.g, color.b );
		glVertex2f( localPosition.x, localPosition.y );
		glColor3f( color.r, color.g, color.b );
		glVertex2f( localPosition.x+dimensions.x, localPosition.y );
		glColor3f( color.r, color.g, color.b );
		glVertex2f( localPosition.x, localPosition.y+dimensions.y );
		glColor3f( color.r, color.g, color.b );
		glVertex2f( localPosition.x, localPosition.y+dimensions.y );
		glColor3f( color.r, color.g, color.b );
		glVertex2f( localPosition.x+dimensions.x, localPosition.y );
		glColor3f( color.r, color.g, color.b );
		glVertex2f( localPosition.x+dimensions.x, localPosition.y+dimensions.y );
	}
	glEnd();
}
void CRenderUtil::drawSpriteTextured( glm::vec2 localPosition, glm::vec2 dimensions, glm::vec4 uvbounds )
{
	glBegin( GL_TRIANGLES );
	{
		glTexCoord2f( uvbounds.x, uvbounds.w );
		glVertex2f( localPosition.x, localPosition.y );
		glTexCoord2f( uvbounds.z, uvbounds.w );
		glVertex2f( localPosition.x+dimensions.x, localPosition.y );
		glTexCoord2f( uvbounds.x, uvbounds.y );
		glVertex2f( localPosition.x, localPosition.y+dimensions.y );
		glTexCoord2f( uvbounds.x, uvbounds.y );
		glVertex2f( localPosition.x, localPosition.y+dimensions.y );
		glTexCoord2f( uvbounds.z, uvbounds.w );
		glVertex2f( localPosition.x+dimensions.x, localPosition.y );
		glTexCoord2f( uvbounds.z, uvbounds.y );
		glVertex2f( localPosition.x+dimensions.x, localPosition.y+dimensions.y );
	}
	glEnd();
}