#version 150

uniform sampler2D tex2dsampler;

in vec4 vColor;

void main()
{
	gl_FragColor = texture2D( tex2dsampler, gl_TexCoord[0].st );
}