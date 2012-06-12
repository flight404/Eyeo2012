uniform vec3 eyePos;
uniform float mainPower;
uniform sampler2D lightsTex;
uniform float numLights;
uniform float invNumLights;
uniform float invNumLightsHalf;
uniform float att;

uniform float time;

uniform samplerCube	cubeMap;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;
varying vec4 vColor;
varying float vVertexLength;

float rand( vec2 co ){
    return fract( sin( dot( co.xy ,vec2( 12.9898,78.233 ) ) ) * 43758.5453 );
}

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
	float spec		= pow( NdotHV, 80.0 );
	
    return lightColor * ( diff * diffMulti + spec * specMulti ) * attenuation;
}


void main()
{
	vec3 lightPos		= vec3( 0.0, 50.0, -200.0 );
	vec3 lightDir		= lightPos - vVertex.xyz;
    float dist			= length(lightDir);
	dist *= dist;
    lightDir /= dist;
    vec3 HV				= normalize( normalize( lightDir ) + vEyeDir );
	
	float ppDiff		= max( dot( lightDir, vNormal ), 0.0 );
	float NdotHV		= max( dot( vNormal, HV ), 0.0 );
	float ppSpec		= pow( NdotHV, 20.0 );
	float ppEyeDiff		= max( dot( vNormal, vEyeDir ), 0.05 );
	float ppEyeFres		= pow( 1.0 - ppEyeDiff, 4.0 );
	
	vec3 reflectDir		= reflect( vEyeDir, normalize( vNormal ) );
	vec3 envColor		= textureCube( cubeMap, reflectDir ).rgb;
	
	float diffMulti		= 1.0;
	float specMulti		= 0.025;
	vec3 pointLighting	= DirectIlluminationWithSpec( vVertex.xyz, vNormal, vEyeDir, dist, lightPos, 1.0, vec3( 1.0 ), att, diffMulti, specMulti );
	diffMulti		= 1.2;
	specMulti		= 10.5;
	
	float index = invNumLightsHalf;
	for( float i=0.0; i<numLights; i+=1.0 ){
		vec3 pointLightPos	= texture2D( lightsTex, vec2( index, 0.25 ) ).rgb;
		vec4 color			= texture2D( lightsTex, vec2( index, 0.75 ) );
		dist			= 0.0;
		
		pointLighting	+= DirectIlluminationWithSpec( vVertex.xyz, vNormal, vEyeDir, dist, pointLightPos, 0.0, color.rgb, att, diffMulti, specMulti );
		index				+= invNumLights;
	}
	
//	float noise			= rand( gl_TexCoord[0].st + vec2( time, time ) );
	
	vec3 litRoomColor	= vec3( envColor * 0.3 + pointLighting * 0.45 );
	vec3 darkRoomColor	= pointLighting;// * noise;
	
	float litAlpha		= 1.0;
	float darkAlpha		= gl_TexCoord[0].s;
	
	gl_FragColor.rgb	= mix( litRoomColor, darkRoomColor, mainPower );
	gl_FragColor.a		= 1.0;//mix( litAlpha, darkAlpha, mainPower );
}