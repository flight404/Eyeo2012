uniform sampler2D heightsTex;
uniform sampler2D normalsTex;
uniform sampler2D gradientTex;
uniform sampler2D normalTex;
uniform vec3 roomDims;
uniform vec3 eyePos;
uniform vec3 lightPos;
uniform vec3 fogColor;
uniform vec3 sandColor;
uniform vec3 terrainScale;
uniform float power;
uniform float zoomMulti;
uniform mat4 mvpMatrix;

varying vec4 vVertex;
varying vec3 vNormal;
varying vec3 vEyeDir;
varying vec3 vLightDir;
varying float vDiff, vDist, vHeight;
varying vec3 vFinalCol;

void main()
{
	gl_TexCoord[0]	= gl_MultiTexCoord0;
	
	float zoom		= zoomMulti * 0.96 + 0.04;
	float zoomScale = 2.0 - zoomMulti * 0.85;
	vec2 zoomCoords = gl_TexCoord[0].st * zoom + ( 1.0 - zoom ) * 0.5;
	vec3 heightsCol = texture2D( heightsTex, zoomCoords ).rgb;
	vNormal			= texture2D( normalsTex, zoomCoords ).rgb;
	
	vVertex			= vec4( gl_Vertex );
//	vVertex.xyz		-= vNormal * 2.0;
	vVertex.xyz		*= terrainScale;
	vVertex.y		+= heightsCol.b * ( pow( ( zoomScale ) + 1.2, 7.0 ) * 0.0035 );
	vVertex.y		-= roomDims.y;
	

	vLightDir		= normalize( lightPos );
//	if( power > 0.5 ) vLightDir = normalize( vec3( 1.0, 1.0, 1.0 ) );
	
	vEyeDir			= normalize( eyePos - vVertex.xyz );
	
	vDiff			= clamp( dot( vLightDir, vNormal ), 0.0, 1.0 );
	float dist		= pow( distance( eyePos, vVertex.xyz * vec3( 1.5, 1.0, 1.5 ) ) * 0.001425, 3.0 );
	vDist			= clamp( 1.0 - dist, 0.0, 1.0 ) * power;
	
//	vVertex.y		-= sin( vDist * 0.1 ) * 500.0 - 40.0;
	
	vec3 gradientCol  = texture2D( gradientTex, vec2( 1.0 - vDist, 0.25 ) ).rgb;
	
	vFinalCol		= mix( fogColor, gradientCol * vDiff, vDist );
	
	gl_Position		= mvpMatrix * vVertex;
}
