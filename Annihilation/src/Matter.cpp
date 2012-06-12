//
//  Matter.cpp
//  Attraction
//
//  Created by Robert Hodgin on 4/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Matter.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;

Matter::Matter()
{
}

Matter::Matter( Vec3f pos )
{
	mPos			= pos;

	mCharge			= 80.0f;
	mCol			= Color( 0.0f, 0.0f, 0.0f );

	mRadius			= 60.0f;
	mMass			= 816000.0f;
	
	mIsDead			= false;
}


void Matter::update( Room *room, float t )
{	

}

void Matter::draw( gl::VboMesh &sphereHi )
{	
	gl::color( mCol );
	gl::draw( sphereHi );
}

