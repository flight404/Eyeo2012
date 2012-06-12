varying vec4 vVertex;

void main()
{
	vVertex			= gl_Vertex;
	
	gl_TexCoord[0]	= gl_MultiTexCoord0;
	gl_Position		= ftransform();
}