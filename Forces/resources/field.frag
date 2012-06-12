uniform vec3 eyePos;
//uniform float radius;
//uniform vec3 roomDim;
uniform float numLights;
uniform float invNumLights;
uniform float invNumLightsHalf;
uniform sampler2D lightsTex;
uniform float att;
//uniform float time;
uniform float power;
uniform samplerCube	cubeMap;

uniform float charge;

varying vec3 vEyeDir;
varying vec3 vLightPos;
varying vec3 vLightDir;
varying vec4 vVertex;
varying vec3 vNormal;
varying vec4 vColor;

//float rand( vec2 co ){
//    return fract( sin( dot( co.xy ,vec2( 12.9898,78.233 ) ) ) * 43758.5453 );
//}
//
vec3 DirectIlluminationWithSpec( vec3 P, vec3 N, vec3 eyeDirN, inout float dist, vec3 lightCenter, float lightRadius, vec3 lightColor, float cutoff, float diffMulti, float specMulti )
{
    // calculate normalized light vector and distance to sphere light surface
    float r			= lightRadius;
    vec3 L			= lightCenter - P;
    dist			= length(L);
	dist			*= dist;
    float d			= max(dist - r, 0.0);
    L /= dist;
    vec3 HV			= normalize( normalize( L ) + eyeDirN );
	
	
    // calculate basic attenuation
    float denom = d/r + 1.0;
    float attenuation = 1.0 / (denom*denom);
    
    // scale and bias attenuation such that:
    //   attenuation == 0 at extent of max influence
    //   attenuation == 1 when d == 0
    attenuation		= (attenuation - cutoff) / (1.0 - cutoff);
    attenuation		= max(attenuation, 0.0);
    
    float diff		= max( dot( L, N ), 0.0 );
	float NdotHV	= max( dot( N, HV ), 0.0 );
	float spec		= pow( NdotHV, 30.0 );
	
    return lightColor * ( spec * specMulti ) * attenuation;
}


void main()
{
	vec3 R				= reflect( vEyeDir, vNormal );
	vec3 envColor		= textureCube( cubeMap, R ).rgb;
	float envGrey		= envColor.r;
	float envSpec		= envColor.g;
	
	float ppDiff		= max( dot( vNormal, normalize( vLightDir ) ), 0.0 );
	vec3 HV				= normalize( vEyeDir + vLightDir );
	float NdotHV		= max( dot( vNormal, HV ), 0.0 );
	float ppSpec		= pow( NdotHV, 50.0 );
	
	float ppFres		= pow( 1.0 - ppDiff, 2.5 );
	float ppEyeDiff		= max( dot( vNormal, vEyeDir ), 0.05 );
	float ppEyeFres		= pow( 1.0 - ppEyeDiff, 4.0 );	
	
	float dist			= 0.0;	
	float diffMulti		= 1.0;
	float specMulti		= 0.25;
	vec3 ppSpecLighting	= DirectIlluminationWithSpec( vVertex.xyz, vNormal, vEyeDir, dist, vLightPos, 1.0, vec3( 1.0 ), att, diffMulti, specMulti );
	diffMulti = 2.0;
	specMulti = 5.0;
	
//	vec3 pointLighting	= vec3( 0.0 );
//	float index = invNumLightsHalf;
//	for( float i=0.0; i<numLights; i+=1.0 ){
//		vec3 pos		= texture2D( lightsTex, vec2( index, 0.25 ) ).rgb;
//		float color		= texture2D( lightsTex, vec2( index, 0.75 ) ).r;
//		float radius	= 0.0;
//		dist			= 0.0;
//		pointLighting	+= DirectIlluminationWithSpec( vVertex.xyz, vNormal, vEyeDir, dist, pos, radius, vec3( color, 0.0, 0.0  ), att, diffMulti, specMulti );
//		index			+= invNumLights;
//	}
	
	
	vec3 roomLitColor	= vColor.rgb + ppSpecLighting;// + mix( ppSpecLighting, pointLighting, power );
	vec3 roomDarkColor	= vColor.rgb + ppSpecLighting;// + mix( ppSpecLighting, pointLighting, power );
	
	gl_FragColor.rgb	= mix( roomLitColor, roomDarkColor, power );
	gl_FragColor.a		= vColor.a;
}