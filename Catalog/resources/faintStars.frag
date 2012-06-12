#version 110
uniform sampler2D spectrumTex;
uniform float power;
uniform vec3 roomDims;
uniform float time;

varying vec4 vVertex;
varying vec4 vColor;


float rand( vec2 co ){
    return fract( sin( dot( co.xy ,vec2( 12.9898,78.233 ) ) ) * 43758.5453 );
}

void main()
{
//	if( power < 0.2 ){
//		if( vVertex.x > roomDims.x || vVertex.x < -roomDims.x )
//			discard;
//		
//		if( vVertex.y > roomDims.y || vVertex.y < -roomDims.y )
//			discard;
//		
//		if( vVertex.z > roomDims.z || vVertex.z < -roomDims.z )
//			discard;
//	}
	
	vec3 spectrumCol	= texture2D( spectrumTex, vec2( vColor.r, 0.5 ) ).rgb;
	vec4 litRoomColor	= vec4( 0.0, 0.0, 0.0, 0.3 );
	float noise = rand( vec2( vVertex.xy + time ) );
	vec4 darkRoomColor	= vec4( spectrumCol, 0.05 + noise * 0.2 );
	vec4 color			= mix( litRoomColor, darkRoomColor, power );
	
	gl_FragColor		= color;
}



