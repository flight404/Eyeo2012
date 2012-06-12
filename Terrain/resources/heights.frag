#version 110
uniform sampler2D reactionTex;
uniform sampler2D heightTex;

void main()
{
	vec3 initSample		= texture2D( heightTex, gl_TexCoord[0].st ).rgb;
	vec4 reactionSample = texture2D( reactionTex, gl_TexCoord[0].st ).rgba;
	
	float newHeight		= initSample.b + ( reactionSample.r * 1.20 + reactionSample.g * 0.25 );
	newHeight -= newHeight * 0.1;
	
	vec3 finalNormal	= vec3( initSample.r, initSample.g, newHeight );
	gl_FragColor.rgb	= finalNormal;
	gl_FragColor.a		= reactionSample.a;
}

