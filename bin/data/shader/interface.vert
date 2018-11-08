#version 150 

in ivec2 in_pos;
in vec2 in_tex;
in uint textureId;
out vec4 qColor;

uniform mat4 MVPMatrix;
uniform float layerSize;

void main()
{
	// 200 is interface layer
	gl_Position = MVPMatrix * vec4( in_pos.x, in_pos.y, -(255.0f - 200)*layerSize, 1.0f );
	if( textureId == 0 )
		qColor = vec4( 1.0f, 1.0f, 0.0f, 1.0f );
	else if( textureId == 1 )
		qColor = vec4( 1.0f, 0.0f, 0.0f, 1.0f );
}