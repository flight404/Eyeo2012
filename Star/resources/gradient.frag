uniform float radius;
uniform float starRadius;
uniform float power;
uniform float time;
uniform vec3 roomDim;
uniform vec2 randSeed;
uniform int	randIterations;
uniform int stage;
uniform int unit;
uniform sampler2D spectrumTex;
uniform float color;

uniform sampler2D gridTex;

varying vec4 vVertex;

const float PI = 3.14159265;

float rand( vec2 co ){
    return fract( sin( dot( co.xy ,vec2( 12.9898,78.233 ) ) ) * 43758.5453 );
}

float drawCircle( float d, float r, float per )
{
	float rSub		= r * per;
	float invPer	= 1.0 - per;
	
	if( d < rSub )		return 1.0;
	else if( d < r )	return 1.0 - pow( ( d - rSub )/( r * invPer ),0.6 );
	else				return 0.0;
}

void main()
{	
	vec3 spectrumCol	= texture2D( spectrumTex, vec2( color, 0.25 ) ).rgb;
	
	float dist			= length( vVertex.xyz );
	float distPer		= dist/radius;
	float alpha			= 0.0;
	float grey			= gl_FragCoord.y / roomDim.y;
	
	vec3 offColor	= vec3( 0.0 );
	vec3 onColor	= vec3( 0.0 );
	float offAlpha	= 0.0;
	float onAlpha	= 0.0;
	
	if( stage == 1 ){ // draw filled circle
		float c			= drawCircle( dist, radius, 0.98 );

		offColor		= vec3( power + grey * 0.1 );
		onColor			= vec3( spectrumCol );
		
		offAlpha		= c;
		onAlpha			= c;
		
	} else {
		float radiusOffset	= radius;
		
		vec4 gridCol		= texture2D( gridTex, gl_TexCoord[0].st );
		float newStarRadius = starRadius + 2.0;
		
		float alpha = 0.0;
		float brightness = 1.0;
		
		if( unit == 2 ){
			if( dist < newStarRadius ){
				alpha		= drawCircle( dist, newStarRadius * 2.0, 0.98 );
			}
		} else {
			
			if( unit == 1 ){ // MIDDLE PANEL
				newStarRadius	*= 0.8;
				float newRadius	= radius - newStarRadius;
				float newDist	= max( dist - newStarRadius, 0.0 );
				float distPer	= clamp( newDist/newRadius, 0.0, 1.0 );
				alpha			= pow( 1.0 - distPer, 1.0 + rand( randSeed ) * 3.0 ) * 0.5;
				
			} else { // CLOSEST PANEL
				newStarRadius	*= 0.5;
				float newRadius	= radius - newStarRadius;
				float newDist	= max( dist - newStarRadius, 0.0 );
//				float distPer	= clamp( newDist/newRadius, 0.0, 1.0 );
				alpha			= pow( max( 1.0 - distPer, 0.0 ), 1.0 + rand( randSeed ) * 5.0 );
			}

			
		}
		
		offColor	= vec3( gridCol );
		onColor		= vec3( brightness * spectrumCol );
		
//		if( alpha > 0.925 ) onColor *= 1.5;
		
		offAlpha	= clamp( gridCol.a + alpha, 0.0, 1.0 );
		onAlpha		= alpha * 0.75;
	}
	
	gl_FragColor.rgb	= mix( offColor, onColor, power );
	gl_FragColor.a		= mix( offAlpha, onAlpha, power );
}