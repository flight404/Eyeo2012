//
//  Streamer.cpp
//  BigBang
//
//  Created by Robert Hodgin on 5/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "Streamer.h"

using namespace ci;

Streamer::Streamer(){}

Streamer::Streamer( const Vec3f &pos, int presetIndex )
	: mPos( pos )
{
	mVel = Rand::randVec3f() * Rand::randFloat( 15.0f, 30.0f );
	mAcc = Vec3f::zero();
	
	mLen = Rand::randInt( 60, 80 );
	for( int i=0; i<mLen; i++ ){
		mPositions.push_back( mPos );
		mVelocities.push_back( Vec3f::zero() );
		mAccels.push_back( Vec3f::zero() );
	}
	
	mRadius		= Rand::randFloat( 3.0f, 5.0f );
	
	int i = Rand::randInt(4);
	if( i == 0 ){
		mColor = Color( 0.6f, 0.1f, 0.0f );
	} else if( i == 1 ){
		mColor = Color( 1.0f, 0.5f, 0.0f );
	} else if( i == 2 ){
		mColor = Color( 0.0f, 0.0f, 0.0f );
	}
	
	mGravity	= Rand::randFloat( -0.2f, -0.25f );
	
	mFrameCount	= 0;
	mAge		= 0.0f;
	mLifespan	= 100.0f;
	mIsDead		= false;
}

void Streamer::update( const Vec3f &roomDims, float dt, bool tick )
{
	mAcc += Vec3f( 0.0f, mGravity, 0.0f ) * dt;
	mVel += mAcc * dt;
	mPos += mVel * dt;
	mVel -= mVel * 0.05f * dt;
	mAcc = Vec3f::zero();
	
	checkBounds( roomDims );
	
	if( mFrameCount < mLen ){
		if( tick ){
			for( int i=mLen-1; i>0; i-- ){
				mPositions[i] = mPositions[i-1];
			}
		}
		mPositions[0] = mPos;
	}
	
	for( int i=0; i<mLen; i++ ){
		mVelocities[i] += mAccels[i] * dt;
		mPositions[i] += mVelocities[i] * dt;
		mVelocities[i] -= mVelocities[i] * 0.15f * dt;
		
		if( mPositions[i].y - mRadius < -roomDims.y ){
			mPositions[i].y = -roomDims.y + mRadius;
			mVelocities[i].y = 0.0f;
		}
	}
	
	if( tick ){
		mFrameCount ++;
	}
	
	mAge += dt;
	mAgePer = 1.0f - mAge/mLifespan;
	if( mAge > mLifespan ) mIsDead = true;
}

void Streamer::checkBounds( const Vec3f &roomDims )
{	
	if( mPos.x - mRadius < -roomDims.x ){
		mPos.x = -roomDims.x + mRadius;
		mVel.x *= -0.15f;
	} else if( mPos.x + mRadius > roomDims.x ){
		mPos.x = roomDims.x - mRadius;
		mVel.x *= -0.15f;		
	}
	
	if( mPos.z - mRadius < -roomDims.z ){
		mPos.z = -roomDims.z + mRadius;
		mVel.z *= -0.15f;
	} else if( mPos.z + mRadius > roomDims.z ){
		mPos.z = roomDims.z - mRadius;
		mVel.z *= -0.15f;		
	}
}

void Streamer::draw()
{
	glBegin( GL_TRIANGLE_STRIP );
	for( int i=0; i<mLen-1; i++ ){
		Vec3f p1 = mPositions[i];
		Vec3f p2 = mPositions[i+1];
		Vec3f dir = p2 - p1;
		dir.normalize();
		Vec3f perp1 = dir.cross( Vec3f::yAxis() );
		perp1.normalize();
		gl::vertex( p1 - perp1 * 2.0f * mAgePer );
		gl::vertex( p1 + perp1 * 2.0f * mAgePer );
	}
	glEnd();
}