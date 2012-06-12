//
//  Shockwave.h
//  Collider
//
//  Created by Robert Hodgin on 3/6/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/Color.h"
#include "cinder/Vector.h"
#include "cinder/Matrix.h"

class Shockwave {
  public:
	Shockwave();
	Shockwave( const ci::Vec3f &pos, float lifespan, float speed );
	void update( float timeDelta );
	void draw();
	
	ci::Matrix44f	mMatrix;
	
	ci::Vec3f	mPos;
	float		mRadius, mRadiusPrev;
	float		mStrength;
	float		mImpulse;
	
	ci::Color	mColor;
	
	float		mAge, mAgePer;
	float		mSpeed;
	float		mLifespan;
	bool		mIsDead;
	
};