uniform sampler2D positionFboTex;
uniform sampler2D sphereNormal;
uniform mat4 mvMatrix;
//uniform mat4 arcballQuat;
uniform float scale;
uniform float power;
uniform float zoomPer;
uniform vec3 roomDims;
uniform float texScale;
uniform vec3 eyePos;

varying vec4 vVertex;
varying vec4 vColor;
varying float vSpectrum;
varying vec3 vEyeDir;
varying vec3 vNormal;
//varying float closenessAlpha;

void main()
{
	gl_TexCoord[0]	= gl_MultiTexCoord0;
	vVertex			= gl_Vertex;
	vColor			= gl_Color;
	
	vec4 pos		= texture2D( positionFboTex, vColor.rg );
	vSpectrum		= fract( pos.a );
	float radius	= pos.a/1000.0;
	
	vVertex.xyz		*= mat3( mvMatrix[0].xyz, mvMatrix[1].xyz, mvMatrix[2].xyz );				// BILLBOARD
//	vVertex.xyz		*= mat3( arcballQuat[0].xyz, arcballQuat[1].xyz, arcballQuat[2].xyz );		// ARCBALLED
	
	
	vVertex.xyz		*= scale * texScale * radius;				// SCALE IMAGE
	vVertex.xyz		+= pos.xyz * scale;		// MOVE TO SCALED POSITION

	vec3 eyeDir		= eyePos - vVertex.xyz;
	vEyeDir			= normalize( eyeDir );
	
	
//	closenessAlpha	= clamp( max( length( eyeDir ) - 20.0, 0.0 ) * 0.0025, 0.0, 1.0 );
	
	
//	if( power < 0.2 ){
//		bool norm = false;
//		if( vVertex.x > roomDims.x || vVertex.x < -roomDims.x )
//			norm = true;
//		
//		if( vVertex.y > roomDims.y || vVertex.y < -roomDims.y )
//			norm = true;
//		
//		if( vVertex.z > roomDims.z || vVertex.z < -roomDims.z )
//			norm = true;
//		
//		if( norm ){
//			vVertex.xyz = normalize( vVertex.xyz ) * roomDims;
//		}
//	}
	
	
	
	gl_Position		= gl_ModelViewProjectionMatrix * vVertex;
}
