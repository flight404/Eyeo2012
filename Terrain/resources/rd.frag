// Based on a Gray Scott Frag shader from rdex-fluxus
// https://code.google.com/p/rdex-fluxus/source/browse/trunk/reactiondiffusion.frag


#version 120


uniform float kernel[9];
uniform vec2 offset[9];
uniform sampler2D tex; // U := r, V := g, other channels ignored
uniform float ru;          // rate of diffusion of U
uniform float rv;          // rate of diffusion of V
uniform float f;           // some coupling parameter
uniform float k;           // another coupling parameter
uniform float dt;

uniform float wind;
uniform sampler2D normalsTex;
uniform float n;

void main(void)
{
	vec4 texColor	= texture2D( tex, gl_TexCoord[0].st );
	
	vec2 sum		= vec2( 0.0, 0.0 );
	for( int i=0; i<9; i++ ){
		vec2 tmp	= texture2D( tex, gl_TexCoord[0].st + offset[i] ).rg;
		sum			+= tmp * kernel[i];
	}
	
	vec3 normalCol	= texture2D( normalsTex, gl_TexCoord[0].st ).rgb;
	normalCol		= texture2D( normalsTex, gl_TexCoord[0].st + ( normalCol.rg * n ) ).rgb;
	
	
	float u		= texColor.r;
	float v		= texColor.g - ( normalCol.g * 0.0025 ) * wind * dt;//; - ( sin(angle * 3.14159265 * 2.0 ) ) * 0.0001;//
	float uvv	= u * v * v;
	
	float K = k - ( normalCol.r * 0.1 ) ;
	float F = f - ( normalCol.g * 0.1 ) ;
	
	float du = ru * sum.r - uvv + F * (1.0 - u);
	float dv = rv * sum.g + uvv - (F + K) * v;
	
	u += du * dt;
	v += dv * dt;
	
	gl_FragColor = vec4( clamp( u, 0.0, 1.0 ), clamp( v, 0.0, 1.0 ), 0.0, 1.0 );
}
