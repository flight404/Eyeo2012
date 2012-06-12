
uniform vec3 pos;
uniform float radius;
uniform mat4 mvpMatrix;

varying vec4 vVertex;
varying vec3 vNormal;

void main()
{
	vVertex			= vec4( gl_Vertex );
	vVertex.xyz		*= radius;
	vVertex.xyz		+= pos;

	vNormal			= gl_Normal;

	gl_Position		= mvpMatrix * vVertex;
	gl_TexCoord[0]	= gl_MultiTexCoord0;
}


