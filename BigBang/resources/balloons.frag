uniform float power;
uniform samplerCube	cubeMap;
uniform vec3 eyePos;
uniform vec3 color;
uniform vec3 roomDims;
uniform float att;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;


void main()
{
	vec3 ppNormal		= vNormal * 0.8;
	vec3 lightPos		= vec3( 0.0, 5000.0, 0.0 );
	vec3 lightDir		= lightPos - vVertex.xyz;
	
	float ppDiff		= max( dot( ppNormal, normalize( lightDir ) ), 0.0 );
	float ppFres		= pow( 1.0 - ppDiff, 2.5 );
	
	float ppEyeDiff		= max( dot( ppNormal, vEyeDir ), 0.05 );
	float ppEyeFres		= pow( 1.0 - ppEyeDiff, 6.0 );	
	
	vec3 reflectDir		= reflect( vEyeDir, normalize( ppNormal ) );
	vec3 cubeMapColor	= textureCube( cubeMap, reflectDir ).rgb;
	float cubeDiff		= cubeMapColor.r * 0.3;
	float cubeSpec		= cubeMapColor.g;
	
	float distFromCeiling = clamp( 1.0 - ( roomDims.y - vVertex.y ) * 0.003, 0.0, 1.0 ) * 0.5; 
	float distFromCeilingShine = pow( distFromCeiling * 2.0, 5.0 );
	
	float distFromFloor   = clamp( - ( -roomDims.y - vVertex.y ) * 0.002, 0.0, 1.0 );
	
	float yPer = min( ( vNormal.y * 0.5 + 0.5 ) * 1.5, 1.0 );
	
	gl_FragColor.rgb	= vec3( cubeDiff * ppEyeDiff * color + cubeSpec * pow( 1.0 - ppEyeDiff, 2.0 ) )
						+ ppEyeFres * yPer + color + distFromCeilingShine * color - ( 0.2 - ( vNormal.y * 0.5 + 0.5 ) * 0.2 );
	
//	gl_FragColor.rgb	= vec3( 0.5 + distFromCeilingShine + ppEyeDiff * 0.3 ) * color + distFromCeiling * color + cubeSpec * ppEyeDiff
//						+ distFromCeiling * ppEyeFres * cubeSpec + ppEyeFres * 0.3;
	
//	gl_FragColor.rgb	= vec3( ppEyeFres * yPer );
	gl_FragColor.a		= 1.0 - ppEyeDiff * 0.1;
}