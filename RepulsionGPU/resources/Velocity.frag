#version 110
uniform sampler2D position;
uniform sampler2D velocity;
uniform int w;
uniform int h;
uniform float invWidth;
uniform float invHeight;
uniform vec3 roomBounds;
uniform float dt;
uniform float mainPower;
uniform float gravity;


const float M_4_PI = 12.566370614359172;

struct Sphere
{
	vec3	pos;
	vec3	vel;
	vec3	acc;
	float	mass;
	float	radius;
};


float getRadiusFromMass( float m )
{
	float r = pow( (3.0 * m )/M_4_PI, 0.3333333 );
	return r;
}

float getMassFromRadius( float r )
{
	float m = ( ( r * r * r ) * M_4_PI ) * 0.33333;
	return m;
}

//
// from http://wp.freya.no/3d-math-and-physics/simple-sphere-sphere-collision-detection-and-collision-response/
//
bool simpleSphereSphere( const Sphere a, const Sphere b )
{
	vec3 dir		= a.pos - b.pos;
	float dist		= length( dir );
	float sumRadii	= a.radius + b.radius;
	
	if( dist <= sumRadii ){
		return true;
	}
	
	return false;
}

bool advancedSphereSphere( Sphere a, Sphere b, inout float testValue, inout float t )
{
	vec3 s		= a.pos - b.pos;
	vec3 v		= a.vel - b.vel;
	float r		= a.radius + b.radius;
	
	float c1	= dot( s, s ) - r*r;	// dot product with self = lengthsquared.
	
	if( c1 < 0.0 ){				// if overlapping, return true
		testValue = 1.0;
		t		= 0.0;
		return true;
	}
	
	float a1 = dot( v, v );		// if velocity change is near zero, return false
	if( a1 < 0.00001 ){
		testValue = 0.0;
		return false;
	}
	
	float b1 = dot( v, s );
	if( b1 >= 0.0 ){
		testValue = 0.0;
		return false;
	}
	
	float d1 = b1*b1 - a1*c1;
	if( d1 < 0.0 ){
		testValue = 0.0;
		return false;
	}
	
	t = ( -b1 - sqrt(d1) )/a1;
	testValue = 1.0;
	return true;
}

void sphereCollisionResponse( inout Sphere a, inout Sphere b )
{
    vec3 U1x,U1y,U2x,U2y,V1x,V1y,V2x,V2y;
	
	
    float x1, x2;
    vec3 v1temp, v1, v2, v1x, v2x, v1y, v2y;
	vec3 x = a.pos - b.pos;
	
    normalize( x );
    v1	= a.vel;
    x1	= dot( x, v1 );
    v1x = x * x1;
    v1y = v1 - v1x;
	
    x	*= -1.0;
    v2	= b.vel;
    x2	= dot( x, v2 );
    v2x = x * x2;
    v2y = v2 - v2x;
	
	float m1	= a.mass;
    float m2	= b.mass;
	float sumMass = a.mass + b.mass;
	a.vel += vec3( v1x * (m1-m2)/sumMass + v2x * (2.0*m2)/sumMass + v1y ) * 0.002;
	//	b.vel += vec3( v1x * (2.0*m1)/sumMass + v2x*(m2-m1)/sumMass + v2y );
}

float distSqrd( vec3 amt )
{
	return ( amt.x * amt.x + amt.y * amt.y + amt.z * amt.z ); 
}


void doPhysics( inout Sphere s1, inout vec3 posOff, inout float massTransferred, float dt, inout float val, bool step )
{
	vec2 texCoord		= gl_TexCoord[0].st;
	
	int myX = int( gl_TexCoord[0].s * float(w) );
	int myY = int( gl_TexCoord[0].t * float(h) );
	
	for( int y=0; y<h; y++ ){
		for( int x=0; x<w; x++ ){
			
			if( x == myX && y == myY ){
				
			} else {
				vec2 tc			= vec2( float(x) * invWidth, float(y) * invHeight );
				vec4 P			= texture2D( position, tc );
				vec4 V			= texture2D( velocity, tc );
				
				vec3 pos		= P.xyz;
				vec3 vel		= V.xyz;
				vec3 acc		= vec3( 0.0 );
				float mass		= P.a;
				float radius	= getRadiusFromMass( mass );
				Sphere s2		= Sphere( pos, vel, acc, mass, radius );
				
				vec3 dir		= s1.pos - s2.pos;
				
				if( simpleSphereSphere( s1, s2 ) ){
					sphereCollisionResponse( s1, s2 );
					val += 0.01;
					float overlap = length( dir ) - s1.radius - s2.radius;
					if( overlap <= 0.0 ){
						posOff += normalize( dir ) * -overlap * 0.03;
						
//						// IM BIGGER SO I GAIN YOUR MASS
//						if( s1.radius > s2.radius ){
//							float new2Radius = s2.radius + overlap;	// overlap is negative
//							float new2Mass = getMassFromRadius( new2Radius );
//							massTransferred += s2.mass - new2Mass;
//							
//						// IM SMALLER SO I LOSE MASS
//						} else {
//							float new1Radius = s1.radius + overlap;	// overlap is negative
//							float new1Mass = getMassFromRadius( new1Radius );
//							massTransferred -= s1.mass - new1Mass;
//						}
					}
					
				} else {
//					float dist	= length( dir );
//					if( dist < ( s1.radius + s2.radius ) * 2.0 ){		// Repel cause too close
//						float F		= ( 1.0 )/( dist * dist );
//						dir			= normalize( dir ) * F * 0.001;
//						
//						s1.acc		+= dir;
//					} else {				// Attract
					float distSq = distSqrd( dir );
					float F		= ( s1.mass * s2.mass )/distSq;
					dir			= normalize( dir ) * F * 0.0001;
					
					s1.acc		-= dir/s1.mass;
					//					}
				}
				
			}
		}
	}
}

void main()
{	
	float maxSpeed = 10.0;
	
	vec2 texCoord	= gl_TexCoord[0].st;
	vec4 vPos		= texture2D( position, texCoord );	// my current position
	vec4 vVel		= texture2D( velocity, texCoord );	// my current velocity
	
	// CREATE MY CURRENT SPHERE
	vec3 pos		= vPos.xyz;
	vec3 vel		= vVel.xyz * 0.925;
	vec3 acc		= vec3( 0.0 );
	float mass		= vPos.a;
	float radius	= getRadiusFromMass( mass );
	Sphere s1		= Sphere( pos, vel, acc, mass, radius ); 	
	
	//	float wOffset		= invWidth * 0.5;
	//	float hOffset		= invHeight * 0.5;
	//	int myX = int( ( texCoord.s - wOffset ) * float(w) );
	//	int myY = int( ( texCoord.t - hOffset ) * float(h) );
	
	vec3 posOffset		= vec3( 0.0 );
	float radiusOffset	= 0.0;
	float massTrans		= 0.0;
	
	float collideAmt = 0.0;
	doPhysics( s1, posOffset, massTrans, dt, collideAmt, false );
	
	s1.acc += vec3( 0.0, gravity, 0.0 );
	s1.vel += s1.acc * dt;
	
	vec3 tempNewPos		= s1.pos + s1.vel * dt;		// NEXT POSITION
	
	
	// AVOID WALLS
	//	if( mainPower > 0.5 ){
	float xPull	= tempNewPos.x/( roomBounds.x );
	float yPull	= tempNewPos.y/( roomBounds.y );
	float zPull	= tempNewPos.z/( roomBounds.z );
	s1.vel -= vec3( xPull * xPull * xPull * xPull * xPull * xPull * xPull * xPull * xPull, 
				   yPull * yPull * yPull * yPull * yPull * yPull * yPull * yPull * yPull, 
				   zPull * zPull * zPull * zPull * zPull * zPull * zPull * zPull * zPull ) * 0.1;
	//	}
	
	//	float velLength = length( s1.vel );						// GET READY TO IMPOSE SPEED LIMIT
	//	if( velLength > maxSpeed ){							// SPEED LIMIT FOR FAST
	//		s1.vel = normalize( s1.vel ) * maxSpeed;
	//	}
	//	
	
	bool hitWall = false;
	vec3 wallNormal = vec3( 0.0 );
	
	if( tempNewPos.y - s1.radius < -roomBounds.y ){
		hitWall = true;
		wallNormal += vec3( 0.0, 1.0, 0.0 );
		float delta = -roomBounds.y - ( tempNewPos.y - s1.radius );
		posOffset.y += delta;
	} else if( tempNewPos.y + s1.radius > roomBounds.y ){
		hitWall = true;
		wallNormal += vec3( 0.0,-1.0, 0.0 );
		float delta = roomBounds.y - ( tempNewPos.y + s1.radius );
		posOffset.y += delta;
	}
	
	if( tempNewPos.x - s1.radius < -roomBounds.x ){
		hitWall = true;
		wallNormal += vec3( 1.0, 0.0, 0.0 );
		float delta = -roomBounds.x - ( tempNewPos.x - s1.radius );
		posOffset.x += delta;
	} else if( tempNewPos.x + s1.radius > roomBounds.x ){
		hitWall = true;
		wallNormal += vec3(-1.0, 0.0, 0.0 );
		float delta = roomBounds.x - ( tempNewPos.x + s1.radius );
		posOffset.x += delta;
	}
	
	if( tempNewPos.z - s1.radius < -roomBounds.z ){
		hitWall = true;
		wallNormal += vec3( 0.0, 0.0, 1.0 );
		float delta = -roomBounds.z - ( tempNewPos.z - s1.radius );
		posOffset.z += delta;
	} else if( tempNewPos.z + s1.radius > roomBounds.z ){
		hitWall = true;
		wallNormal += vec3( 0.0, 0.0,-1.0 );
		float delta = roomBounds.z - ( tempNewPos.z + s1.radius );
		posOffset.z += delta;
	}
	
	// WARNING, THIS MAY BE FAULTY MATH. MIGHT EXPLAIN LOST PARTICLES
	if( hitWall ){
		vec3 reflect = 2.0 * wallNormal * ( wallNormal * s1.vel );
		s1.vel -= reflect * 0.65;
	}
	
	gl_FragData[0]		= vec4( s1.vel, collideAmt );
	gl_FragData[1]		= vec4( posOffset, massTrans );
}
