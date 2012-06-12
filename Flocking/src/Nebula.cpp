//
//  Nebula.cpp
//  Collider
//
//  Created by Robert Hodgin on 2/9/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "Nebula.h"

using namespace ci;

Nebula::Nebula()
{
}

Nebula::Nebula( const Vec3f &pos, const Vec3f &vel, float radius, Color c, float lifespan )
{
	mPos			= pos;
	mVel			= vel;
	mRadiusDest		= radius;
	mRadius			= radius;
	mColor			= c;
	mAge			= 0.0f;
	mLifespan		= Rand::randFloat( lifespan * 0.5f, lifespan );
	mRadiusMulti	= 0.3f;
	mRot			= Rand::randFloat( 360.0f );
	mAgePer			= 0.0f;
	mIsDead			= false;
}

void Nebula::update( float dt )
{
	mPos	+= mVel * dt;
	mVel	-= mVel * 0.01 * dt;
	mRadiusDest += mRadiusMulti * dt;
	mRadiusMulti -= mRadiusMulti * 0.03f * dt;
	
	mRadius -= ( mRadius - mRadiusDest ) * 0.1f * dt;
	
	mAge += dt;
	mAgePer = 1.0f - mAge/mLifespan;//sin( ( mAge/mLifespan ) * M_PI );
	
	if( mAge > mLifespan ){
		mIsDead = true;
	}
}

void Nebula::draw( const Vec3f &right, const Vec3f &up )
{
	gl::drawBillboard( mPos, Vec2f( mRadius, mRadius ), mRot, right, up );
}
