#version 150

in vec2 in_pos;
in vec2 in_size;
in int layer;
in vec4 in_texcoords;

out vec2 gSize;
out vec4 gTexcoords;

uniform float layerSize = 0.1f;

void main()
{
	gl_Position = vec4( in_pos, -(255.0f - layer)*layerSize, 1.0f );
	
	gSize = in_size / 2;
	gTexcoords.wxyz = in_texcoords;
}