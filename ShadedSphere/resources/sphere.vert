uniform vec3 eyePos;
uniform vec3 pos;
uniform float radius;
uniform mat4 mvpMatrix;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;
varying vec4 vColor;

void main()
{
	vVertex			= vec4( gl_Vertex );
	vNormal			= gl_Normal;
	vColor			= gl_Color;
	
	vVertex.xyz		*= radius;
	vEyeDir			= normalize( eyePos - vVertex.xyz );
	
	gl_Position		= mvpMatrix * vVertex;
	gl_TexCoord[0]	= gl_MultiTexCoord0;
}


