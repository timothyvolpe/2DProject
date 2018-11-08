#version 150 

layout(lines_adjacency) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4 qColor[];
out vec4 vColor;

void main()
{
	
	vColor = qColor[0];
	gl_Position = gl_in[0].gl_Position;
    EmitVertex();
	vColor = qColor[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
	vColor = qColor[2];
	gl_Position = gl_in[2].gl_Position;
    EmitVertex();
	vColor = qColor[3];
	gl_Position = gl_in[3].gl_Position;
    EmitVertex();

    EndPrimitive();
}