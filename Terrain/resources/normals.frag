#version 110
uniform sampler2D tex;

uniform float xOffset;
uniform float yOffset;

void main()
{
	float sCoord		= gl_TexCoord[0].s;
	float tCoord		= gl_TexCoord[0].t;
	
	float h0 = texture2D( tex, vec2( sCoord, tCoord ) ).b; 
	float h1 = texture2D( tex, vec2( sCoord, tCoord + yOffset ) ).b;
	float h2 = texture2D( tex, vec2( sCoord + xOffset, tCoord ) ).b;
	
	vec3 normal = normalize( vec3( h0-h2, h0-h1, 1.0 ) );
	
	gl_FragColor.rgb	= normal;
	gl_FragColor.a		= 1.0;
}