uniform vec3 eyePos;

varying vec4 vVertex;
varying vec3 vNormal;
varying vec3 vEyeDir;
void main()
{
	vNormal			= gl_Normal;
	vVertex			= gl_Vertex;
	
	vEyeDir			= normalize( eyePos - vVertex.xyz );
	
	gl_TexCoord[0]	= gl_MultiTexCoord0;
	gl_Position		= ftransform();
}