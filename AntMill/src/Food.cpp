//
//  Food.cpp
//  AntMill
//
//  Created by Robert Hodgin on 5/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "Food.h"

using namespace ci;

Food::Food()
{
}

Food::Food( const Vec3f &pos, float charge )
	: mPos( pos ), mCharge( charge )
{
	mVel		= Vec3f::zero();
	mGrabbed	= false;
	float c		= mCharge * 0.5f + 0.5f;
	mColor		= Color( c, c, c );
	mIsGone		= false;
}

void Food::update( float dt, const Vec3f &roomDims )
{
	if( mIsGone ){
		mVel += Vec3f( 0.0f, -0.2f, 0.0f );
	}
	
	mPos += mVel * dt;
	mVel -= mVel * 0.04f * dt;
	
	stayInBounds( &mPos, roomDims );
}

void Food::draw()
{
	gl::drawSphere( mPos, 2.0f, 4 );
}

void Food::stayInBounds( Vec3f *v, const Vec3f &roomDims )
{
//	float boundsOffset = 1.0f;
	if( v->x < -roomDims.x ){
		v->x = -roomDims.x;
		mVel.x *= -0.5f;
	} else if( v->x > roomDims.x ){
		v->x = roomDims.x;
		mVel.x *= -0.5f;
	}
	
	if( v->y < -roomDims.y ){
		v->y = -roomDims.y;
		mVel.y *= -0.5f;
	} else if( v->y > roomDims.y ){
		v->y = roomDims.y;
		mVel.y *= -0.5f;
	}
	
	if( v->z < -roomDims.z ){
		v->z = -roomDims.z;
		mVel.z *= -0.5f;
	} else if( v->z > roomDims.z ){
		v->z = roomDims.z;
		mVel.z *= -0.5f;
	}
}