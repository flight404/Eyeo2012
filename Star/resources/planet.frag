uniform sampler2D spectrumTex;
uniform float starColor;
uniform float planetColor;
uniform float power;
uniform vec2 windowDims;
uniform vec3 eyePos;

varying vec4 vVertex;
varying vec3 vNormal;
varying vec3 vEyeDir;

void main()
{
	vec3 lightPos		= vec3( 0.0 );
	vec3 lightDir		= normalize( -vVertex.xyz );
	vec3 HV				= normalize( lightDir + vEyeDir );
	
	float dist			= length( vVertex.xyz );
	float invDistSqrd	= 1.0/( dist * dist );
	
	float distToEye		= distance( vVertex.xyz, eyePos );
	
	float ppDiff		= max( dot( lightDir, vNormal ), 0.0 );
//	float NdotHV		= max( dot( vNormal, HV ), 0.0 );
//	float ppSpec		= pow( NdotHV, 50.0 );
	
	vec3 spectrumCol	= texture2D( spectrumTex, vec2( starColor, 0.25 ) ).rgb;
	vec3 planetCol		= texture2D( spectrumTex, vec2( planetColor, 0.75 ) ).rgb;
	
	vec3 offColor		= vec3( 0.0 );
	vec3 onColor		= vec3( spectrumCol * planetCol * ppDiff );
		
	
	// STUPID FIX TO HIDE DEPTH SORTING ISSUES
	vec2 fragPos		= ( gl_FragCoord.st/windowDims );
	float fragPosLength	= distance( fragPos, vec2( 0.5, 0.5 ) );
	float alpha			= 1.0;
	if( distToEye > 466.0 )
		alpha			= min( fragPosLength * 10.0 - 0.75, 1.0 );
	
	gl_FragColor.rgb	= mix( offColor, onColor, power );
	gl_FragColor.a		= alpha;
}
