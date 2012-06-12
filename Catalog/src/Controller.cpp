//
//  Controller.cpp
//  Star
//
//  Created by Robert Hodgin on 5/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/Rand.h"
#include "Controller.h"

using namespace ci;
using std::vector;

Controller::Controller()
{
}

void Controller::init( Room *room )
{
	mRoom				= room;
}

void Controller::update( float dt )
{
	// GLOWS
	for( vector<Glow>::iterator it = mGlows.begin(); it != mGlows.end(); ){
		if( it->mIsDead ){
			it = mGlows.erase( it );
		} else {
			it->update( dt );
			++ it;
		}
	}
	
	// NEBULAS
	for( vector<Nebula>::iterator it = mNebulas.begin(); it != mNebulas.end(); ){
		if( it->mIsDead ){
			it = mNebulas.erase( it );
		} else {
			it->update( dt );
			++ it;
		}
	}
}

void Controller::drawGlows( gl::GlslProg *shader, const Vec3f &right, const Vec3f &up )
{
	for( vector<Glow>::iterator it = mGlows.begin(); it != mGlows.end(); ++it ){
		shader->uniform( "alpha", it->mAgePer );
		shader->uniform( "color", it->mColor );
		it->draw( right, up );
	}
}

void Controller::drawNebulas( gl::GlslProg *shader, const Vec3f &right, const Vec3f &up )
{
	for( vector<Nebula>::iterator it = mNebulas.begin(); it != mNebulas.end(); ++it ){
		shader->uniform( "alpha", it->mAgePer );
		it->draw( right, up );
	}
}

void Controller::addGlows( const Star &star, int amt )
{
	for( int i=0; i<amt; i++ ){
		float radius	= Rand::randFloat( 1.0f, 4.0f );// * star.mRadiusMulti;
		Vec3f dir		= Rand::randVec3f();
		Vec3f pos		= star.mPos + dir * star.mRadius * 45.0f;//( star.mScreenRadius * 32.0f );
		Vec3f vel		= dir * Rand::randFloat( 0.1f, 0.2f );
		float color		= 1.0f - star.mColor;
		float lifespan	= Rand::randFloat( 25.0f, 45.0f );
		
		mGlows.push_back( Glow( pos, vel, radius, color, lifespan ) );
	}
}

void Controller::addNebulas( const Star &star, int amt )
{
	for( int i=0; i<amt; i++ ){
		float radius	= Rand::randFloat( 1.0f, 1.5f );// * star.mRadiusMulti;
		Vec3f dir		= Rand::randVec3f();
		Vec3f pos		= star.mPos + dir * star.mRadius * 45.0f;//( star.mScreenRadius * 35.5f );
		Vec3f vel		= dir * Rand::randFloat( 0.05f, 0.15f );
		
		float lifespan	= Rand::randFloat( 23.0f, 36.0f );
		
		mNebulas.push_back( Nebula( pos, vel, radius, lifespan ) );
	}
}

void Controller::clear()
{
	mNebulas.clear();
	mGlows.clear();
}
	


