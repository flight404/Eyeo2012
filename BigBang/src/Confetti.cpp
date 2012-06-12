//
//  Confetti.cpp
//  BigBang
//
//  Created by Robert Hodgin on 5/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Quaternion.h"
#include "Confetti.h"

using namespace ci;

Confetti::Confetti(){}

Confetti::Confetti( const Vec3f &pos, float speedMulti, int presetIndex )
	: mPos( pos )
{
	mVel		= Rand::randVec3f() * Rand::randFloat( 25.0f, 50.0f ) * speedMulti;
	mDriftVel	= Rand::randVec3f() * 0.3f;
	mDriftVel.y = 0.0f;
	mAcc		= Vec3f::zero();
	
	mRadius		= 2.0f;
	
	mColor		= Color( CM_HSV, Rand::randFloat(), 1.0f, 1.0f );
	
	mXRot		= Rand::randFloat( 20.0f, 40.0f );
	mYRot		= Rand::randFloat( 90.0f );
	mZRot		= Rand::randFloat( 10.0f, 25.0f );
	
	Vec3f axis	= Rand::randVec3f();
	axis.y = 0.0f;
	mMatrix.setToIdentity();
	
	mGravity	= Rand::randFloat( -0.3f, -0.1f );
	mAge		= 0.0f;
	mAgePer		= 1.0f;
	mLifespan	= Rand::randFloat( 500.0f, 1000.0f );
	mIsDead		= false;
	mHasLanded	= false;
}

void Confetti::update( const Vec3f &roomDims, float dt )
{
//	if( mPos.y <= -roomDims.y ){
//		mPos.y = -roomDims.y;
//		mXRot = 0.0f;
//		mZRot = 0.0f;
//		mHasLanded = true;
//	} else {
//		mHasLanded = false;
//	}
	
	
	if( mPos.y > -roomDims.y ){
		mAcc += Vec3f( 0.0f, mGravity, 0.0f );
		mVel += mAcc * dt;
		mPos += mVel * dt;
		mPos += mDriftVel * dt;
		mVel -= mVel * 0.15f * dt;
		mAcc = Vec3f::zero();
		
		checkBounds( roomDims );
	} else {
		mXRot = 0.0f;
		mZRot = 0.0f;
	}
	
	mAge += dt;
	mAgePer = 1.0f - mAge/mLifespan;
	if( mAge > mLifespan ) mIsDead = true;
	
	
	mMatrix.setToIdentity();
	mMatrix.translate( mPos );
	mMatrix.rotate( Vec3f( mAge * mXRot, mYRot, mAge * mZRot ) );
	mMatrix.scale( Vec3f( mRadius, 0.1f, mRadius ) * mAgePer );
}

void Confetti::checkBounds( const Vec3f &roomDims )
{	
	if( mPos.x - mRadius < -roomDims.x ){
		mPos.x = -roomDims.x + mRadius;
		mVel.x *= -0.915f;
	} else if( mPos.x + mRadius > roomDims.x ){
		mPos.x = roomDims.x - mRadius;
		mVel.x *= -0.915f;		
	}
	
	if( mPos.y + mRadius > roomDims.y ){
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

void Confetti::draw()
{
	gl::color( mColor );
	gl::drawCube( Vec3f::zero(), Vec3f( 1.0f, 1.0f, 1.0f ) );
}


