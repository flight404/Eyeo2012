#version 120

uniform sampler2D fboTex;
uniform sampler2D homeTex;
uniform vec3 homePos;
uniform float homeRadius;
uniform vec3 foodPos;
uniform float foodRadius;
uniform vec2 fboSize;

varying vec4 vVertex;

void main()
{
	vec2 tc				= vec2( gl_TexCoord[0].s, gl_TexCoord[0].t );
	vec3 fboRgb			= texture2D( fboTex, tc ).rgb * 0.985;
	
	vec3 adjVertex		= vec3( vVertex.x, vVertex.y, 0.0 ) - vec3( fboSize.x, fboSize.y, 0.0 ) * 0.5;
	vec3 adjHomePos		= vec3( homePos.x, homePos.z, 0.0 );
	vec3 adjFoodPos		= vec3( -foodPos.x, foodPos.z, 0.0 );
	
	vec2 dirToHome		= adjVertex.xy - adjHomePos.xy;
	dirToHome *= 0.0025 + vec2( 0.5, 0.5 );
	
	vec2 dirToFood		= adjVertex.xy - adjFoodPos.xy;
	dirToFood *= 0.0025 + vec2( 0.5, 0.5 );
	
	

	// STRONG HOME FORCE
	float blueHomeVal	= clamp( 1.0 - length( dirToHome ) * 0.01, 0.0, 1.0 ) ;
	
	// SEARCHING
	float homeVal		= texture2D( homeTex, dirToHome * 0.005 + vec2( 0.5 ) ).r;

	// FOOD
	float foodVal		= texture2D( homeTex, dirToFood * 0.015 + vec2( 0.5 ) ).g;
	
	gl_FragColor.rgb	= vec3( max( homeVal, fboRgb.r ), max( foodVal, fboRgb.g ), blueHomeVal );
	gl_FragColor.a		= 1.0;
}
