#version 150 

uniform sampler2D tex2dsampler;

in vec2 vTexCoord;

void main()
{
	gl_FragColor = texture2D( tex2dsampler, vTexCoord );
}