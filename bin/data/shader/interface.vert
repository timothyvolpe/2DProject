#version 130

in ivec3 in_pos;

uniform mat4 MVPMatrix;
uniform float layerSize;

void main()
{
	gl_Position = MVPMatrix * vec4( in_pos.x, in_pos.y, -(255.0f - in_pos.z)*layerSize, 1.0f );
	gl_FrontColor = vec4( 1.0f, 0.0f, 0.0f, 1.0f );
}