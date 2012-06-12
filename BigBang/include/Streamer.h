//
//  Confetti.h
//  BigBang
//
//  Created by Robert Hodgin on 5/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/Perlin.h"

class Streamer {
  public:
	Streamer();
	Streamer( const ci::Vec3f &pos, int presetIndex );
	void update( const ci::Vec3f &roomDims, float dt, bool tick );
	void checkBounds( const ci::Vec3f &roomDims );
	void draw();
	
	ci::Vec3f	mPos;
	int			mLen;
	std::vector<ci::Vec3f>	mPositions;
	std::vector<ci::Vec3f>	mVelocities;
	std::vector<ci::Vec3f>	mAccels;
	ci::Vec3f	mVel;
	ci::Vec3f	mAcc;
	
	float		mGravity;
	
	float		mRadius;
	ci::Color	mColor;
	
	int			mFrameCount;
	float		mAge;
	float		mAgePer;
	float		mLifespan;
	bool		mIsDead;
};