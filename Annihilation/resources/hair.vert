uniform vec3 eyePos;
uniform mat4 mvpMatrix;

varying vec4 vVertex;
varying vec3 vNormal;
varying vec3 vEyeDir;

void main()
{
	vVertex			= vec4( gl_Vertex );
	vNormal			= gl_Normal;
	
	vEyeDir			= normalize( eyePos - vVertex.xyz );

	gl_Position		= mvpMatrix * vVertex;
	gl_TexCoord[0]	= gl_MultiTexCoord0;
}


