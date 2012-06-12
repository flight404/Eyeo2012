//
//  Particle.cpp
//  Forces
//
//  Created by Robert Hodgin on 5/14/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Sphere.h"
#include "Particle.h"

using namespace ci;

Particle::Particle(){}

Particle::Particle( const Vec3f &pos, float charge )
	: mPos( pos ), mCharge( charge )
{
	mVel			= Vec3f::zero();
	mAcc			= Vec3f::zero();
	mForce			= 0.0f;
	
	mRadius			= 1.0f;
	mShellRadius	= 12.0f;
}

void Particle::update( const Camera &cam, float dt )
{
	Sphere s		= Sphere( mPos, mRadius * 10.0f );
	mScreenPos		= cam.worldToScreen( mPos, app::getWindowWidth(), app::getWindowHeight() );
	mScreenRadius	= cam.getScreenRadius( s, app::getWindowWidth(), app::getWindowHeight() );
	
	mColor			= mCharge * 0.5f + 0.5f;
	
	mVel += mAcc * dt;
	mPos += mVel * dt;
	mAcc = Vec3f::zero();
	
	mShellRadius = mRadius + fabs( mForce ) * 50000.0f;
	
	mMatrix.setToIdentity();
	mMatrix.translate( mPos );
}

void Particle::draw()
{	
	gl::color( Color( mColor, mColor, mColor ) );
	gl::drawSphere( mPos, mRadius );
}

void Particle::addShards( int amt )
{
	int index = 0;
	for( int i=0; i<amt; i++ ){
		Vec3f dir		= Rand::randVec3f();
		Vec3f pos		= mPos + dir;
		dir.normalize();
		
		float height = Rand::randFloat( 1.0f, 3.0f );
		height = height * height;
		if( Rand::randFloat() < 0.1f )
			height *= 2.5f;
		float radius = height * 0.1f;
		mShards.push_back( Shard( Vec3f::zero(), dir, height, radius ) );
		mShards.back().init( &mShardMesh, Color( mColor, mColor, mColor ) );
		
		index ++;
	}
}