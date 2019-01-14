#version 150 

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec2 gSize[];
in vec4 gTexcoords[];
in mat4 gModelMat[];
in uint gTileInfo[][2];

out vec2 vRelTexCoord;
out vec2 vTileDimensions;
out vec2 vTileStart;

uniform mat4 MVPMatrix;

void main()
{
	vec2 tileInfoDecode = vec2( gTileInfo[0][0], gTileInfo[0][1] );

	gl_Position = MVPMatrix * gModelMat[0] * vec4( -gSize[0].x, -gSize[0].y, 0, 1 );
	vRelTexCoord = vec2( 0.0f, 1.0f ) * tileInfoDecode;
    EmitVertex();
	
    gl_Position = MVPMatrix * gModelMat[0] * vec4( gSize[0].x, -gSize[0].y, 0, 1 );
	vRelTexCoord = vec2( 1.0f, 1.0f ) * tileInfoDecode;
    EmitVertex();
	
	// Provoking index
	gl_Position = MVPMatrix * gModelMat[0] * vec4( -gSize[0].x, gSize[0].y, 0, 1 );
	vRelTexCoord = vec2( 0.0f, 0.0f ) * tileInfoDecode;
	vTileStart = gTexcoords[0].wx;
	vTileDimensions = gTexcoords[0].yz;
    EmitVertex();
	
	gl_Position = MVPMatrix * gModelMat[0] * vec4( gSize[0].x, gSize[0].y, 0, 1 );
	vRelTexCoord = vec2( 1.0f, 0.0f ) * tileInfoDecode;
	EmitVertex();
	
    EndPrimitive();
}