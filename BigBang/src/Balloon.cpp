//
//  Balloon.cpp
//  BigBang
//
//  Created by Robert Hodgin on 5/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Sphere.h"
#include "Balloon.h"

#define SPRING_STRENGTH 0.05
#define SPRING_DAMPING 0.25
#define REST_LENGTH 0.0

using namespace ci;

Balloon::Balloon(){}

Balloon::Balloon( const Vec3f &pos, int presetIndex )
	: mPos( pos )
{
	mVel		= Rand::randVec3f() * Rand::randFloat( 3.0f, 13.0f );
	mAcc		= Vec3f::zero();

	mSpringPos	= mPos - Vec3f( 0.0f, 20.0f, 0.0f );
	mSpringVel	= mVel;
	mSpringPos	= mAcc;
	
	mRadius		= Rand::randFloat( 22.0f, 18.0f );
	mMatrix.setToIdentity();
	
	if( presetIndex == 4 ){
		if( Rand::randBool() ){
			mColor = Color( 0.5f, 0.5f, 0.5f );
		} else {
			mColor = Color::black();
		}		
	} else {
		int i = Rand::randInt(4);
		if( i == 0 ){
			mColor = Color( 0.6f, 0.1f, 0.0f );
		} else if( i == 1 ){
			mColor = Color( 1.0f, 0.5f, 0.0f );
		} else if( i == 2 ){
			mColor = Color( 0.0f, 0.0f, 0.0f );
		}
	}
	
	mBuoyancy	= Rand::randFloat( 0.01f, 0.07f );
	
	mAge		= 0.0f;
	mLifespan	= 10000.0f;
	mIsDead		= false;
}

void Balloon::updateSpring( float dt )
{
	ci::Vec3f dir		= mSpringPos - ( mPos - Vec3f( 0.0f, 20.0f, 0.0f ) );
	float dist			= dir.length();
	dir.safeNormalize();
	float springForce	= -( dist - REST_LENGTH ) * SPRING_STRENGTH;
	float dampingForce	= -SPRING_DAMPING * ( dir.x*mSpringVel.x + dir.y*mSpringVel.y + dir.z*mSpringVel.z );
	float r				= springForce + dampingForce;
	dir *= r;
	mSpringAcc += dir;
	
	mSpringVel += mSpringAcc * dt;
	mSpringPos += mSpringVel * dt;
	mSpringVel -= mSpringVel * 0.04 * dt;
	mSpringAcc = ci::Vec3f::zero();
}

void Balloon::update( const Camera &cam, const Vec3f &roomDims, float dt )
{
	updateSpring( dt );
	
	mAcc += Vec3f( 0.0f, mBuoyancy, 0.0f );
	mVel += mAcc * dt;
	mPos += mVel * dt;
	mVel -= mVel * 0.025f * dt;
	mAcc = Vec3f::zero();
	
	mScreenPos		= cam.worldToScreen( mPos, app::getWindowWidth(), app::getWindowHeight() );
//	mDistToCam		= -cam.worldToEyeDepth( mPos );
	Sphere sphere	= Sphere( mPos, mRadius );
	mScreenRadius	= cam.getScreenRadius( sphere, app::getWindowWidth(), app::getWindowHeight() );
	
	checkBounds( roomDims );
	
	Vec3f tiltAxis = ( mPos - mSpringPos ).normalized();
	
	mMatrix.setToIdentity();
	mMatrix.translate( mPos );
	mMatrix.rotate( tiltAxis, 2.0f );
//	mMatrix.rotate( Vec3f( mAge * mXRot, mYRot, mAge * mZRot ) );
	mMatrix.scale( Vec3f( mRadius, mRadius, mRadius ) );
	
	mAge += dt;
}

void Balloon::checkBounds( const Vec3f &roomDims )
{	
	if( mPos.x - mRadius < -roomDims.x ){
		mPos.x = -roomDims.x + mRadius;
		mVel.x *= -0.915f;
	} else if( mPos.x + mRadius > roomDims.x ){
		mPos.x = roomDims.x - mRadius;
		mVel.x *= -0.915f;		
	}

	if( mPos.y - mRadius < -roomDims.y ){
		mPos.y = -roomDims.y + mRadius;
		mVel.y *= -0.915f;
	} else if( mPos.y + mRadius > roomDims.y ){
		mPos.y = roomDims.y - mRadius;
		mVel.y *= -0.915f;
	}
	
	if( mPos.z - mRadius < -roomDims.z ){
		mPos.z = -roomDims.z + mRadius;
		mVel.z *= -0.915f;
	} else if( mPos.z + mRadius > roomDims.z ){
		mPos.z = roomDims.z - mRadius;
		mVel.z *= -0.915f;		
	}
}

void Balloon::draw()
{
	gl::color( mColor );
	gl::drawSphere( mPos, mRadius );
}