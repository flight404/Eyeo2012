uniform vec3 color;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;


void main()
{	
	vec3 greenGlow		= vec3( 0.1, 0.4, 0.3 );
	
	gl_FragColor.rgb	= greenGlow * color.r;
	gl_FragColor.a		= 1.0;
}