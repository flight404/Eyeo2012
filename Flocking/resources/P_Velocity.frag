#version 110
uniform sampler2D positionTex;
uniform sampler2D velocityTex;
uniform int fboDim;
uniform float invFboDim;

uniform sampler2D preyPositionTex;
uniform int preyFboDim;
uniform float invPreyFboDim;
uniform float dt;
uniform float power;
uniform vec3 roomBounds;



uniform sampler2D lanternsTex;
uniform float numLights;
uniform float invNumLights;
uniform float invNumLightsHalf;
uniform float att;

void reactToLanterns( inout vec3 acc, vec3 _myPos )
{
	float index = invNumLightsHalf;
	for( float i=0.0; i<numLights; i+=1.0 ){
		vec4 LANTERN	= texture2D( lanternsTex, vec2( index, 0.25 ) );
		vec3 pos		= LANTERN.xyz;
		float radius	= LANTERN.w + 250.0;
		float radiusSqrd= radius * radius;
		
		vec3 dirToLantern		= _myPos - pos;
		float distToLantern		= length( dirToLantern );
		float distToLanternSqrd	= distToLantern * distToLantern;
		
		// IF WITHIN THE ZONE, REACT TO THE LANTERN
		if( distToLanternSqrd < radiusSqrd ){
			acc += normalize( dirToLantern ) * ( radiusSqrd / distToLanternSqrd ) * dt * 1.0;
		}

		index			+= invNumLights;
	}
}



void reactToPrey( inout vec3 acc, vec3 predatorPos )
{
	for( int y=0; y<preyFboDim; y++ ){
		for( int x=0; x<preyFboDim; x++ ){
//			vec2 tc			= vec2( float(x), float(y) ) * invFboDim + offset;
//			vec4 pos		= texture2D( positionTex, tc );
//			vec3 dir		= myPos - pos.xyz;
//			
			vec2 tc					= vec2( float(x), float(y) ) * invPreyFboDim + invPreyFboDim * 0.5;
			vec3 preyPos			= texture2D( preyPositionTex, tc ).rgb;
			float preyZoneRadius	= 150.0 * 150.0;
			
			vec3 dirToPrey			= predatorPos - preyPos;
			float distToPrey		= length( dirToPrey );
			float distToPreySqrd	= distToPrey * distToPrey;
			
			if( distToPreySqrd < preyZoneRadius ){
				acc -= normalize( dirToPrey ) * ( preyZoneRadius / ( distToPreySqrd + 1.0 ) ) * 0.01 * dt;
			}
		}
	}
}



void main()
{	
	float zoneRadius	 = 130.0 + ( power * 100.0 );
	float zoneRadiusSqrd = zoneRadius * zoneRadius;
	float minThresh		 = 0.14;
	float maxThresh		 = 0.20;
	float maxSpeed		 = 3.0 + ( power * 7.0 );
	
	vec4 vPos			= texture2D( positionTex, gl_TexCoord[0].st );
	vec3 myPos			= vPos.xyz;
	float leadership	= vPos.a;
	
	vec4 vVel			= texture2D( velocityTex, gl_TexCoord[0].st );
	vec3 myVel			= vVel.xyz;
	float myCrowd		= vVel.a;
	
	vec3 acc			= vec3( 0.0, 0.0, 0.0 );
	float offset		= invFboDim * 0.5;
	
	int myX				= int( gl_TexCoord[0].s * float(fboDim) );
	int myY				= int( gl_TexCoord[0].t * float(fboDim) );
	float crowded		= 2.0;
	
	
	// PREY WILL RESPECT THE 3-ZONE FLOCKING
	for( int y=0; y<fboDim; y++ ){
		for( int x=0; x<fboDim; x++ ){
			if( x == myX && y == myY ){
				// Avoid comparing my sphere against my sphere
			} else {
				vec2 tc			= vec2( float(x), float(y) ) * invFboDim + offset;
				vec4 pos		= texture2D( positionTex, tc );
				vec3 dir		= myPos - pos.xyz;
				
				float dist		= length( dir );
				float distSqrd	= dist * dist;
				
				vec3 dirNorm	= normalize( dir );
				
				if( distSqrd < zoneRadiusSqrd ){
					float percent = distSqrd/zoneRadiusSqrd + 0.0000001;
					crowded += ( 1.0 - percent ) * 0.4;
					
					// IF FRIEND IS TOO CLOSE, REPEL
					if( percent < minThresh ){
						float F  = ( minThresh/percent - 1.0 );
						acc		+= dirNorm * F * dt * leadership * 0.25;
						
						// IF FRIEND IS IN THE SWEET SPOT, ALIGN	
					} else if( percent < maxThresh ){
						float threshDelta		= maxThresh - minThresh;
						float adjustedPercent	= ( percent - minThresh )/( threshDelta + 0.0000001 );
						float F					= ( 1.0 - ( cos( adjustedPercent * 6.28318 ) * -0.5 + 0.5 ) );
						
						acc += normalize( texture2D( velocityTex, tc ).xyz ) * F * dt * leadership * 0.25;
						
						// IF FRIEND IS FAR, BUT WITHIN THE ACCEPTABLE ZONE, ATTRACT	
					} else if( dist < zoneRadius ){
						
						float threshDelta		= 1.0 - maxThresh;
						float adjustedPercent	= ( percent - maxThresh )/( threshDelta + 0.0000001 );
						float F					= ( 1.0 - ( cos( adjustedPercent * 6.28318 ) * -0.5 + 0.5 ) ) * dt * leadership * 0.25;
						
						acc -= dirNorm * F;
						
					}
				}
			}
		}
	}
		
	
	if( power > 0.5 )
		reactToPrey( acc, myPos );
	
	reactToLanterns( acc, myPos );
	
	myCrowd -= ( myCrowd - crowded ) * ( 0.1 * dt );
	
	
	// LIMIT SPEED
	myVel += acc * dt;
	float newMaxSpeed = maxSpeed + myCrowd * ( 0.045 + ( power * 0.015 ) );
	
	float velLength = length( myVel );			
	if( velLength > newMaxSpeed ){				
		myVel = normalize( myVel ) * newMaxSpeed;
	}

	
	vec3 tempNewPos		= myPos + myVel * dt;		// NEXT POSITION
	
	// AVOID WALLS
	//if( power > 0.5 ){
	float xPull	= tempNewPos.x/( roomBounds.x );
	float yPull	= tempNewPos.y/( roomBounds.y );
	float zPull	= tempNewPos.z/( roomBounds.z );
	myVel -= vec3( xPull * xPull * xPull * xPull * xPull, 
				  yPull * yPull * yPull * yPull * yPull, 
				  zPull * zPull * zPull * zPull * zPull );
	//}
	
	bool hitWall = false;
	vec3 wallNormal = vec3( 0.0 );
	float myRadius = 4.0;
	
	if( tempNewPos.y - myRadius < -roomBounds.y ){
		hitWall = true;
		wallNormal += vec3( 0.0, 1.0, 0.0 );
	} else if( tempNewPos.y + myRadius > roomBounds.y ){
		hitWall = true;
		wallNormal += vec3( 0.0,-1.0, 0.0 );
	}
	
	if( tempNewPos.x - myRadius < -roomBounds.x ){
		hitWall = true;
		wallNormal += vec3( 1.0, 0.0, 0.0 );
	} else if( tempNewPos.x + myRadius > roomBounds.x ){
		hitWall = true;
		wallNormal += vec3(-1.0, 0.0, 0.0 );
	}
	
	if( tempNewPos.z - myRadius < -roomBounds.z ){
		hitWall = true;
		wallNormal += vec3( 0.0, 0.0, 1.0 );
	} else if( tempNewPos.z + myRadius > roomBounds.z ){
		hitWall = true;
		wallNormal += vec3( 0.0, 0.0,-1.0 );
	}
	
	// WARNING, THIS MAY BE FAULTY MATH. MIGHT EXPLAIN LOST PARTICLES
	if( hitWall ){
		vec3 reflect = 2.0 * wallNormal * ( wallNormal * myVel );
		myVel -= reflect * 0.65;
	}
	
	
	gl_FragColor.rgb	= myVel;
	gl_FragColor.a		= myCrowd;
}