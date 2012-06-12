#version 120

uniform float kernelRes;
uniform float invKernelRes;
uniform sampler2D fboTex;
uniform sampler2D kernelTex;
uniform vec2 orientationVector;
uniform float blurAmt;

void main()
{
	vec3 colorSum = vec3( 0.0 );
	for( int i=0; i<kernelRes; i++ ){
		float iPer		= i * invKernelRes;
		vec2 kernel		= texture2D( kernelTex, vec2( iPer, 0.5 ) ).rg;
		float amt		= ( kernel.r - 0.5 ) * blurAmt;
		vec2 offset		= orientationVector * amt;
		
		vec3 ColorSample = texture2D( fboTex, gl_TexCoord[0].st + offset ).rgb;
		colorSum += ColorSample * kernel.g;
	}
	colorSum *= 0.05;

	gl_FragColor.rgb = colorSum;
	gl_FragColor.a   = 1.0;
}