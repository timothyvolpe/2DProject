#version 150 

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec2 qSize[];
in vec4 qTexCoords[];
in int qTextureId[];
in vec3 qColor[];

out vec2 vTexCoords;
out vec3 vColor;
out int vTextureId;

uniform mat4 MVPMatrix;

void main()
{
	vColor = qColor[0];
	vTextureId = qTextureId[0];
	vTexCoords = vec2( qTexCoords[0].xy ) + vec2( 0, qTexCoords[0].w );
	gl_Position = MVPMatrix * ( gl_in[0].gl_Position + vec4( 0, qSize[0].y, 0, 0 ) );
    EmitVertex();
	
	vColor = qColor[0];
	vTextureId = qTextureId[0];
	vTexCoords = vec2( qTexCoords[0].xy + qTexCoords[0].zw );
    gl_Position = MVPMatrix * ( gl_in[0].gl_Position + vec4( qSize[0].xy, 0, 0 ) );
    EmitVertex();
	
	vColor = qColor[0];
	vTextureId = qTextureId[0];
	vTexCoords = vec2( qTexCoords[0].xy );
	gl_Position = MVPMatrix * ( gl_in[0].gl_Position );
    EmitVertex();
	
	vColor = qColor[0];
	vTextureId = qTextureId[0];
	vTexCoords = vec2( qTexCoords[0].xy ) + vec2( qTexCoords[0].z, 0 );
	gl_Position = MVPMatrix * ( gl_in[0].gl_Position + vec4( qSize[0].x, 0, 0, 0 ) );
    EmitVertex();

    EndPrimitive();
}