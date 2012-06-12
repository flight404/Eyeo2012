//
//  Star.cpp
//  Star
//
//  Created by Robert Hodgin on 4/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "Star.h"

#define M_4_PI 12.566370614359172

using namespace ci;
using std::vector;

Star::Star()
{
}

Star::Star( const Vec3f &pos, float mass )
	: mPos( pos ), mMass( mass )
{
	mRadiusDest = powf( (3.0f * mMass )/(float)M_4_PI, 0.3333333f );
	mRadius		= mRadiusDest * 0.5f;
	mColorDest  = 0.6f;
	mColor		= mColorDest;
	mMaxRadius  = 200.0f;
	
	mNumPlanets		= 0;
	mCurrentPlanets = 0;
}

void Star::update( float dt )
{
	if( mCurrentPlanets < mNumPlanets ){
		addPlanet();
	} else if( mCurrentPlanets > mNumPlanets ){
		removePlanet();
	}
	
	if( mCurrentPlanets > 0 )
		updatePlanets( dt );
	
	mRadius		-= ( mRadius - mRadiusDest ) * 0.1f;
	mRadiusDrawn = mRadius * 0.9f;
	float radiusPer = mRadius/mMaxRadius;
	mRadiusMulti = lerp( 0.2f, 4.5f, radiusPer * radiusPer );
	mColor		-= ( mColor - mColorDest ) * 0.1;
}

void Star::updatePlanets( float dt )
{
	for( std::vector<Planet>::iterator it = mPlanets.begin(); it != mPlanets.end(); ++it ){
		it->update( dt );
	}
}
void Star::addPlanet()
{
	float orbitRadius = mCurrentPlanets * mCurrentPlanets * Rand::randFloat( 100.0f, 150.0f ) + 100.0f + mRadius;
	float orbitSpeed  = 1.0f/orbitRadius * 4.0f;//Rand::randFloat( 0.001f, 0.01f );
	float radius	  = 3 + Rand::randFloat( mCurrentPlanets * 2, mCurrentPlanets * 4 );
	mPlanets.push_back( Planet( orbitRadius, orbitSpeed, radius ) );
	mCurrentPlanets ++;
}

void Star::removePlanet()
{
	mPlanets.pop_back();
	mCurrentPlanets --;
}

void Star::draw()
{

}

void Star::drawOrbitRings()
{
	if( mCurrentPlanets > 0 ){
		gl::pushModelView();
		gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
		for( vector<Planet>::iterator it = mPlanets.begin(); it != mPlanets.end(); ++it ){
			gl::drawStrokedCircle( Vec2f::zero(), it->mOrbitRadius );
		}
		gl::popModelView();
	}
}

void Star::drawPlanets( gl::GlslProg *shader )
{	
	if( mCurrentPlanets > 0 ){
//		gl::pushModelView();
//		gl::rotate( Vec3f( 87.0f, 8.0f, 0.0f ) );
		for( vector<Planet>::iterator it = mPlanets.begin(); it != mPlanets.end(); ++it ){
			shader->uniform( "planetColor", it->mColor );
			it->draw();
		}
//		gl::popModelView();
	}
}

void Star::resetRadius()
{
	mRadius = mRadiusDest * 0.5f;
}

void Star::randRadius()
{
	mRadiusDest = Rand::randFloat( 50.0f, 100.0f );
	mMass		= ( (mRadiusDest * mRadiusDest * mRadiusDest ) * (float)M_4_PI ) * 0.33333f;
}

void Star::adjustRadius( float amt )
{
	mRadiusDest = constrain( mRadiusDest + amt, 1.0f, mMaxRadius );
}

void Star::setRadius( float amt )
{
	mRadiusDest = constrain( amt, 1.0f, mMaxRadius );
}

void Star::setColor( float amt )
{
	mColorDest = amt;
}

void Star::setNumPlanets( int amt )
{
	mPlanets.clear();
	mCurrentPlanets = 0;
	mNumPlanets = amt;
}

