#version 120
uniform vec3 eyePos;
uniform mat4 matrix;
uniform vec3 pos;
uniform vec3 radius;
uniform mat4 mvpMatrix;

varying vec4 vVertex;
varying vec3 vNormal;
varying vec4 vColor;
varying vec3 vEyeDir;

void main()
{
	vVertex			= matrix * gl_Vertex;
	vNormal			= normalize( vec3( matrix * vec4( gl_Normal, 0.0 ) ) );
	vColor			= gl_Color;
	vEyeDir			= normalize( eyePos - vVertex.xyz );
	gl_Position		= mvpMatrix * vVertex;
	gl_TexCoord[0]	= gl_MultiTexCoord0;
}