//
//  Glow.h
//  Collider
//
//  Created by Robert Hodgin on 3/6/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/Vector.h"

class Smoke {
public:
	
	Smoke();
	Smoke( const ci::Vec3f &pos, const ci::Vec3f &vel, float radius, float lifespan );
	void update( float timeDelta );
	void draw( const ci::Vec3f &right, const ci::Vec3f &up );
	
	ci::Vec3f	mPos;
	ci::Vec3f	mVel;
	float		mRot, mRotVel;
	float		mRadius, mRadiusVel;
	float		mAge, mAgePer, mLifespan;
	bool		mIsDead;
};