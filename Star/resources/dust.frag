uniform sampler2D spectrumTex;
uniform float color;
uniform float power;

varying vec4 vColor;

void main()
{
	vec3 col			= texture2D( spectrumTex, vec2( color, 0.25 ) ).rgb;
	
	gl_FragColor.rgb	= mix( vec3( 0.0 ), vec3( col ), power );
	gl_FragColor.a		= vColor.a;
}


