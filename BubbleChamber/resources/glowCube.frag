uniform vec3 eyePos;

varying vec4 vVertex;
varying vec3 vNormal;
varying vec4 vColor;
varying vec3 vEyeDir;

void main()
{
	vec3 lightPos		= vec3( 0.0, 500.0, 0.0 );
	vec3 lightDir		= lightPos - vVertex.xyz;
	
	float ppDiff		= max( dot( vNormal, normalize( lightDir ) ), 0.0 );
	
	gl_FragColor.rgb	= vec3( ppDiff );
	gl_FragColor.a		= 1.0;
}