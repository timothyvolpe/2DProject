#version 150 

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec2 geom_size[];
in vec4 geom_texcoords[];

out vec4 vColor;
out vec2 vTexCoord;

uniform mat4 MVPMatrix;

void main()
{
	vColor = vec4( 1.0f, 0.0f, 0.0f, 1.0f );
	gl_Position = MVPMatrix * (gl_in[0].gl_Position + vec4( -geom_size[0].x, -geom_size[0].y, 0, 0 ));
	//vTexCoord = vec2( geom_texcoords[0].w, geom_texcoords[0].x );
    EmitVertex();
	vColor = vec4( 1.0f, 0.0f, 0.0f, 1.0f );
    gl_Position = MVPMatrix * (gl_in[0].gl_Position + vec4( geom_size[0].x, -geom_size[0].y, 0, 0 ));
	//vTexCoord = vec2( geom_texcoords[0].w, geom_texcoords[0].x );
    EmitVertex();
	vColor = vec4( 1.0f, 0.0f, 0.0f, 1.0f );
	gl_Position = MVPMatrix * (gl_in[0].gl_Position + vec4( -geom_size[0].x, geom_size[0].y, 0, 0 ));
	//vTexCoord = vec2( geom_texcoords[0].w, geom_texcoords[0].x );
    EmitVertex();
	vColor = vec4( 1.0f, 0.0f, 0.0f, 1.0f );
	gl_Position = MVPMatrix * (gl_in[0].gl_Position + vec4( geom_size[0].x, geom_size[0].y, 0, 0 ));
	//vTexCoord = vec2( geom_texcoords[0].w, geom_texcoords[0].x );
    EmitVertex();

    EndPrimitive();
}