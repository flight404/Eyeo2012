uniform sampler2D tex;
uniform vec2 windowSize;
void main()
{
	vec2 offset			= ( gl_FragCoord.xy/windowSize - vec2( 0.5 ) ) * 0.005;
	float rChannel		= texture2D( tex, gl_TexCoord[0].st ).r;
	float gChannel		= texture2D( tex, gl_TexCoord[0].st + offset ).g;
	float bChannel		= texture2D( tex, gl_TexCoord[0].st + offset * 2.0 ).b;
	vec3 finalCol		= vec3( rChannel, gChannel, bChannel );
	
	gl_FragColor.rgb	= finalCol;
	gl_FragColor.a		= 1.0;
}