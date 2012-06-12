//
//  Confetti.h
//  BigBang
//
//  Created by Robert Hodgin on 5/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/Camera.h"

class Balloon {
  public:
	Balloon();
	Balloon( const ci::Vec3f &pos, int presetIndex );
	void update( const ci::Camera &cam, const ci::Vec3f &roomDims, float dt );
	void updateSpring( float dt );
	void checkBounds( const ci::Vec3f &roomDims );
	void draw();
	
	ci::Vec3f	mPos;
	ci::Vec3f	mVel;
	ci::Vec3f	mAcc;
	
	ci::Vec3f	mSpringPos;
	ci::Vec3f	mSpringVel;
	ci::Vec3f	mSpringAcc;
	
	ci::Vec2f	mScreenPos;
	float		mScreenRadius;
	
	ci::Matrix44f	mMatrix;
	
	float		mBuoyancy;
	float		mRadius;
	ci::Color	mColor;
	
	float		mAge;
	float		mLifespan;
	bool		mIsDead;
};