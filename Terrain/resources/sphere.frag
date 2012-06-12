uniform sampler2D heightsTex;
uniform vec3 eyePos;
uniform float radius;
uniform float power;
uniform vec3 roomDim;
uniform vec3 fogColor;
uniform float mainPower;
uniform samplerCube	cubeMap;
uniform float color;
uniform vec3 sandColor;
uniform float timePer;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;
varying vec3 vFog;
varying float vHeight;
varying float vDist;

void main()
{	
	vec3 lightPos		= vec3( 0.0, 500.0, 0.0 );
	vec3 lightDir		= lightPos - vVertex.xyz;
	vec3 lightDirNorm	= normalize( lightDir );
	
	float ppDiff		= max( dot( vNormal, lightDirNorm ), 0.0 );
	vec3 HV				= normalize( lightDirNorm + vEyeDir );
	float NdotHV		= max( dot( vNormal, HV ), 0.0 );
	float ppSpec		= pow( NdotHV, 30.0 );
	
	float ppFres		= pow( 1.0 - ppDiff, 1.5 );
	float ppEyeDiff		= max( dot( vNormal, vEyeDir ), 0.0 );
	float ppEyeFres		= pow( 1.0 - ppEyeDiff, 2.0 );
	
	vec3 c = vec3( 1.0 - ppEyeFres * 0.2 );
	
	vec3 reflectDir		= reflect( vEyeDir, vNormal * vec3( 1.0, 1.5, 1.0 ) );
	vec3 envColor		= textureCube( cubeMap, reflectDir ).rgb;
	
	float envGrey		= envColor.r;
	float envSpec		= envColor.g;
	float envDark		= envColor.b;
	
	float fakeAo		= pow( vNormal.y * 0.3 + 0.3, 0.4 );
	float rimLight		= pow( fakeAo, 4.0 ) * ppEyeFres;
	
	float bloomShadow	= vNormal.y * 0.5 + 0.5;
	float reflection	= envGrey * ppEyeDiff * 0.1;
	float reflectionRim	= envGrey * ppEyeFres * 0.3;
	float fres			= ppEyeFres * 0.2;
	float lighting		= ( envSpec - ppEyeFres ) * 0.1;
	float centerGlow	= ppEyeDiff * 0.4 * bloomShadow;
	
//	float ext			= ( 1.0 - texture2D( heightsTex, gl_TexCoord[0].st * 4.0 ).b ) * 0.01;
	
	vec3 skyGreen		= vec3( 164.0/255.0, 196.0/255.0, 158.0/255.0 );
	
	vec3 litRoomColor	= vec3( fres + rimLight + centerGlow + ppDiff * 0.2 ) + envSpec * 0.3 * ppEyeFres * timePer;
	vec3 darkRoomColor	= vec3( ( ppFres * 0.2 * sandColor ) + ppDiff * skyGreen + envDark * ppEyeFres * 0.4 );
	
	gl_FragColor.rgb	= mix( litRoomColor, mix( fogColor, darkRoomColor, vDist ), power );
	gl_FragColor.a		= 1.0;
}