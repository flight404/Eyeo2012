uniform vec3 eyePos;
uniform mat4 matrix;
uniform mat4 mvpMatrix;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;

void main()
{
	vNormal			= normalize( vec3( matrix * vec4( gl_Normal, 0.0 ) ) );
	vVertex			= matrix * vec4( gl_Vertex );

	vEyeDir			= normalize( eyePos - vVertex.xyz );
	
	gl_Position		= mvpMatrix * vVertex;
	gl_TexCoord[0]	= gl_MultiTexCoord0;
}


