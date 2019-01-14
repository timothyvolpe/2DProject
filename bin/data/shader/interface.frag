#version 150 

uniform sampler2D tex2dsampler;

in flat int vTextureId;
in vec3 vColor;
in vec2 vTexCoords;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

const float smoothing = 1.0/16.0;

void main()
{
	if( vTextureId >= 0 )
	{
		gl_FragColor = texture2D( tex2dsampler, vTexCoords );
		
		/*vec2 flipped_texCoords = vec2( vTexCoords.x, 1.0 - vTexCoords.y );
		vec2 pos = flipped_texCoords;
		vec3 sample = texture2D( tex2dsampler, flipped_texCoords ).rgb;
		ivec2 sz = textureSize( tex2dsampler, 0 ).xy;
		float dx = dFdx( pos.x ) * sz.x; 
		float dy = dFdy( pos.y ) * sz.y;
		float toPixels = 8.0 * inversesqrt( dx * dx + dy * dy );
		float sigDist = median( sample.r, sample.g, sample.b );
		float w = fwidth( sigDist );
		float opacity = smoothstep( 0.5 - w, 0.5 + w, sigDist );

		gl_FragColor = vec4( 1.0f, 0.0f, 0.0f, opacity );*/
		
		/*vec2 msdfUnit = 8.0f / vec2( textureSize(tex2dsampler, 0) );
		vec3 sample = texture( tex2dsampler, vTexCoords ).rgb;
		float sigDist = median( sample.r, sample.g, sample.b ) - 0.5;
		sigDist *= dot( msdfUnit, 0.5/fwidth( vTexCoords ) );
		float opacity = clamp( sigDist + 0.5, 0.0, 1.0 );
		gl_FragColor = mix( vec4( 1.0f, 0.0f, 0.0f, 1.0f ), vec4( 0.0f, 1.0f, 0.0f, 1.0f ), opacity);*/
		
		float distance = texture2D( tex2dsampler, vTexCoords ).r;
		float alpha = smoothstep( 0.5 - smoothing, 0.5 + smoothing, distance );
		gl_FragColor = mix( vec4( 0.0f, 0.0f, 0.0f, 0.0f ), vec4( vColor.rgb, 1.0f ), alpha );
	}
	else
		gl_FragColor = vec4( vColor, 1.0f );
}