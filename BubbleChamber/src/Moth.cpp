//
//  Moth.cpp
//  BubbleChamber
//
//  Created by Robert Hodgin on 5/7/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "Moth.h"

using namespace ci;

Moth::Moth()
{
}

Moth::Moth( const Vec3f &pos )
: mPos( pos )
{
	mAcc	= Vec3f::zero();
	mVel	= Vec3f::zero();
	
	mIsDead		= false;
	
	mAge	= Rand::randFloat( 6.0f );
	
	mColorf		= 1.0f;
}

void Moth::update( Room *room, float dt )
{
	Vec3f oldPos = mPos;
		
	mAcc += Vec3f( 0.0f, sin( mAge * Rand::randFloat( 5.0f, 10.0f ) ) * 0.1f, 0.0f );
	mVel += mAcc * dt;
	
	float velLength = mVel.length();
	mAxis = mVel.normalized();
	if( velLength < 0.3f ){
		mVel = mAxis * 0.3f;
	}
	else if( velLength > 2.0f ){
		mVel = mAxis * 2.0f;
	}


	checkBoundary( room );
	
	mPos += mVel * dt;
	mVel -= mVel * 0.01f * dt;
	mAcc = Vec3f::zero();
	
	mAge += dt;
}

void Moth::checkBoundary( Room *room )
{
	// TEST BOUNDARY
	Vec3f dims = room->getDims() * 0.85f;
	
	
	if( mPos.x < -dims.x ){
		mVel.x += 1.0f;
	} else if( mPos.x > dims.x ){
		mVel.x -= 1.0f;
	}
	
	if( mPos.y < -dims.y ){
		mVel.y += 1.0f;
	} else if( mPos.y > dims.y ){
		mVel.y -= 1.0f;
	}
	
	if( mPos.z < -dims.z ){
		mVel.z += 1.0f;
	} else if( mPos.z > dims.z ){
		mVel.z -= 1.0f;
	}
}

void Moth::draw()
{
	float size = ( sin( mAge ) * 0.5f + 0.5f ) * 0.5f + 0.75f;
	mColorf = sin( mAge * 3.0f ) * 0.3f + 0.5f;
	gl::color( Color( mColorf, mColorf, mColorf ) );
	gl::drawCube( mPos, Vec3f( size, size, size ) );
}