#version 120

varying vec3 vVertex;
varying vec4 vColor;
void main()
{
	vVertex			= gl_Vertex.xyz;
	vColor			= gl_Color;
	gl_TexCoord[0]	= gl_MultiTexCoord0;
	gl_Position		= ftransform();
}
