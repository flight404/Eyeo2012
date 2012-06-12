#version 110
uniform sampler2D spectrumTex;
uniform sampler2D starTex;
uniform float power;
uniform float zoomPer;
uniform vec3 roomDims;
uniform float texScale;
uniform vec3 eyePos;

varying vec4 vVertex;
varying float vSpectrum;
varying vec3 vEyeDir;
varying vec3 vNormal;

void main()
{
	vec3 spectrumCol	= texture2D( spectrumTex, vec2( vSpectrum, 0.5 ) ).rgb;
	if( texScale < 0.5 ) spectrumCol = vec3( 1.0 );
	vec4 starCol		= texture2D( starTex, gl_TexCoord[0].st );
	float alpha			= starCol.a;
	
	vec3 litRoomColor	= vec3( starCol.rgb );
	vec3 darkRoomColor	= starCol.rgb * spectrumCol;
	
	vec3 finalColor		= mix( litRoomColor, darkRoomColor, power );
	gl_FragColor.rgb	= finalColor;
	gl_FragColor.a		= alpha;
}



