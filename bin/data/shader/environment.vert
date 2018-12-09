#version 150

in vec2 in_pos;
in vec2 in_texcoords;
in uint in_layer;

out vec2 vTexcoords;

uniform mat4 MVPMatrix;
uniform int drawMode;

uniform float layerSize = 0.1f;

void main()
{
    gl_Position = MVPMatrix * vec4( in_pos, -(255.0f - in_layer)*layerSize, 1.0f );
	vTexcoords = in_texcoords;
	
	//if( drawMode == 0 ) // sky
		//gl_FrontColor = gl_Color;
	//if( drawMode == 1 ) // mountains
		//gl_TexCoord[0] = gl_MultiTexCoord0;
}
