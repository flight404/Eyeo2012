uniform vec3 eyePos;
uniform float radius;
uniform vec3 roomDim;
uniform float numLights;
uniform float invNumLights;
uniform float invNumLightsHalf;
uniform sampler2D lightsTex;
uniform float att;
uniform float time;
uniform float mainPower;
uniform samplerCube	cubeMap;

uniform float charge;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;
varying vec4 vColor;

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
	float spec		= pow( NdotHV, 30.0 );
	
    return lightColor * ( diff * diffMulti + spec * specMulti ) * attenuation;
}


void main()
{
	vec3 ppNormal		= ( vNormal * 0.5 );
	vec3 lightPos		= vec3( 0.0, 500.0, 0.0 );
	vec3 lightDir		= lightPos - vVertex.xyz;
	
	float ppDiff		= max( dot( ppNormal, normalize( lightDir ) ), 0.0 );

	float dist			= 0.0;	
	float diffMulti		= 1.0;
	float specMulti		= 0.25;
	vec3 ppSpecLighting	= DirectIlluminationWithSpec( vVertex.xyz, ppNormal, vEyeDir, dist, lightPos, 0.0, vec3( 1.0 ), att, diffMulti, specMulti );
	diffMulti		= 4.5;
	specMulti		= 4.2;
	
	float ppFres		= pow( 1.0 - ppDiff, 2.5 );

//	
//	float ao = vColor.r;
//	
//	float c;
//	if( charge > 0.5 ){
//		c = 1.0 - ppFres * 1.5;
//	} else {
//		c = ppSpec + ppFres * 0.2;
//	}
	
	
	float ppEyeDiff		= max( dot( ppNormal, vEyeDir ), 0.05 );
	float ppEyeFres		= pow( 1.0 - ppEyeDiff, 4.0 );	
	
	vec3 c;
	vec3 starLighting		= vec3( 0.0 );

	if( charge > 0.5 ){ // white
		c = vec3( 1.0 - ppEyeFres * 0.2 );
	} else {			// black
		c = vec3( 0.15 - ( ppEyeDiff * 0.2 ) );

		
	}
	
	
	vec3 finalColor;
	if( mainPower < 0.5 ){
		vec3 reflectDir		= reflect( vEyeDir, normalize( ppNormal * vec3( 0.85, 1.0, 1.5 ) ) );
		vec3 envColor		= textureCube( cubeMap, reflectDir ).rgb;
		
		float bloomShadow	= vVertex.y * 0.005 + 0.8;
		float shadow		= ( 1.0 - mainPower ) * pow( ppNormal.y * 0.5 + 0.5, 0.25 );
		float fakeAo		= pow( ppNormal.y * 0.3 + 0.3, 0.4 );
		float shine			= ( 0.1 + ppDiff * 0.4 + ppSpecLighting.r + ppFres * 0.2 );
		float topShadow		= pow( fakeAo, 1.0 ) * ppEyeFres * 0.5;
		float rimLight		= pow( fakeAo, 4.0 ) * ppEyeFres;
		float fres			= ppEyeFres * 0.2;
		float centerGlow	= ppEyeDiff * 0.2;
//		float noise			= rand( gl_TexCoord[0].st + vec2( time, time ) );
		
		vec3 litRoomColor	= c * vec3( fakeAo * shine + centerGlow * fakeAo - topShadow + rimLight * 2.0 ) + envColor * ppEyeFres * 0.7 + envColor * ppEyeDiff * 0.4 + c * 0.1;
		
		finalColor			= litRoomColor;
	}
	 else {
		float index = invNumLightsHalf;
		for( float i=0.0; i<numLights; i+=1.0 ){
			vec3 pos		= texture2D( lightsTex, vec2( index, 0.25 ) ).rgb;
			vec3 color		= texture2D( lightsTex, vec2( index, 0.75 ) ).rgb;
			float radius	= 0.0;
			dist			= 0.0;
			
			starLighting	+= DirectIlluminationWithSpec( vVertex.xyz, ppNormal, vEyeDir, dist, pos, radius, color, att, diffMulti, specMulti );
			
			index			+= invNumLights;
		}
		
		vec3 darkRoomColor	= starLighting + c;
		finalColor			= darkRoomColor;
	}
	
	gl_FragColor.rgb	= finalColor;
	gl_FragColor.a		= 1.0;
}