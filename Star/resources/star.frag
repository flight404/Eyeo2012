uniform vec3 eyePos;
uniform float radius;
uniform vec3 roomDim;
uniform float mainPower;
uniform samplerCube	cubeMap;
uniform sampler2D spectrumTex;
uniform float color;

uniform float time;
uniform float charge;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;
varying vec4 vColor;


void main()
{
//	if( vVertex.x > roomDim.x || vVertex.x < -roomDim.x )
//		discard;
//	
//	if( vVertex.y > roomDim.y || vVertex.y < -roomDim.y )
//		discard;
//	
//	if( vVertex.z > roomDim.z || vVertex.z < -roomDim.z )
//		discard;
	
	vec3 spectrumCol	= texture2D( spectrumTex, vec2( color, 0.25 ) ).rgb;
	
	vec3 lightPos		= vec3( 0.0, 500.0, 0.0 );
	vec3 lightDir		= lightPos - vVertex.xyz;
	vec3 lightDirNorm	= normalize( lightDir );
	
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

	
	
	vec3 litRoomColor	= vec3( reflectionRim + fres + rimLight + centerGlow + ppDiff * 0.2 ) + envSpec * 0.3 * ppEyeFres;
	vec3 darkRoomColor	= vec3( ppEyeDiff + 0.5 ) * 0.7 + color * 0.4 + 0.4 + pow( ppEyeFres, 2.0 ) * 0.4;
	
	gl_FragColor.rgb	= mix( litRoomColor, darkRoomColor * spectrumCol, mainPower );
	gl_FragColor.a		= 1.0;
}




