//
//  Antimatter.h
//  Attraction
//
//  Created by Robert Hodgin on 4/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "Room.h"
#include <vector>
#include <list>

class Antimatter {
  public:
	Antimatter();
	Antimatter( ci::Vec3f po );
	void update( Room *room, float t );
	void draw( ci::gl::VboMesh &sphereHi );
	void drawSprings();
	
	ci::Vec3f	mPos;
	ci::Color	mCol;
	
	float		mCharge;
	float		mMass, mInvMass;
	float		mRadius;
	
	bool		mIsDead;
};