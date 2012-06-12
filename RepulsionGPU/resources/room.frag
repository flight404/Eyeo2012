uniform vec3 eyePos;
uniform vec3 roomDims;
uniform float numLights;
uniform float invNumLights;
uniform float invNumLightsHalf;
uniform sampler2D lightsTex;
uniform float att;
uniform float power;
uniform float lightPower;

varying vec3 eyeDir;
varying vec4 vVertex;
varying vec3 vNormal;

// http://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
vec3 PointLight( vec3 vertex, vec3 normal, vec3 eyeDirN, vec3 lightCenter, float lightRadius, vec3 color, float cutoff )
{
    vec3 lightDir	= lightCenter - vertex;
    float distance	= length( lightDir );
	distance *= distance;
    float d			= max( distance - lightRadius, 0.0 );
    lightDir		/= distance;
    vec3 HV			= normalize( normalize( lightDir ) + eyeDirN );
	
    // calculate basic attenuation
//    float denom = d/lightRadius + 1.0;
	float denom = d + 1.0;
    float attenuation = 1.0 / ( denom*denom );
    
    // scale and bias attenuation such that:
    //   attenuation == 0 at extent of max influence
    //   attenuation == 1 when d == 0
    attenuation		= ( attenuation - cutoff ) / ( 1.0 - cutoff );
    attenuation		= max( attenuation, 0.0 );
    
    float diff		= max( dot( lightDir, normal ), 0.0 );
//	float NdotHV	= max( dot( normal, HV ), 0.0 );
//	float spec		= pow( normalize( NdotHV ), 30.0 );
    return color * diff * 0.025 * attenuation;
}


void main()
{
//	vec3 starLighting		= vec3( 0.0 );
//	float index = invNumLightsHalf;
//	for( float i=0.0; i<numLights; i+=1.0 ){
//		vec3 pos		= texture2D( lightsTex, vec2( index, 0.25 ) ).rgb;
//		vec3 color		= texture2D( lightsTex, vec2( index, 0.75 ) ).rgb;
//		float radius	= 1.0;
//		starLighting	+= PointLight( vVertex.xyz, vNormal, eyeDir, pos, radius, color, att );
//		index			+= invNumLights;
//	}

	float aoLight		= 1.0 - length( vVertex.xyz ) * ( 0.0015 + ( power * 0.005 ) );
	float aoDark		= aoLight * 0.1;
	
	float ceiling		= 0.0;
	if( vNormal.y < -0.5 ) ceiling = 1.0;
	
	float yPer = clamp( vVertex.y/roomDims.y, 0.0, 1.0 );
	float ceilingGlow	= pow( yPer, 2.0 ) * 0.25;
	ceilingGlow			+= pow( yPer, 200.0 );
	ceilingGlow			+= pow( max( yPer - 0.7, 0.0 ), 3.0 ) * 4.0;
	
	vec3 litRoomColor	= vec3( aoLight + ( ceiling + ceilingGlow ) * lightPower );
	vec3 darkRoomColor	= vec3( aoDark );
	
	gl_FragColor.rgb	= mix( litRoomColor, darkRoomColor, power );
	gl_FragColor.a		= 1.0;
}