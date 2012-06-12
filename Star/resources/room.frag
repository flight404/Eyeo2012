uniform sampler2D spectrumTex;
uniform float color;
uniform vec4 starPos;
uniform vec3 eyePos;
uniform float mainPower;
uniform float lightPower;
uniform vec3 roomDims;
uniform float lightIntensity;
uniform float timePer;
//uniform float numLights;
//uniform float invNumLights;
//uniform float invNumLightsHalf;
//uniform sampler2D lightsTex;
uniform float att;

varying vec3 eyeDir;
varying vec4 vVertex;
varying vec3 vNormal;

// http://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
vec3 PointLight( vec3 vertex, vec3 normal, vec3 lightCenter, float lightRadius, vec3 lightColor, float cutoff )
{
    vec3 lightDir	= lightCenter - vertex;
	float distance	= length( lightDir );
//	float sinDistance = ( sin( distance * 0.5 ) * 0.5 + 0.5 ) * 0.9 + 0.1;
//	distance *= sinDistance;
	distance *= distance;// * distance;
//	distance *= 0.0025;
    float d			= max( distance - lightRadius, 0.0 );
    lightDir		/= distance;
    
    // calculate basic attenuation
    float denom = d/lightRadius + 1.0;
    float attenuation = 1.0 / ( denom*denom );
    
    // scale and bias attenuation such that:
    //   attenuation == 0 at extent of max influence
    //   attenuation == 1 when d == 0
    attenuation = ( attenuation - cutoff ) / ( 1.0 - cutoff );
    attenuation = max( attenuation, 0.0 );
    
    float dot = max( dot( lightDir, normal ), 0.0 );
    return lightColor * dot * attenuation;
}



void main()
{
//	vec3 starLighting		= vec3( 0.0 );
//	float index = invNumLightsHalf;
//	for( float i=0.0; i<numLights; i+=1.0 ){
//		vec3 pos		= texture2D( lightsTex, vec2( index, 0.25 ) ).rgb;
//		vec3 color		= texture2D( lightsTex, vec2( index, 0.75 ) ).rgb;
//		float radius	= 0.0;
//		starLighting	+= PointLight( vVertex.xyz, vNormal, pos, radius, color, att );
//		index			+= invNumLights;
//	}

	vec3 spectrumCol	= texture2D( spectrumTex, vec2( color, 0.25 ) ).rgb;
	vec3 starLighting	= PointLight( vVertex.xyz, vNormal, starPos.xyz, starPos.w, spectrumCol, att );
	
	float aoLight		= 1.0 - length( vVertex.xyz ) * 0.0015;
	float aoDark		= aoLight * 0.1;
	
	float ceiling		= 0.0;
	if( vNormal.y < -0.5 ) ceiling = 1.0;
	
	float yPer = clamp( vVertex.y/roomDims.y, 0.0, 1.0 );
	float ceilingGlow	= pow( yPer, 2.0 ) * 0.25;
	ceilingGlow			+= pow( yPer, 200.0 );
	ceilingGlow			+= pow( max( yPer - 0.7, 0.0 ), 3.0 ) * 4.0;
	
	vec3 litRoomColor	= vec3( aoLight + ( ceiling + ceilingGlow * timePer ) * lightPower );
	vec3 darkRoomColor	= vec3( starLighting * color * lightIntensity );
	
	gl_FragColor.rgb	= mix( litRoomColor, darkRoomColor, mainPower );
	gl_FragColor.a		= 1.0;
}