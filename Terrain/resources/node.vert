uniform sampler2D heightsTex;
uniform vec2 texCoord;
uniform vec3 eyePos;
uniform vec3 fogColor;
uniform mat4 matrix;
uniform mat4 mvpMatrix;
uniform float power;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;
varying vec3 vFog;
varying float vHeight;
varying float vDist;

void main()
{
	gl_TexCoord[0]	= gl_MultiTexCoord0;
	
	vHeight			= texture2D( heightsTex, texCoord ).b;
	
	vNormal			= normalize( gl_Normal );
	vVertex			= matrix * vec4( gl_Vertex );
//	vVertex.y		+= vHeight * 3.0;
	
	float dist		= pow( distance( eyePos, vVertex.xyz * vec3( 1.5, 1.0, 1.5 ) ) * 0.0015, 3.0 );
	vDist			= clamp( 1.0 - dist, 0.0, 1.0 ) * power;
	vFog			= mix( fogColor, vec3( 0.0 ), vDist );
	
	vEyeDir			= normalize( eyePos - vVertex.xyz );
	
	gl_Position		= mvpMatrix * vVertex;
	
}


