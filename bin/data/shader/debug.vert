#version 150

uniform mat4 MVPMatrix;

in vec2 in_pos;
in vec3 in_color;

out vec4 vColor;

void main()
{
    gl_Position = MVPMatrix * vec4( in_pos, 0.0f, 1.0f );
	vColor = vec4( in_color, 1.0f );
}