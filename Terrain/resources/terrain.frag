#version 110
uniform vec3 eyePos;
uniform vec3 lightPos;
uniform sampler2D sandNormalTex;
uniform sampler2D gradientTex;
uniform vec2 mousePosNorm;
uniform vec3 spherePos;
uniform float sphereRadius;
uniform float power;

varying vec4 vVertex;
varying vec3 vNormal;
varying vec3 vEyeDir;
varying vec3 vLightDir;
varying vec3 vFinalCol;


void main()
{	
//	vec3 sandNormal		= texture2D( sandNormalTex, gl_TexCoord[0].st * 30.0 ).rgb * 2.0 - 1.0;
//	vec3 ppNormal		= normalize( vNormal + sandNormal );
	vec3 ppNormal		= vNormal;
	
	
	float ppEyeDiff		= max( dot( ppNormal, vEyeDir ), 0.0 );
	float ppFres		= pow( 1.0 - ppEyeDiff, 5.0 );
	float ppDiff		= max( dot( ppNormal, vLightDir ), 0.0 );
	
	float ppSpec		= pow( ppDiff, 100.0 );
	
	float distToSphere	= distance( vVertex.xyz, vec3( spherePos.x, -200.0, spherePos.z ) ) - sphereRadius * 0.05;
	float distPer		= pow( clamp( distToSphere * 0.05, 0.0, 1.0 ), 2.0 );
	vec3 shadow			= texture2D( gradientTex, vec2( distPer, 0.75 ) ).rgb;
	
	
	float aoLight		= 1.0 - length( vVertex.xyz ) * 0.0011;
	float aoDark		= aoLight * 0.2;
	
	vec3 litRoomColor	= mix( vec3( aoLight * ppDiff + aoLight * ppSpec * 0.01 + 0.05 ), vec3( 0.2 ), ( 1.0 - distPer ) );
	vec3 darkRoomColor	= mix( vFinalCol + ppSpec * 0.1, shadow, ( 1.0 - distPer ) );
	
	
	gl_FragColor.rgb	= mix( litRoomColor, darkRoomColor, power );
	gl_FragColor.a		= 1.0;
}