
uniform float time;
uniform float power;

varying vec4 vColor;

float rand( vec2 co ){
    return fract( sin( dot( co.xy ,vec2( 12.9898,78.233 ) ) ) * 43758.5453 );
}

void main()
{
	float noise			= rand( gl_FragCoord.xy + vec2( time, time ) ) * vColor.r;
	gl_FragColor.rgb	= vec3( noise );
	gl_FragColor.a		= 1.0;
}