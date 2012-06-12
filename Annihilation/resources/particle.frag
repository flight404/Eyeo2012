uniform vec3 eyePos;
uniform float radius;
uniform vec3 roomDim;
uniform float numLights;
uniform float invNumLights;
uniform float invNumLightsHalf;
uniform sampler2D lightsTex;
uniform sampler2D sandNormalTex;
uniform float att;
uniform float mainPower;

uniform float charge;

varying vec3 eyeDir;
varying vec4 vVertex;
varying vec3 vNormal;
varying vec4 vColor;
varying float vVertexLength;


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
	vec3 sandNormal		= texture2D( sandNormalTex, gl_TexCoord[0].st ).rgb * 2.0 - 1.0;
	vec3 ppNormal		= ( vNormal * 0.9 + sandNormal * 0.1 );
	vec3 eyeDirNorm		= normalize( eyeDir );
	
	vec3 lightPos		= vec3( 0.0, 500.0, 0.0 );
	vec3 lightDir		= lightPos - vVertex.xyz;

	float dist			= 0.0;	
	float diffMulti		= 4.5;
	float specMulti		= 4.2;

	
	
	float ppEyeDiff		= max( dot( ppNormal, eyeDirNorm ), 0.05 );
	float ppEyeFres		= pow( 1.0 - ppEyeDiff, 4.0 );	
	
	vec3 c;
	vec3 starLighting		= vec3( 0.0 );

	if( charge < 0.5 ){ // white
		c = vec3( 1.0 - ppEyeFres * 0.2 );
	} else {			// black
		c = vec3( 0.15 - ( ppEyeDiff * 0.2 ) );

		float index = invNumLightsHalf;
		for( float i=0.0; i<numLights; i+=1.0 ){
			vec3 pos		= texture2D( lightsTex, vec2( index, 0.25 ) ).rgb;
			vec3 color		= texture2D( lightsTex, vec2( index, 0.75 ) ).rgb;
			float radius	= 0.0;
			dist			= 0.0;
			
			starLighting	+= DirectIlluminationWithSpec( vVertex.xyz, ppNormal, eyeDirNorm, dist, pos, radius, color, att, diffMulti, specMulti );
			
			index			+= invNumLights;
		}
	}

	gl_FragColor.rgb	= starLighting;
	gl_FragColor.a		= 1.0;
}