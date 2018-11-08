#version 150 

uniform sampler2D tex2dsampler;

in vec4 vColor;

void main()
{
	//gl_FragColor = texture2D( tex2dsampler, gl_TexCoord[0].st );
	gl_FragColor = vColor;
	//gl_FragColor = vec4( 1.0f, 0.0f, 0.0f, 1.0f );
}