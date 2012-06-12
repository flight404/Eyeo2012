//
//  Glob.cpp
//  Forces
//
//  Created by Robert Hodgin on 5/24/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "Glob.h"

using namespace ci;

Glob::Glob()
{
}

Glob::Glob( const Vec3f &pos, const Vec3f &vel, float radius, float lifespan )
{
	mPos			= pos;
	mVel			= vel;
	mRadius			= radius;
	mColor			= 1.0f;
	
	mAge			= 0.0f;
	mAgePer			= 1.0f;
	mLifespan		= Rand::randFloat( lifespan * 0.5f, lifespan );
	mIsDead			= false;
	mBounced		= false;
}

void Glob::update( float floorLevel, float dt )
{
	mVel	+= Vec3f( 0.0f, -0.12f, 0.0f );
	mPos	+= mVel * dt;
	mVel	-= mVel * 0.01 * dt;
	
	if( mPos.y < floorLevel ){
		mPos.y = floorLevel;
		mVel.y *= -0.6f;
		mBounced = true;
	}
	
	mAge	+= dt;
	mAgePer = 1.0f - mAge/mLifespan;
	
	if( mAge > mLifespan ){
		mIsDead = true;
	}
}

void Glob::draw( const Vec3f &right, const Vec3f &up )
{
	gl::drawBillboard( mPos, Vec2f( mRadius, mRadius ), 0.0f, right, up );
}
