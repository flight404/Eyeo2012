#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

#define M_4_PI 12.566370614359172

using namespace ci;

Particle::Particle()
{
}

Particle::Particle( const Vec3f &pos, const Vec3f &vel, float charge )
{
	init( pos, vel, charge );
}

Particle::Particle( const Vec3f &pos, const Vec3f &vel )
{
	float charge = -1.0f;
	if( Rand::randBool() ){
		charge = 1.0f;
	}
	init( pos, vel, charge );
}

void Particle::init( const Vec3f &pos, const Vec3f &vel, float charge )
{
	mPos			= pos;
	mVel			= vel;
	mAcc			= Vec3f::zero();
	
	mCharge			= charge;
	mCol			= Color( 0.0f, 1.0f, 0.0f );
	
	mDecay			= 0.9875f;
	mRadiusInit		= 2.0f;
	mRadiusDest		= mRadiusInit;
	mRadius			= 0.001f;
	mMass			= 100.0f;
	mInvMass		= 1.0f/mMass;
	mIsDead			= false;
	
	mFusionThresh	= 0.0f;
	
	mDistPer		= 1.0f;
	mDistToClosestNeighbor = 10000.0f;
}

float Particle::getMassFromRadius( float r )
{
	float m = ( ( r * r * r ) * (float)M_4_PI ) * 0.33333f;
	return m;
}

void Particle::update( Room *room, float dt )
{	
	mVel += mAcc * dt;
//	mVelNormal = mVel.normalized();
//	limitSpeed();
	if( mVel.length() > 10.0f ){
		mVel = mVel.normalized() * 10.0f;
	}
	mPos += mVel * dt;
	
	mDistPer = ( 1.0f/mDistToClosestNeighbor ) * 75.0f;
	mRadiusDest = math<float>::min( mRadiusInit + mDistPer, 40.0f );
	mRadius -= ( mRadius - mRadiusDest ) * 0.1f;
	mMass = getMassFromRadius( mRadius );
	mInvMass = 1.0f/mMass;
	
	Vec3f roomBounds = room->getDims();

	float bounceDecay = -0.815f;
	if( mPos.x - mRadius < -roomBounds.x ){
		mPos.x = -roomBounds.x + mRadius;
		mVel.x *= bounceDecay;
	} else if( mPos.x + mRadius > roomBounds.x ){
		mPos.x = roomBounds.x - mRadius;
		mVel.x *= bounceDecay;
	}
	
	if( mPos.y - mRadius < -roomBounds.y ){
		mPos.y = -roomBounds.y + mRadius;
		mVel.y *= bounceDecay;
	} else if( mPos.y + mRadius > roomBounds.y ){
		mPos.y = roomBounds.y - mRadius;
		mVel.y *= bounceDecay;
	}
	
	if( mPos.z - mRadius < -roomBounds.z ){
		mPos.z = -roomBounds.z + mRadius;
		mVel.z *= bounceDecay;
	} else if( mPos.z + mRadius > roomBounds.z ){
		mPos.z = roomBounds.z - mRadius;
		mVel.z *= bounceDecay;
	}
	
	mVel -= mVel * 0.01f * dt;
	mFusionThresh -= ( mFusionThresh ) * 0.1f;
	
	mAcc = Vec3f::zero();
}

void Particle::draw( gl::VboMesh &sphereLo, gl::VboMesh &sphereHi )
{	

	gl::color( mCol );
	if( mRadius < 15.0f )
		gl::draw( sphereLo );
	else
		gl::draw( sphereHi );
}

void Particle::finish()
{
	mDistToClosestNeighbor = 10000000.0f;
}


