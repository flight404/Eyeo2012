uniform sampler2D coronaTex;
uniform sampler2D spectrumTex;
uniform float starColor;
uniform float power;

varying vec4 vVertex;

void main()
{
	vec3 spectrumCol	= texture2D( spectrumTex, vec2( starColor, 0.25 ) ).rgb;
	vec4 coronaCol		= texture2D( coronaTex, gl_TexCoord[0].st );

	
	float coronaAlpha	= coronaCol.a;
	
	vec3 offColor		= vec3( 0.0 );
	vec3 onColor		= vec3( spectrumCol );
	
	gl_FragColor.rgb	= mix( offColor, onColor, power );
	gl_FragColor.a		= coronaAlpha;
}




