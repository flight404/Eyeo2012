#version 120

uniform float kernelRes;
uniform float invKernelRes;
uniform sampler2D fboTex;
uniform sampler2D kernelTex;
uniform vec2 orientationVector;

void main()
{
	float blurAmt = 0.0125;
	
	vec4 ColorSum = vec4( 0.0 );
	for( int i=0; i<kernelRes; i++ ){
		float iPer		= i * invKernelRes;
		vec2 kernel		= texture2D( kernelTex, vec2( iPer, 0.5 ) ).rg;
		float amt		= ( kernel.r - 0.5 ) * blurAmt;
		vec2 offset		= orientationVector * amt;
		
		vec4 ColorSample = texture2D( fboTex, gl_TexCoord[0].st + offset );
		ColorSum += ColorSample * kernel.g;
	}
	
	gl_FragColor = ColorSum * 0.10;
}