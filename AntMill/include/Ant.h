//
//  Ant.h
//  AntMill
//
//  Created by Robert Hodgin on 5/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once
#include <vector>
#include "Food.h"

class Controller;
class Food;

class Ant 
{
  public:
	Ant();
	Ant( Controller *controller, const ci::Vec3f &pos );
	void update( float dt, const ci::Vec3f &roomDims );
	void updateSpring( float dt );
	void applySpeedLimit();
	void applyJitter();
	void updateSensors();
	void updateParticles();
	void checkForPause();
	void checkForHome();
	void foundFood( Food *food );
	void stayInBounds( ci::Vec3f *v, const ci::Vec3f &roomDims );
	void findAntsInZone();
	void followAntsInZone();
	void draw();
	void turn( float amt );
	

	// PIN POINTS
	ci::Vec3f			mPinPerp;
	ci::Vec3f			mPinUp;
	
	ci::Vec3f			mAnchorPos;
	ci::Vec3f			mSpringPos;
	ci::Vec3f			mSpringVel;
	ci::Vec3f			mSpringAcc;
	
	Controller			*mController;
	static uint32_t		sNextUniqueId;
	uint32_t			mId;
	ci::Vec3f			mPos;
	ci::Vec3f			mVel;
	ci::Vec3f			mAcc;
	
	float				mAngle;
	ci::Vec2f			mOrientation;
	
	ci::Vec3f			mDir, mDirPerp;
	ci::Vec3f			mLeftSensorPos, mRightSensorPos;
	ci::Vec3f			mTailPos;
	
	Food				*mGrabbedFood;
	
	std::vector<Ant*>	mVisibleAnts;
	
	float				mLength;
	float				mRadius;
	float				mColor;
	float				mMaxSpeed;
	
	bool				mHasFood;
	bool				mIsHome;
	
	bool				mIsSpecial;
	float				mAge;
	bool				mIsInjured;
	bool				mIsDead;
	
	static float		sCenterOffsetMulti;
	static float		sCenterRadiusMulti;
};

