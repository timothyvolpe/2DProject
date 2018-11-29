#version 150 

uniform sampler2D tex2dsampler;

in vec2 vTexCoord;
in vec2 vRelTexCoord;
in flat vec2 vTileDimensions;
in flat vec2 vTileStart;

void main()
{
	vec2 newCoord = mod( vRelTexCoord, 1 ) * vTileDimensions + vTileStart;
	gl_FragColor = texture2D( tex2dsampler, newCoord );
}