//
//  Spark.cpp
//  Attraction
//
//  Created by Robert Hodgin on 4/24/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Spark.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;

Spark::Spark()
{
}

Spark::Spark( Vec3f pos, Vec3f vel )
: mPos( pos ), mVel( vel )
{
	mLifespan	= Rand::randFloat( 23.0f, 30.0f );
	mAge		= 0.0f;
	mAgePer		= 1.0f;
	
	mIsDead		= false;
}

void Spark::update( float timeDelta )
{
	mVel -= 0.02f * mVel * timeDelta;
	mPos += mVel * timeDelta;
	mAge += timeDelta;
	mAgePer = 1.0f - mAge/mLifespan;
	
	if( mAge > mLifespan ){
		mIsDead = true;
	}
}