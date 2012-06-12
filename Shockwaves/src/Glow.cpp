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

Glow::Glow( const Vec3f &pos, const Vec3f &vel, float radius, float lifespan )
{
	mPos		= pos;
	mVel		= vel;
	mRadius		= radius;
	mAge		= 0.0f;
	mLifespan	= lifespan;
	mIsDead		= false;
	mRot		= Rand::randFloat( 360.0f );
	mRotVel		= Rand::randFloat( -0.5f, 0.5f );
}

void Glow::update( float dt )
{
	mPos	+= mVel * dt;
	mVel	-= mVel * 0.01 * dt;

	//			mRadius *= 0.99f;
	
	mRot	+= mRotVel * dt;
	mRadius += mRadius * 0.035f * dt;
	
	mAge	+= dt;
	mAgePer  = 1.0f - mAge/mLifespan;
	if( mAge > mLifespan ){
		mIsDead = true;
	}
}

void Glow::draw( const Vec3f &right, const Vec3f &up )
{
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, mAgePer * 0.75f ) );
	gl::drawBillboard( mPos, Vec2f( mRadius, mRadius ), mRot, right, up );
}