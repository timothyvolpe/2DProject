#version 150

uniform mat4 MVPMatrix;

void main()
{
    gl_Position = MVPMatrix * gl_Vertex;
	gl_FrontColor = gl_Color;
}