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

Shockwave::Shockwave( const Vec3f &pos, const Vec3f &axis, float lifespan, float speed )
{
	mPos			= pos;
	mAxis			= axis;
	mRadius			= 1.0f;
	mRadiusPrev		= 0.0f;
	mLifespan		= lifespan;
	mSpeed			= speed;
	mStrength		= 0.3f;
	
	mColor			= Color( 1.0f, 0.1f, 0.1f );
	if( Rand::randBool() )
		mColor		= Color( 0.1f, 0.1f, 1.0f );
	
	mAge			= 0.0f;
	mIsDead			= false;
	
	mShell			= Rand::randFloat( 100.0f, 120.0f );
	mShellDest		= mShell * 1.3f;
	
	mImpulse		= 1.0f;
	
	mRot			= Rand::randFloat( M_PI * 2.0f );
	mRotVel			= Rand::randFloat( -0.005f, 0.005f );
}

void Shockwave::update( float timeDelta ){
	mRadiusPrev	= mRadius - 8.0f;
	mRadius		+= mSpeed * timeDelta;
	mAge		+= timeDelta;
	mAgePer		= 1.0f - mAge/mLifespan;
	mShell		-= ( mShell - mShellDest ) * ( 0.1f * timeDelta );
	mShellDest	+= 3.0f * timeDelta;
	
	mImpulse	*= 0.975f;
	
	mRot		+= mRotVel * timeDelta;
	if( mAge > mLifespan ){
		mIsDead = true;
	}
}

