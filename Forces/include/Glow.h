//
//  Glow.h
//  Collider
//
//  Created by Robert Hodgin on 3/6/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/Vector.h"
#include <vector>

class Glow {
public:
	
	Glow();
	Glow( const ci::Vec3f &pos, const ci::Vec3f &vel, float radius, float charge, float lifespan );
	void update( float dt );
	void draw( const ci::Vec3f &right, const ci::Vec3f &up );
	
	ci::Vec3f	mPos;
	ci::Vec3f	mVel;
	float		mCharge;
	float		mColor;
	float		mRot, mRotVel;
	float		mRadius, mRadiusDest;
	float		mAge, mAgePer, mLifespan;
	bool		mIsDead;
};