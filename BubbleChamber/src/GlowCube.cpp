//
//  GlowCube.cpp
//  BubbleChamber
//
//  Created by Robert Hodgin on 5/7/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "GlowCube.h"

using namespace ci;

GlowCube::GlowCube()
{
}

GlowCube::GlowCube( const Vec3f &pos )
: mPos( pos )
{
	mAcc		= Vec3f::zero();
	mVel		= Vec3f::zero();
	mAxis		= Rand::randVec3f();
	mAngle		= Rand::randFloat( M_PI * 2.0f );
	mAngleVel	= Rand::randFloat( -0.1f, 0.1f );
	mIsDead		= false;
	
	mAge		= Rand::randFloat( 6.0f );
	mAgePer		= 1.0f;
	mLifespan	= Rand::randFloat( 50.0f, 150.0f );
	mRadius		= Rand::randFloat( 4.0f, 10.0f );
}

void GlowCube::update( float dt )
{
//	mVel += mAcc * dt;
//	mPos += mVel * dt;
//	mVel -= mVel * 0.01f * dt;
//	mAcc = Vec3f::zero();
	
	mAngle += dt * mAngleVel;
	
	mMatrix.setToIdentity();
	mMatrix.translate( mPos );
	mMatrix.scale( Vec3f( mRadius, mRadius, mRadius ) );
	mMatrix.rotate( mAxis * mAngle );
	
	mAge += dt;
	mAgePer = 1.0f - mAge/mLifespan;
	if( mAge > mLifespan ){
		mIsDead = true;
	}
}

void GlowCube::draw()
{
	gl::drawCube( Vec3f::zero(), Vec3f( 1.0f, 1.0f, 1.0f ) * mAgePer );
}