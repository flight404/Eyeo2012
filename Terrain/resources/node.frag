uniform vec3 color;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;
varying vec3 vFog;
varying float vHeight;

void main()
{	
	vec3 greenGlow		= vec3( 0.1, 0.4, 0.3 );
	
	gl_FragColor.rgb	= vec3( clamp( vHeight * 0.075 + 0.5, 0.0, 1.0 ) );
	gl_FragColor.a		= 1.0;
}