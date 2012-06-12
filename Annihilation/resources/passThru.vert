#version 120

varying vec3 vVertex;

void main()
{
	vVertex			= gl_Vertex.xyz;
	gl_TexCoord[0]	= gl_MultiTexCoord0;
	gl_Position		= ftransform();
}
