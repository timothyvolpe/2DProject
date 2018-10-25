#version 120

uniform sampler2D tex2dsampler;

void main()
{
	gl_FragColor = texture2D( tex2dsampler, gl_TexCoord[0].st );
}