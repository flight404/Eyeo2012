//
//  FieldLine.cpp
//  Forces
//
//  Created by Robert Hodgin on 5/14/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "FieldLine.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using std::vector;

int FieldLine::sLen		 = 10;
float FieldLine::sInvLen = 1.0f/(float)FieldLine::sLen;
int FieldLine::sNumVerts = ( FieldLine::sLen - 1 ) * 2; 

FieldLine::FieldLine()
{
}

FieldLine::FieldLine( const Vec3f &pos, const Vec3f &vel, float charge, float lifespan )
{
	mPos		= pos;
	for( int i=0; i<sLen; i++ ){
		mPositions.push_back( mPos );
//		mNormals.push_back( Vec3f::yAxis() );
//		mPerps.push_back( Vec3f::xAxis() );
	}
	
	mVel		= vel;
	mMaxSpeed	= 8.0f;
	
	mCharge		= charge;
	mAge		= 0.0f;
	mLifespan	= lifespan;
	mIsDead		= false;
}

void FieldLine::update( float dt, bool tick )
{	
	mColor		= mCharge * 0.5f + 0.5f;
	Vec3f dir	= mVel.normalized();
	mVel		= dir * mMaxSpeed;
	mPos		+= mVel * dt;
	
	if( tick ){
		for( int i=sLen-1; i>0; i-- ){
			mPositions[i]	= mPositions[i-1];
//			mNormals[i]		= mNormals[i-1];
//			mPerps[i]		= mPerps[i-1];
		}
	}
	
//	Vec3f perp = dir.cross( Vec3f::zAxis() ).normalized();
//	Vec3f perp2 = dir.cross( perp ).normalized();
//	perp = dir.cross( perp2 ).normalized();
	
//	mPerps[0]		= perp;
//	mNormals[0]		= perp2;
	mPositions[0]	= mPos;
	
	mVel	-= mVel * 0.02 * dt;
	mAge	+= dt;
	mAgePer  = 1.0f - mAge/mLifespan;
	
	if( mAge > mLifespan ){
		mIsDead = true;
	}
}

void FieldLine::draw()
{
}
