//
//  Predator.cpp
//  FlockingFix
//
//  Created by Robert Hodgin on 5/12/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "Predator.h"

using namespace ci;
using std::vector;

Predator::Predator(){}

Predator::Predator( const Vec3f &pos )
{
	mLen = 15;
	for( int i=0; i<mLen; i++ ){
		float per = (float)i/(float)(mLen-1);
		mRadii.push_back( sin( per * M_PI ) );
		mPositions.push_back( Rand::randVec3f() * 50.0f );
	}
}

void Predator::update( const Vec3f &pos )
{
	for( int i=mLen-1; i>0; i-- ){
		mPositions[i] = mPositions[i-1];
	}
	mPos			= pos;
	mPositions[0]	= mPos;
}

void Predator::draw()
{
	for( int i=0; i<mLen-1; i++ ){
		gl::vertex( mPositions[i] );
		gl::vertex( mPositions[i+1] );
	}
}