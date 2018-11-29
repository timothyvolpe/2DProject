#version 150

in vec2 in_pos;
in float in_rotation;
in vec2 in_size;
in vec4 in_texcoords;
//in uint in_layer;
in uvec3 in_tileinfo;

out vec2 gSize;
out vec4 gTexcoords;
out mat4 gModelMat;
out uint gTileInfo[2];

uniform float layerSize = 0.1f;

void main()
{
	gl_Position = vec4( in_pos, -(255.0f - in_tileinfo.x)*layerSize, 1.0f );
	
	// Create a rotation matrix
	vec3 axis = vec3( 0.0, 0.0, 1.0 );
	mat4 rotmat = mat4(
		axis.x*axis.x * (1.0f - cos(in_rotation)) + cos(in_rotation),       // column 1 of row 1
		axis.x*axis.y * (1.0f - cos(in_rotation)) + axis.z * sin(in_rotation), // column 2 of row 1
		axis.x*axis.z * (1.0f - cos(in_rotation)) - axis.y * sin(in_rotation), // column 3 of row 1
		0.0f,

		axis.y*axis.x * (1.0f - cos(in_rotation)) - axis.z * sin(in_rotation), // column 1 of row 2
		axis.y*axis.y * (1.0f - cos(in_rotation)) + cos(in_rotation),       // ...
		axis.y*axis.z * (1.0f - cos(in_rotation)) + axis.x * sin(in_rotation), // ...
		0.0f,

		axis.z*axis.x * (1.0f - cos(in_rotation)) + axis.y * sin(in_rotation), // column 1 of row 3
		axis.z*axis.y * (1.0f - cos(in_rotation)) - axis.x * sin(in_rotation), // ...
		axis.z*axis.z * (1.0f - cos(in_rotation)) + cos(in_rotation),        // ...
		0.0f,
		
		0.0f,
		0.0f,
		0.0f,
		1.0f
	);
	
	gModelMat = mat4(
					1.0f,
					0.0f,
					0.0f,
					0.0f,
					
					0.0f,
					1.0f,
					0.0f,
					0.0f,
					
					0.0f,
					0.0f,
					1.0f,
					0.0f,
					
					gl_Position.xyz,
					1.0f
				) * rotmat;
	
	
	gSize = in_size / 2;
	gTexcoords.wxyz = in_texcoords;
	gTileInfo[0] = in_tileinfo.y;
	gTileInfo[1] = in_tileinfo.z;
}