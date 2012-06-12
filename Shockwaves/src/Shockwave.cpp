//
//  Shockwave.cpp
//  Collider
//
//  Created by Robert Hodgin on 3/6/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Shockwave.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;

Shockwave::Shockwave()
{
}

Shockwave::Shockwave( const Vec3f &pos, float lifespan, float speed )
{
	mPos			= pos + Vec3f( 0.1f, 0.1f, 0.1f );
	mRadius			= 1.0f;
	mRadiusPrev		= 0.0f;
	mLifespan		= lifespan;
	mSpeed			= speed;
	mStrength		= 1.0f;
	
	mColor			= Color( 0.0f, 0.0f, 0.0f );
	
	mAge			= 0.0f;
	mIsDead			= false;
	
	mImpulse		= 2.0f;
	
	mMatrix.setToIdentity();
}

void Shockwave::update( float dt ){
	mRadiusPrev	= mRadius;// - 28.0f;
	mRadius		+= mSpeed * dt;
	
	mImpulse	-= mImpulse * 0.025f * dt;
	
	mMatrix.setToIdentity();
	mMatrix.translate( mPos );
	mMatrix.scale( Vec3f( mRadius, mRadius, mRadius ) );
	
	mAge		+= dt;
	mAgePer		= 1.0f - mAge/mLifespan;
	if( mAge > mLifespan ){
		mIsDead = true;
	}
}

void Shockwave::draw()
{
	gl::drawSphere( mPos, 10.0f );
}

