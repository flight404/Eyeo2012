#version 120
uniform sampler2D heightsTex;
uniform sampler2D normalsTex;
uniform vec2 texCoord;
uniform vec3 eyePos;
uniform vec3 fogColor;
uniform mat4 mvpMatrix;
uniform float power;
uniform float zoomMulti;
uniform vec3 terrainScale;
uniform float sphereRadius;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;
varying vec3 vFog;
varying float vHeight;
varying float vDist;


void main()
{
	gl_TexCoord[0]	= gl_MultiTexCoord0;
	
	float zoom		= zoomMulti * 0.96 + 0.04;
	float zoomScale = 2.0 - zoomMulti * 0.85;
	vec2 zoomCoords = texCoord * zoom + ( 1.0 - zoom ) * 0.5;
	
	float s			= 0.025 * zoom;
	float h0 		= texture2D( heightsTex, zoomCoords + vec2( -s, 0.0 ) ).b;
	float h1		= texture2D( heightsTex, zoomCoords + vec2(  s, 0.0 ) ).b;
	float h2		= texture2D( heightsTex, zoomCoords + vec2(  0.0, -s ) ).b;
	float h3		= texture2D( heightsTex, zoomCoords + vec2(  0.0,  s ) ).b;
	vHeight			= ( h0 + h1 + h2 + h3 ) * 0.25;

	vNormal			= normalize( gl_Normal );
	vVertex			= vec4( gl_Vertex );
	//	vVertex.y		*= terrainScale.y;
	vVertex.y		+= vHeight * ( pow( ( zoomScale ) + 1.2, 7.0 ) * 0.0035 );
	vVertex.y		+= -200.0 + sphereRadius;
	
	
	
//	vec3 bumpNormal	= texture2D( normalsTex, gl_TexCoord[0].st ).rgb;
//	float hOff		= texture2D( heightsTex, gl_TexCoord[0].st ).b;
//	
////	vec3 T, B, N;
////	vec3 c1		= cross( vNormal, vec3(0.0, 0.0, 1.0) ); 
////	vec3 c2		= cross( vNormal, vec3(0.0, 1.0, 0.0) ); 
////	
////	if( length(c1) > length(c2) ){
////		T = c1;	
////	} else {
////		T = c2;	
////	}
////	
////	T = normalize( T );
////	B = cross( vNormal, T ); 
////	B = normalize( B );
//	
//	
//	vec3 normal		= normalize(gl_NormalMatrix * gl_Normal);
//	mat3 trans		= transpose(gl_NormalMatrix);
//	vec3 tangent	= gl_NormalMatrix * trans[0]; 
//	vec3 binormal	= gl_NormalMatrix * trans[1];
//
//	vec3 newNormal = vec3( dot( bumpNormal, tangent ),
//						   dot( bumpNormal, binormal ),
//						   dot( bumpNormal, normal ) );
//	
//	vVertex.xyz		+= newNormal * hOff * 0.3;
//	vNormal			= newNormal;

	float dist		= pow( distance( eyePos, vVertex.xyz * vec3( 1.5, 1.0, 1.5 ) ) * 0.001425, 3.0 );
	vDist			= clamp( 1.0 - dist, 0.0, 1.0 ) * power;
	vFog			= mix( fogColor, vec3( 1.0 ), vDist );
	
	vEyeDir			= normalize( eyePos - vVertex.xyz );
	
	gl_Position		= mvpMatrix * vVertex;
	
}


