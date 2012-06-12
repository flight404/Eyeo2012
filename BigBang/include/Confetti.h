//
//  Confetti.h
//  BigBang
//
//  Created by Robert Hodgin on 5/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once


class Confetti {
  public:
	Confetti();
	Confetti( const ci::Vec3f &pos, float speedMulti, int presetIndex );
	void update( const ci::Vec3f &roomDims, float dt );
	void checkBounds( const ci::Vec3f &roomDims );
	void draw();
	
	ci::Vec3f		mPos;
	ci::Vec3f		mVel;
	ci::Vec3f		mDriftVel;
	ci::Vec3f		mAcc;
	
	float			mRadius;
	ci::Color		mColor;
	float			mXRot, mYRot, mZRot;
	float			mGravity;
	
	ci::Matrix44f	mMatrix;
	
	float			mAge;
	float			mAgePer;
	float			mLifespan;
	bool			mIsDead;
	bool			mHasLanded;
};