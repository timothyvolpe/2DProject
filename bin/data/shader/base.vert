#version 120

uniform mat4 MVPMatrix;

void main()
{
    gl_Position = MVPMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}