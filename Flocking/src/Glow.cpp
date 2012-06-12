//
//  Glow.cpp
//  Collider
//
//  Created by Robert Hodgin on 3/6/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Glow.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;

Glow::Glow()
{
}

Glow::Glow( const Vec3f &pos, const Vec3f &vel, float radius, Color c, float lifespan )
{
	mPos			= pos;
	mVel			= vel;
	mRadiusDest		= radius;
	mRadius			= radius * 1.25;
	mColor			= c;
	mAge			= 0.0f;
	mLifespan		= lifespan;
	mIsDead			= false;
	mRot			= Rand::randFloat( 360.0f );
	mRotVel			= Rand::randFloat( -10.0f, 10.0f );
	
	if( Rand::randFloat() < 0.01f ){
		mRadius *= 2.0f;
		mLifespan *= 1.3f;
		mRotVel	= Rand::randPosNegFloat( 1.0f, 3.0f );
	}
}

void Glow::update( float dt )
{
	mRadius -= ( mRadius - mRadiusDest ) * 0.1f * dt;
	mPos	+= mVel * dt;
	mVel	-= mVel * 0.02 * dt;
	mAge	+= dt;
	mAgePer  = 1.0f - mAge/mLifespan;
	mRot	+= mRotVel * dt;
	//			mRadius *= 0.99f;
	
	if( mAge > mLifespan ){// || mRadius < 1.0f ){
		mIsDead = true;
		//				mRadius = 1.0f;
	}
}

void Glow::draw( const Vec3f &right, const Vec3f &up )
{
	gl::drawBillboard( mPos, ci::Vec2f( mRadius, mRadius ) * mAgePer, mRot, right, up );
}
