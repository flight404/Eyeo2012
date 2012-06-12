//
//  Bait.h
//  Flocking
//
//  Created by Robert Hodgin on 4/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"

class Lantern {
  public:
	Lantern();
	Lantern( const ci::Vec3f &pos );
	void update( float dt, float yFloor );
	void draw();
	
	ci::Vec3f	mPos;
	float		mRadius;
	float		mRadiusDest;
	float		mFallSpeed;
	ci::Color	mColor;
	
	float		mVisiblePer;
	
	bool		mIsDead;
	bool		mIsSinking;
	bool		mIsDying;
};