
uniform float scale;

varying vec4 vVertex;
varying vec4 vColor;

void main()
{
	gl_TexCoord[0]	= gl_MultiTexCoord0;
	vVertex			= gl_Vertex;
	vColor			= gl_Color;

	vVertex.xyz		*= scale;				// SCALE IMAGE

	gl_Position		= gl_ModelViewProjectionMatrix * vVertex;
}
