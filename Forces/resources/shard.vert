#version 120
uniform vec3 eyePos;
uniform mat4 matrix;
uniform mat4 mvpMatrix;

varying vec4 vVertex;
varying vec3 vNormal;
varying vec4 vColor;
varying vec3 vEyeDir;
varying vec3 vLightPos;
varying vec3 vLightDir;

void main()
{
	gl_TexCoord[0]	= gl_MultiTexCoord0;
	
	vVertex			= matrix * gl_Vertex;
	vNormal			= normalize( vec3( matrix * vec4( gl_Normal, 0.0 ) ) );
	vColor			= gl_Color;
	vEyeDir			= normalize( eyePos - vVertex.xyz );
	vLightPos		= vec3( 0.0, 500.0, 0.0 );
	vLightDir		= vLightPos - vVertex.xyz;
	
	gl_Position		= mvpMatrix * vVertex;
}
