uniform vec3 eyePos;
uniform float radius;
uniform vec3 roomDim;
uniform float mainPower;
uniform vec3 color;


uniform float charge;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;


void main()
{
	if( vVertex.x > roomDim.x || vVertex.x < -roomDim.x )
		discard;
	
	if( vVertex.y > roomDim.y || vVertex.y < -roomDim.y )
		discard;
	
	vec3 lightPos		= vec3( 0.0, 500.0, 0.0 );
	vec3 lightDir		= lightPos - vVertex.xyz;
	
	float ppDiff		= max( dot( vNormal, normalize( lightDir ) ), 0.0 );
	float ppFres		= pow( 1.0 - ppDiff, 2.5 );
	float ppEyeDiff		= max( dot( vNormal, vEyeDir ), 0.05 );
	float ppEyeFres		= pow( 1.0 - ppEyeDiff, 4.0 );	
	
	float shadow		= ( 1.0 - mainPower ) * pow( vNormal.y * 0.5 + 0.5, 0.25 );
	float fakeAo		= pow( vNormal.y * 0.3 + 0.3, 0.4 );
	float shine			= ( 0.1 + ppDiff * 0.4 + ppFres * 0.2 );
	float topShadow		= pow( fakeAo, 1.0 ) * ppEyeFres * 0.5;
	float rimLight		= pow( fakeAo, 4.0 ) * ppEyeFres;
	float fres			= ppEyeFres * 0.2;
	float centerGlow	= ppEyeDiff * 0.2;
	
	
	vec3 litRoomColor	= vec3( 1.0 - ppEyeFres * 0.2 ) * vec3( fakeAo * shine + centerGlow * fakeAo - topShadow + rimLight * 2.0 );
	vec3 darkRoomColor	= vec3( color );
	
	gl_FragColor.rgb	= mix( litRoomColor, darkRoomColor, mainPower );
	gl_FragColor.a		= 0.0;
}