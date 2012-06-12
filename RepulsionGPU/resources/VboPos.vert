
uniform sampler2D currentPosition;
uniform sampler2D currentVelocity;
uniform vec3 eyePos;
uniform float power;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;
varying vec3 vColor;
varying vec3 lightDir;
varying float collideAmt;


const float M_4_PI = 12.566370614359172;

float getRadiusFromMass( float m )
{
	float r = pow( (3.0 * m )/M_4_PI, 0.3333333 );
	return r;
}

float getMassFromRadius( float r )
{
	float m = ( ( r * r * r ) * M_4_PI ) * 0.33333;
	return m;
}

void main()
{
	gl_TexCoord[0]	= gl_MultiTexCoord0;
	vVertex			= vec4( gl_Vertex );
	vNormal			= vec3( gl_Normal );
	vColor			= vec3( gl_Color );
	
	vec4 currentPos	= texture2D( currentPosition, vColor.xy );
	vec4 currentVel = texture2D( currentVelocity, vColor.xy );
	collideAmt		= currentVel.a;
	
	float mass		= currentPos.a;
	float radius	= getRadiusFromMass( mass );
	
	vEyeDir			= normalize( eyePos - vVertex.xyz ) * 0.5;
	lightDir		= vec3( 0.0, 1.3, 0.0 ) ;
	
	vVertex			= vec4( vVertex.xyz * radius + currentPos.xyz, gl_Vertex.w );
	gl_Position		= gl_ModelViewProjectionMatrix * vVertex;
}