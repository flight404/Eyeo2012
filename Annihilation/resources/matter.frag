uniform float mainPower;
uniform vec3 roomDim;
uniform sampler2D lightsTex;
uniform float numLights;
uniform float invNumLights;
uniform float invNumLightsHalf;
uniform float att;
uniform float time;
uniform samplerCube	cubeMap;

uniform vec4 antimatter;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;


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
	
    return lightColor * ( diff * diffMulti + spec * specMulti ) * attenuation;
}


void main()
{
	vec3 ppNormal		=vNormal;
	vec3 lightPos		= vec3( antimatter.x, antimatter.y, antimatter.z );
	vec3 lightDir		= lightPos - vVertex.xyz;
	vec3 lightDirNorm	= normalize( lightDir );
	
	float radius		= antimatter.w;
	
	float dist			= 0.0;	
	float diffMulti		= 1.0;
	float specMulti		= 0.0025;
	vec3 pointLighting	= DirectIlluminationWithSpec( vVertex.xyz, ppNormal, vEyeDir, dist, lightPos, 1.0, vec3( 1.0 ), att, diffMulti, specMulti );
	diffMulti		= 0.3;
	specMulti		= 0.15;
	
	
	radius = 0.0;
	float index = invNumLightsHalf;
	for( float i=0.0; i<numLights; i+=1.0 ){
		vec3 pointLightPos	= texture2D( lightsTex, vec2( index, 0.25 ) ).rgb;
		vec4 color			= texture2D( lightsTex, vec2( index, 0.75 ) );
//		vec3 color			= vec3( 1.0, 0.5, 0.0 );
		float radius		= 1.0;
		dist				= 0.0;
		
		pointLighting	+= DirectIlluminationWithSpec( vVertex.xyz, ppNormal, vEyeDir, dist, pointLightPos, radius, color.rgb, att, diffMulti * color.a, specMulti * color.a );
		index				+= invNumLights;
	}
	
	
	lightPos		= vec3( 0.0, 500.0, 0.0 );
	lightDir		= lightPos - vVertex.xyz;
	lightDirNorm	= normalize( lightDir );
	
	float ppDiff		= max( dot( vNormal, lightDirNorm ), 0.0 );
	vec3 HV				= normalize( lightDirNorm + vEyeDir );
	float NdotHV		= max( dot( vNormal, HV ), 0.0 );
	float ppSpec		= pow( NdotHV, 30.0 );
	
	float ppFres		= pow( 1.0 - ppDiff, 2.5 );
	float ppEyeDiff		= max( dot( vNormal, vEyeDir ), 0.0 );
	float ppEyeFres		= pow( 1.0 - ppEyeDiff, 2.0 );	
	
	vec3 c = vec3( 1.0 - ppEyeFres * 0.2 );
	
	vec3 reflectDir		= reflect( vEyeDir, vNormal * vec3( 1.0, 1.5, 1.0 ) );
	vec3 envColor		= textureCube( cubeMap, reflectDir ).rgb;
	
	float envGrey		= envColor.r;
	float envSpec		= envColor.g;
	
	float fakeAo		= pow( vNormal.y * 0.3 + 0.3, 0.4 );
	float rimLight		= pow( fakeAo, 4.0 ) * ppEyeFres;
	
	float bloomShadow	= vVertex.y * 0.005 + 0.8;
	float reflection	= envGrey * ppEyeDiff * 0.1;
	float reflectionRim	= envGrey * ppEyeFres * 0.3;
	float fres			= ppEyeFres * 0.2;
	float lighting		= ( envSpec - ppEyeFres ) * 0.1;
	float centerGlow	= ppEyeDiff * 0.4 * bloomShadow;
//	float noise			= rand( gl_TexCoord[0].st + vec2( time, time ) );
	
	
	vec3 litRoomColor	= vec3( reflectionRim + fres + rimLight + centerGlow + ppDiff * 0.2 ) + envSpec * 0.3 * ppEyeFres;
	vec3 darkRoomColor	= vec3( 0.1 - ( ppEyeDiff * 0.3 ) ) + pointLighting;
	
	gl_FragColor.rgb	= mix( litRoomColor, darkRoomColor, mainPower );
	gl_FragColor.a		= 1.0;
}