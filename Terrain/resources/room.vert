uniform vec3 eyePos;
uniform mat4 mMatrix;
uniform mat4 mvpMatrix;

varying vec3 eyeDir;
varying vec4 vVertex;
varying vec3 vNormal;

void main()
{
	vVertex			= mMatrix * vec4( gl_Vertex );
	vNormal			= gl_Normal;//normalize( vec3( mMatrix * vec4( gl_Normal, 0.0 ) ) );
	
	eyeDir			= normalize( eyePos - vVertex.xyz );
	
	gl_Position		= mvpMatrix * vVertex;
	gl_TexCoord[0]	= gl_MultiTexCoord0;
}


