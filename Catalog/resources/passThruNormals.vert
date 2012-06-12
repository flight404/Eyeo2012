
varying vec4 vVertex;
varying vec3 vNormal;
void main()
{
	vNormal			= gl_Normal;
	vVertex			= gl_Vertex;
	gl_TexCoord[0]	= gl_MultiTexCoord0;
	gl_Position		= ftransform();
}