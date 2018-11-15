#version 150 

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec2 gSize[];
in vec4 gTexcoords[];

out vec4 vColor;
out vec2 vTexCoord;

uniform mat4 MVPMatrix;

void main()
{
	gl_Position = MVPMatrix * (gl_in[0].gl_Position + vec4( -gSize[0].x, -gSize[0].y, 0, 0 ));
	vTexCoord = vec2( gTexcoords[0].w, gTexcoords[0].x+gTexcoords[0].z );
    EmitVertex();
    gl_Position = MVPMatrix * (gl_in[0].gl_Position + vec4( gSize[0].x, -gSize[0].y, 0, 0 ));
	vTexCoord = vec2( gTexcoords[0].w+gTexcoords[0].y, gTexcoords[0].x+gTexcoords[0].z );
    EmitVertex();
	gl_Position = MVPMatrix * (gl_in[0].gl_Position + vec4( -gSize[0].x, gSize[0].y, 0, 0 ));
	vTexCoord = vec2( gTexcoords[0].w, gTexcoords[0].x );
    EmitVertex();
	gl_Position = MVPMatrix * (gl_in[0].gl_Position + vec4( gSize[0].x, gSize[0].y, 0, 0 ));
	vTexCoord = vec2( gTexcoords[0].w+gTexcoords[0].y, gTexcoords[0].x );
    EmitVertex();

    EndPrimitive();
}