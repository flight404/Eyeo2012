uniform vec3 eyePos;
uniform vec3 pos;
uniform float radius;
uniform mat4 matrix;
uniform mat4 mvpMatrix;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;

void main()
{
	vNormal			= normalize( vec3( matrix * vec4( gl_Normal, 0.0 ) ) );
	
	vVertex			= matrix * vec4( gl_Vertex );
	
//	vVertex.xz *= 0.9;
//	
//	if( vVertex.y < -0.99 )
//		vVertex.xz *= 1.4;
//	
//	if( vNormal.y > 0.0 ){
//		vVertex.xyz *= radius;
//	} else {
//		vVertex.xyz *= ( radius + ( pow( vNormal.y, 13.2 ) * radius * 0.4 ) ); 
//	}
	
//	vVertex.xyz		*= radius;//vec3( vVertex.x * radius * ( vNormal.y * 0.5 + 0.5 ), vVertex.y * radius, vVertex.z * radius * ( vNormal.y * 0.5 + 0.5 ) );
	
	
	vEyeDir			= normalize( eyePos - vVertex.xyz );
	
	gl_Position		= mvpMatrix * vVertex;
	gl_TexCoord[0]	= gl_MultiTexCoord0;
}


