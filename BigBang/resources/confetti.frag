uniform vec3 color;

varying vec3 vNormal;


void main()
{
	gl_FragColor.rgb	= color * ( vNormal.y * 0.3 + 0.7 ) * 3.0;
	gl_FragColor.a		= 1.0;
}