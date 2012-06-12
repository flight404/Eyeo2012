#version 110
uniform float power;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;
varying vec3 vColor;
varying vec3 lightDir;
varying float collideAmt;

void main()
{	
	float ppEyeDiff		= max( dot( vNormal, vEyeDir ), 0.0 );
	float ppEyeFres		= pow( 1.0 - ppEyeDiff, 3.0 );
	float ppDiff		= max( dot( lightDir, vNormal ), 0.0 );
	vec3 HV				= normalize( lightDir + vEyeDir );
	float NdotHV		= max( dot( vNormal, HV ), 0.0 );
	float ppSpec		= pow( NdotHV, 50.0 );
	float ppFres		= pow( 1.0 - ppEyeDiff, 3.0 );
	
	vec3 litRoomColor	= vec3( ppDiff * 0.1 + ppSpec * ( 1.0 - power ) * 0.3 + ppEyeFres * 0.2 + ppEyeDiff * 0.1 );
	vec3 darkRoomColor	= vec3( ppEyeFres * 0.1 );
	
	gl_FragColor.rgb	= mix( litRoomColor, darkRoomColor, power ) + vec3( collideAmt, 0.0, 0.0 );
	gl_FragColor.a		= 1.0;
}



