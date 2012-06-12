uniform float mainPower;

varying vec3 eyeDir;
varying vec4 vVertex;
varying vec3 vNormal;
varying vec4 vColor;
varying float vVertexLength;

void main()
{
	gl_FragColor.rgb	= vec3( mainPower );
	gl_FragColor.a		= 1.0;
}