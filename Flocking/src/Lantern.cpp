//
//  Bait.cpp
//  Flocking
//
//  Created by Robert Hodgin on 4/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "Lantern.h"

using namespace ci;

Lantern::Lantern( const Vec3f &pos )
{
	mPos		= pos;
	mRadius		= 0.0f;
	mRadiusDest	= Rand::randFloat( 4.5f, 7.5f );
	if( Rand::randFloat() < 0.1f ) mRadiusDest = Rand::randFloat( 13.0f, 25.0f );
	
	mFallSpeed	= Rand::randFloat( -0.5f, -0.15f );
	mColor		= Color( CM_HSV, Rand::randFloat( 0.0f, 0.1f ), 0.9f, 1.0f );
	mIsDead		= false;
	mIsDying	= false;
	
	mVisiblePer	= 1.0f;
}

void Lantern::update( float dt, float yFloor ){
	mPos += Vec3f( 0.0f, mFallSpeed * dt, 0.0f );
	if( ( mPos.y + mRadiusDest ) < yFloor ){
		mIsSinking = true;
		mIsDying = true;
	}
	
	
	if( mIsSinking ){
		mVisiblePer = 1.0f - ( ( mPos.y + mRadiusDest ) - yFloor ) / ( mRadius + mRadius );
	}
	
	
	if( mIsDying ){
		mRadius -= ( mRadius - 0.0f ) * 0.2f;
		if( mRadius < 0.1f )
			mIsDead = true;
	} else {
		mRadius -= ( mRadius - ( mRadiusDest + Rand::randFloat( 0.9f, 1.2f ) ) ) * 0.2f;
	}
}

void Lantern::draw(){
	gl::drawSphere( mPos, mRadius * 0.5f, 32 );
}