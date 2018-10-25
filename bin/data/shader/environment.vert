#version 120

uniform mat4 MVPMatrix;
uniform int drawMode;

void main()
{
    gl_Position = MVPMatrix * gl_Vertex;
	//if( drawMode == 0 ) // sky
		//gl_FrontColor = gl_Color;
	if( drawMode == 1 ) // mountains
		gl_TexCoord[0] = gl_MultiTexCoord0;
}