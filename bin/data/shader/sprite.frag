#version 150 

uniform sampler2D tex2dsampler;

in vec4 vColor;
in vec2 vTexCoord;

void main()
{
	gl_FragColor = vColor;
	//gl_FragColor = texture2D( tex2dsampler, vTexCoord );
}