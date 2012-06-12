uniform vec3 eyePos;
uniform float power;
uniform vec3 roomDims;
uniform float lightPower;
uniform float timePer;
uniform float numLights;
uniform float invNumLights;
uniform float invNumLightsHalf;
uniform sampler2D lanternsTex;
uniform float att;

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
	//    vec3 HV			= normalize( normalize( lightDir ) + eyeDirN );
	
    // calculate basic attenuation
    float denom = d/lightRadius + 1.0;
//	float denom = d + 1.0;
    float attenuation = 1.0 / ( denom*denom );
    
    // scale and bias attenuation such that:
    //   attenuation == 0 at extent of max influence
    //   attenuation == 1 when d == 0
    attenuation		= ( attenuation - cutoff ) / ( 1.0 - cutoff );
    attenuation		= max( attenuation, 0.0 );
    
    float diff		= max( dot( lightDir, normal ), 0.0 );
	//	float NdotHV	= max( dot( normal, HV ), 0.0 );
	//	float spec		= pow( NdotHV, 131.0 );
    return color * diff * 3.0 * attenuation;// + spec ;
}


void main()
{
	vec3 eyeDir			= eyePos - vVertex.xyz;
	vec3 eyeDirNorm		= normalize( eyeDir );
	
	vec3 baitLighting = vec3( 0.0 );
	float index = invNumLightsHalf;
	for( float i=0.0; i<numLights; i+=1.0 ){
		vec4 pos		= texture2D( lanternsTex, vec2( index, 0.25 ) );
		vec4 color		= texture2D( lanternsTex, vec2( index, 0.75 ) );
		float radius	= pos.a;
		baitLighting    += PointLight( vVertex.xyz, vNormal, eyeDirNorm, pos.rgb, radius, color.rgb, att );
		index			+= invNumLights;
	}
	
	float aoLight		= 1.0 - length( vVertex.xyz ) * 0.0015;
	float aoDark		= aoLight * 0.1;
	
	float ceiling		= 0.0;
	if( vNormal.y < -0.5 ) ceiling = 1.0;
	
	float yPer = vVertex.y/roomDims.y;
	float ceilingGlow	 = pow( clamp( yPer, 0.0, 1.0 ), 2.0 ) * 0.25;
	ceilingGlow			+= pow( clamp( yPer, 0.0, 1.0 ), 200.0 );
	ceilingGlow			+= pow( clamp( yPer - 0.7, 0.0, 1.0 ), 3.0 ) * 4.0;
	
	vec3 litRoomColor	= vec3( aoLight + ( ceiling + ceilingGlow * timePer ) * lightPower );
	vec3 darkRoomColor	= vec3( aoDark + baitLighting );
	
	gl_FragColor.rgb	= mix( litRoomColor, darkRoomColor, power );
//	gl_FragColor.rgb	= texture2D( lightsTex, gl_TexCoord[0].st ).rgb;
	gl_FragColor.a		= 1.0;
}