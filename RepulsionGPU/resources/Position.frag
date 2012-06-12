#version 110
uniform sampler2D pos0Tex;
uniform sampler2D pos1Tex;
uniform sampler2D vel0Tex;
uniform sampler2D vel1Tex;
uniform float dt;

// POSITION RGB
// RADIUS	A

void main()
{	
	vec4 vPos0			= texture2D( pos0Tex, gl_TexCoord[0].st );
	vec4 vPos1			= texture2D( pos1Tex, gl_TexCoord[0].st );
	vec4 vVel0			= texture2D( vel0Tex, gl_TexCoord[0].st );
	vec4 vVel1			= texture2D( vel1Tex, gl_TexCoord[0].st );
	
	vec3 newPos			= vPos0.xyz + ( vVel0.xyz * dt );
	newPos += vVel1.xyz;
	
	float mass			= vPos0.a;// + vVel1.a;
	
//	if( mass < 0.1 ){
//		mass = 1.0;
//		newPos = vPos1.xyz * 200.0;
//		mass = vPos1.w;
//	}
	
	
	gl_FragData[0]		= vec4( newPos, mass );
	gl_FragData[1]		= vPos1;
}
