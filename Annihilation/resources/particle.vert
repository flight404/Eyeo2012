uniform vec3 eyePos;
uniform vec3 pos;
uniform float radius;
uniform mat4 mvpMatrix;

varying vec3 eyeDir;
varying vec4 vVertex;
varying vec3 vNormal;
varying float vVertexLength;
varying vec4 vColor;

void main()
{
	vVertex			= vec4( gl_Vertex );
	vVertex.xyz		*= radius;
	vVertex.xyz		+= pos;
	
	vVertexLength	= length( vVertex );
	vNormal			= gl_Normal;//normalize( vec3( mMatrix * vec4( gl_Normal, 0.0 ) ) );
	vColor			= gl_Color;
	
	eyeDir			= normalize( eyePos - vVertex.xyz );
	
	gl_Position		= mvpMatrix * vVertex;
	gl_TexCoord[0]	= gl_MultiTexCoord0;
}


