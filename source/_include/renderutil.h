#pragma once

#include <glm\glm.hpp>

class CRenderUtil
{
public:
	static void drawSprite( glm::vec2 localPosition, glm::vec2 dimensions );
	static void drawSpriteColored( glm::vec2 localPosition, glm::vec2 dimensions, glm::vec3 color );
	static void drawSpriteTextured( glm::vec2 localPosition, glm::vec2 dimensions, glm::vec4 uvbounds );
};