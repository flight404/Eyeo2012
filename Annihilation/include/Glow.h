//
//  Glow.h
//  Collider
//
//  Created by Robert Hodgin on 3/6/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/Vector.h"

class Glow {
public:
	
	Glow();
	Glow( const ci::Vec3f &pos, const ci::Vec3f &vel, float radius, float lifespan, const ci::Vec3f &right, const ci::Vec3f &up );
	void update( float timeDelta );
	void draw();
	
	ci::Vec3f	mPos;
	ci::Vec3f	mVel;
	float		mRot;
	float		mRadius;
	float		mAge, mAgePer, mLifespan;
	bool		mIsDead;
	
	ci::Vec3f	mRight;
	ci::Vec3f	mUp;
};