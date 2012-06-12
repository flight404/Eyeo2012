//
//  Antimatter.cpp
//  Attraction
//
//  Created by Robert Hodgin on 4/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Antimatter.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

#define SPRING_STRENGTH 0.1
#define SPRING_DAMPING 0.2

using namespace ci;
using namespace std;
using std::vector;
using std::list;

Antimatter::Antimatter()
{
}

Antimatter::Antimatter( Vec3f pos )
{
	mPos			= pos;
	
	mCharge			= -10.0f;
	mCol			= Color( 1.0f, 1.0f, 1.0f );
	
	mRadius			= 10.0f;
	
	mMass			= 3700.0f;
	
	mIsDead			= false;
}


void Antimatter::update( Room *room, float t )
{	

}

void Antimatter::draw( gl::VboMesh &sphereHi )
{	
	gl::color( mCol );
	gl::draw( sphereHi );
}

void Antimatter::drawSprings()
{
	gl::color( Color( 0.0f, 0.0f, 0.0f ) );

}

