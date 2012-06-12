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

class Shockwave {
  public:
	Shockwave();
	Shockwave( const ci::Vec3f &pos, const ci::Vec3f &axis, float lifespan, float speed );
	void update( float timeDelta );

	ci::Vec3f	mPos;
	ci::Vec3f	mAxis;
	float		mRadius, mRadiusPrev;
	float		mRot, mRotVel;
	float		mStrength;
	float		mImpulse;
	float		mShell, mShellDest;
	float		mAge, mAgePer;
	float		mSpeed;
	float		mLifespan;
	ci::Color	mColor;
	bool		mIsDead;
	
};