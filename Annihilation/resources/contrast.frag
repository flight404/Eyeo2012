#version 120

uniform sampler2D tex;

void main()
{
	vec3 col = texture2D( tex, gl_TexCoord[0].st ).rgb;

	gl_FragColor.rgb = vec3( col );//vec3( pow( col, 3.0 ) );
	gl_FragColor.a   = 1.0;
}