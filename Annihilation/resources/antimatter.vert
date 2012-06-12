uniform vec3 eyePos;
uniform mat4 mvpMatrix;
uniform float radius;
uniform vec3 pos;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;

void main()
{
	vVertex			= vec4( gl_Vertex );
	vVertex.xyz		*= radius;
	vVertex.xyz		+= pos;
	
	vNormal			= gl_Normal;
	
	vEyeDir			= normalize( eyePos - vVertex.xyz );
	
	gl_Position		= mvpMatrix * vVertex;
	gl_TexCoord[0]	= gl_MultiTexCoord0;
}