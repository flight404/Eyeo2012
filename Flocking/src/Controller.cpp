//
//  Controller.cpp
//  Flocking
//
//  Created by Robert Hodgin on 4/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/gl/Gl.h"
#include "cinder/Rand.h"
#include "Controller.h"

using namespace ci;
using std::vector;

Controller::Controller(){}

Controller::Controller( Room *room, int maxLanterns )
{
	mRoom			= room;
	mMaxLanterns	= maxLanterns;
	
//	mNumPredators	= predatorFboDim * predatorFboDim;
//	for( int i=0; i<mNumPredators; i++ ){
//		mPredators.push_back( Predator( Vec3f::zero() ) );
//	}
}

void Controller::updatePredatorBodies( gl::Fbo *fbo )
{
	// BOTH THESE METHODS ARE TOO SLOW.
	// IS THERE NO WAY TO READ OUT THE CONTENTS OF A TINY FBO TEXTURE
	// WITHOUT THE FRAMERATE DROPPING FROM 60 TO 30?
	
//	gl::disableDepthRead();
//	gl::disableDepthWrite();
//	gl::disableAlphaBlending();
//	
//	int index = 0;
//	Surface32f predatorSurface( fbo->getTexture() );
//	Surface32f::Iter it = predatorSurface.getIter();
//	while( it.line() ){
//		while( it.pixel() ){
//			mPredators[index].update( Vec3f( it.r(), it.g(), it.b() ) );
//			index ++;
//		}
//	}
	
//	int index = 0;
//	GLfloat pixel[4];
//	int fboDim = fbo->getWidth();
//	fbo->bindFramebuffer();
//	for( int y=0; y<fboDim; y++ ){
//		for( int x=0; x<fboDim; x++ ){
//			glReadPixels( x, y, 1, 1, GL_RGB, GL_FLOAT, (void *)pixel );
//			mPredators[index].update( Vec3f( pixel[0], pixel[1], pixel[2] ) );
//			index ++;
//		}
//	}
//	fbo->unbindFramebuffer();
}

void Controller::update()
{
	// LANTERNS
	for( std::vector<Lantern>::iterator it = mLanterns.begin(); it != mLanterns.end(); ){
		if( it->mIsDead ){
			it = mLanterns.erase( it );
		} else {
			it->update( mRoom->mTimeAdjusted, mRoom->getFloorLevel() );
			++it;
		}
	}
	
	// ADD LANTERN IF REQUIRED
	mNumLanterns = mLanterns.size();
	if( mRoom->isPowerOn() ){
		if( ( Rand::randFloat() < 0.00175f && mNumLanterns < 3 ) || mNumLanterns < 1 ){
			addLantern( mRoom->getRandCeilingPos() );
		}
	}
	
	// ADD GLOWS AND NEBULA EFFECTS
	if( mRoom->mTick ){
		for( std::vector<Lantern>::iterator it = mLanterns.begin(); it != mLanterns.end(); ){
			// ADD GLOWS
			int numGlowsToSpawn = 5;
			addGlows( &(*it), numGlowsToSpawn );
			
			// ADD NEBULAS
			int numNebulasToSpawn = 1;
			addNebulas( &(*it), numNebulasToSpawn );
			
			++it;
		}
	}
	
	// SORT LANTERNS
	sort( mLanterns.begin(), mLanterns.end(), depthSortFunc );
	
	// GLOWS
	for( vector<Glow>::iterator it = mGlows.begin(); it != mGlows.end(); ){
		if( it->mIsDead ){
			it = mGlows.erase( it );
		} else {
			it->update( mRoom->mTimeAdjusted );
			++ it;
		}
	}
	
	// NEBULAS
	for( vector<Nebula>::iterator it = mNebulas.begin(); it != mNebulas.end(); ){
		if( it->mIsDead ){
			it = mNebulas.erase( it );
		} else {
			it->update( mRoom->mTimeAdjusted );
			++ it;
		}
	}
}



void Controller::drawLanterns( gl::GlslProg *shader )
{
	for( std::vector<Lantern>::iterator it = mLanterns.begin(); it != mLanterns.end(); ++it ){
		shader->uniform( "color", it->mColor );
		it->draw();
	}
}

void Controller::drawLanternGlows( const Vec3f &right, const Vec3f &up )
{
	for( std::vector<Lantern>::iterator it = mLanterns.begin(); it != mLanterns.end(); ++it ){
		float radius = it->mRadius * 10.0f;// * it->mVisiblePer * 10.0f;
		gl::color( Color( it->mColor ) );
		gl::drawBillboard( it->mPos, Vec2f( radius, radius ), 0.0f, right, up );
		gl::color( Color::white() );
		gl::drawBillboard( it->mPos, Vec2f( radius, radius ) * 0.35f, 0.0f, right, up );
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
		shader->uniform( "color", it->mColor );
		it->draw( right, up );
	}
}

void Controller::addLantern( const Vec3f &pos )
{
	if( mNumLanterns < mMaxLanterns ){
		mLanterns.push_back( Lantern( pos ) );
	}
}

void Controller::addGlows( Lantern *lantern, int amt )
{
	for( int i=0; i<amt; i++ ){
		float radius	= Rand::randFloat( 5.0f, 6.0f );
		Vec3f dir		= Rand::randVec3f();
		Vec3f pos		= lantern->mPos + dir * ( lantern->mRadius - radius );
		dir.xz() *= -0.25f;
		Vec3f vel		= dir * Rand::randFloat( 0.3f, 0.5f );
		float lifespan	= Rand::randFloat( 115.0f, 130.0f );
		Color col		= lantern->mColor;
		
		mGlows.push_back( Glow( pos, vel, radius, col, lifespan ) );
		
		if( Rand::randFloat() < 0.01f )
			mGlows.push_back( Glow( pos, Vec3f( vel.x, 1.5f, vel.z ), radius * 0.5f, col, lifespan * 0.5f ) );
	}
}

void Controller::addNebulas( Lantern *lantern, int amt )
{
	for( int i=0; i<amt; i++ ){
		float radius		= Rand::randFloat( 5.0f, 8.0f );
		Vec3f dir			= Rand::randVec3f();
		Vec3f pos			= lantern->mPos + dir * ( lantern->mRadius - radius );
		dir.xz() *= -0.25f;
		Vec3f vel			= dir * Rand::randFloat( 0.05f, 0.1f );
		float lifespan		= Rand::randFloat( 135.0f, 155.0f );
		Color col		= lantern->mColor;
		
		mNebulas.push_back( Nebula( pos, vel, radius, col, lifespan ) );
		
		if( Rand::randFloat() < 0.01f )
			mNebulas.push_back( Nebula( pos, Vec3f( vel.x, 1.5f, vel.z ), radius * 0.5f, col, lifespan * 0.5f ) );
	}
}


bool depthSortFunc( Lantern a, Lantern b ){
	return a.mPos.z > b.mPos.z;
}