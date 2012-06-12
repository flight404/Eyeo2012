uniform sampler2D prevPosition;
uniform sampler2D currentPosition;
uniform sampler2D currentVelocity;

varying vec4 vVertex;
varying vec3 vNormal;
//varying float leadership;
//varying float crowd;
varying vec3 vLightDir;

void main()
{
	gl_TexCoord[0]	= gl_MultiTexCoord0;
	vVertex			= vec4( gl_Vertex );
	vNormal			= vec3( gl_Normal );
	
	vec4 prevPos	= texture2D( prevPosition, gl_TexCoord[0].st );
	vec4 currentPos	= texture2D( currentPosition, gl_TexCoord[0].st );
	vec4 currentVel	= texture2D( currentVelocity, gl_TexCoord[0].st );
	
	vec3 dir		= normalize( currentPos.xyz - prevPos.xyz + vec3( 0.001 ) );
	vec3 right		= normalize( cross( dir, vec3( 0.0, 1.0, 0.0 ) ) );
	vec3 up			= normalize( cross( dir, right ) );
	
	mat4 m			= mat4( vec4( right, 0.0 ), 
						   vec4( up, 0.0 ), 
						   vec4( dir, 0.0 ), 
						   vec4( 0.0, 0.0, 0.0, 1.0 ) );
	
	
	vec4 vTest		= m * vec4( vVertex.xyz, 0.0 );
	vec4 nTest		= m * vec4( vNormal, 0.0 );
	
	vNormal			= nTest.xyz;	
//	leadership		= currentPos.a;
//	crowd			= min( pow( ( currentVel.a - 2.0 ) * 0.04, 4.0 ), 1.0 ); // * 0.06
	
	vVertex			= vec4( vTest.xyz + currentPos.xyz, gl_Vertex.w );
	//	vVertex.xyz		+= currentPos.xyz;
	
	vec3 lightPos	= vec3( 0.0, 0.0, -200.0 );
	vLightDir		= normalize( lightPos - vVertex.xyz );
	
	gl_Position		= gl_ModelViewProjectionMatrix * vVertex;
}
