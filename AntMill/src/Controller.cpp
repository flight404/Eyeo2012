//
//  Controller.cpp
//  BubbleChamber
//
//  Created by Robert Hodgin on 5/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "Controller.h"

#define K 0.15
#define DAMPING 0.6
#define REST_LENGTH 0.0

using namespace ci;
using std::vector;
using std::list;

Controller::Controller(){}

Controller::Controller( Room *room )
	: mRoom( room )
{
}

void Controller::init( int maxAnts, int numSpecialAnts )
{
	std::cout << "SIMULATION INITIALIZED WITH " << maxAnts << " ANTS AND " << numSpecialAnts << " OF THEM ARE SPECIAL." << std::endl;
	
	mFoods.clear();
	mAnts.clear();
	
	mNumSpecialAnts			= numSpecialAnts;
	
	mHomePos				= Vec3f( 0.0f, mRoom->getFloorLevel(), 0.0f );
	mHomeRadius				= 55.0f;
	mHomeRadiusSqrd			= mHomeRadius * mHomeRadius;
	
	mFoodPos				= Vec3f( 200.0f, mRoom->getFloorLevel(),-200.0f );//200.0
	mFoodRadius				= 25.0f;
	mFoodRadiusSqrd			= mFoodRadius * mFoodRadius;

	int numFoods = 200;
	for( int i=0; i<numFoods; i++ ){
		Vec3f pos = mFoodPos + Rand::randVec3f() * Vec3f( 20.0f, 0.0f, 20.0f );
		mFoods.push_back( Food( pos,-1.0f ) );
	}
	
	int specialCounter = 0;
	for( int i=0; i<maxAnts; i++ ){
		Vec3f pos = Rand::randVec3f() * mHomeRadius;
		pos.y = mRoom->getFloorLevel();
		mAnts.push_back( Ant( this, pos ) );
		
		if( specialCounter < mNumSpecialAnts ){
			mAnts.back().mIsSpecial = true;
		}
		
		specialCounter ++;
	}
}

void Controller::repelAnts()
{
	float zoneRadiusSqrd = 125.0f;

	for( vector<Ant>::iterator p1 = mAnts.begin(); p1 != mAnts.end(); ++p1 ){
		
		vector<Ant>::iterator p2 = p1;
		for( ++p2; p2 != mAnts.end(); ++p2 ) {
			Vec3f dir = p1->mPos - p2->mPos;
			float distSqrd = dir.lengthSquared();
			
			if( distSqrd < zoneRadiusSqrd && distSqrd > 25.0f ){
				float F = ( zoneRadiusSqrd/ ( distSqrd + 10.0f ) );
				dir = dir.normalized() * F * 0.025f;
				
				p1->mAcc += dir;
				p2->mAcc -= dir;
			}
		}
	}
}

void Controller::update( float dt, bool step )
{
	repelAnts();
	mSpecialAnts.clear();
	updateAnts( dt );
	
	sort( mSpecialAnts.begin(), mSpecialAnts.end(), depthSortFunc );
}

void Controller::updateAnts( float dt )
{
	for( vector<Ant>::iterator it = mAnts.begin(); it != mAnts.end(); ++it )
	{
		it->update( dt, mRoom->getDims() );
		
		if( !it->mHasFood ){
			Vec3f dirToFood			= it->mPos - mFoodPos;
			float distToFoodSqrd	= dirToFood.lengthSquared();
		
			if( distToFoodSqrd < mFoodRadiusSqrd ){
				pickupFood( &(*it) );
			}
			
		} else {
			Vec3f dirToHome			= it->mPos - mHomePos;
			float distToHomeSqrd	= dirToHome.lengthSquared();
			
			if( distToHomeSqrd < mHomeRadiusSqrd && Rand::randFloat() < 0.02f ){
				dropFood( &(*it) );
			}
		}
		
		if( it->mIsSpecial ){
			mSpecialAnts.push_back( &(*it) );
		}
	}
}

void Controller::pickupFood( Ant *ant )
{
	for( vector<Food>::iterator it = mFoods.begin(); it != mFoods.end(); ++it ){
		Vec3f dirToFood = ant->mPos - it->mPos;
		float distToFood = dirToFood.length();
		
		if( distToFood < 5.0f && !ant->mHasFood && !it->mGrabbed){
			ant->foundFood( &(*it) );
			it->mGrabbed = true;
		}
	}
}

void Controller::dropFood( Ant *ant )
{
	ant->mGrabbedFood = NULL;
	ant->mHasFood = false;
}

void Controller::draw()
{
}

void Controller::drawAnts()
{
	glBegin( GL_LINES );
	for( vector<Ant>::iterator it = mAnts.begin(); it != mAnts.end(); ){
		it->draw();
		++it;
	}
	glEnd();
}

void Controller::drawAntTails( const Color &outBound, const Color &inBound )
{
	glPointSize( 2.0f );
	glBegin( GL_POINTS );
	for( vector<Ant>::iterator it = mAnts.begin(); it != mAnts.end(); ++it ){
		if( it->mHasFood ){
			gl::color( inBound );
		} else {
			gl::color( outBound );
		}
		gl::vertex( it->mTailPos );
	}
	glEnd();
}

void Controller::drawFoods()
{
	for( vector<Food>::iterator it = mFoods.begin(); it != mFoods.end(); ){
		gl::color( it->mColor );
		it->draw();
		++it;
	}
}

void Controller::drawHome()
{
	gl::drawBillboard( mHomePos, Vec2f( mHomeRadius, mHomeRadius ) * 2.0f, 0.0f, Vec3f::xAxis(), Vec3f::zAxis() );
}

bool depthSortFunc( Ant *a, Ant *b ){
	return a->mPos.z > b->mPos.z;
}


