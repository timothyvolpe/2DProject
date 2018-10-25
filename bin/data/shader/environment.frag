#version 120

uniform vec2 screenResolution;
uniform int drawMode;

uniform sampler2D tex2dsampler;

void main()
{
	if( drawMode == 0 ) // sky
	{
		vec2 position = gl_FragCoord.xy / screenResolution.xy;
		vec3 color;
		
		//color = mix( vec3( 1.0f, 1.0f, 1.0f ), vec3( 0.0f, 0.0f, 0.0f ), position.y );
		color = vec3( 0.9f, 0.9f, 0.9f ) * exp( -position.y*9.0f ); // white
		color += vec3( 0.1f * exp( -8.0f*position.y ), 0.2f, 1.0f - exp( -0.69f*position.y ) );
		//color = vec3( 1.0f - (exp(20.0f*position.y)-1), 1.0f - (exp(2.0f*position.y)-1), 0.8f * (exp(position.y)) );
		
		gl_FragColor = vec4( color, 1.0f );
		//gl_FragColor = gl_Color;
	}
	else if( drawMode == 1 ) // mountains
	{
		gl_FragColor = texture2D( tex2dsampler, gl_TexCoord[0].st );
	}
}