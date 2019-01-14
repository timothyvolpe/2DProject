#version 150 

in ivec2 in_pos;
in ivec2 in_size;
in vec4 in_tex;
in int in_textureId;
in vec3 in_color;

out vec4 qTexCoords;
out vec2 qSize;
out int qTextureId;
out vec3 qColor;

uniform float layerSize = 0.1f;
uniform int interfaceLayer = 200;

void main()
{
	// 200 is interface layer
	gl_Position = vec4( in_pos.x, in_pos.y, -(255.0f - interfaceLayer)*layerSize, 1.0f );
	qSize = in_size;
	qTexCoords = in_tex;
	qTextureId = in_textureId;
	qColor = in_color;
}