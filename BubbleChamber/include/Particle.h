//
//  Particle.h
//  BubbleChamber
//
//  Created by Robert Hodgin on 5/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/Vector.h"
#include "cinder/Ray.h"
#include "Room.h"
#include <vector>

class Controller;

class Particle {
  public:
	Particle();
	Particle( Controller *controller, int mGen, const ci::Vec3f &pos, const ci::Vec3f &dir, float speed, int len );
	void update( Room *room, float dt, bool tick );
	void draw();
	bool isOutOfBounds( Room *room );
	
	Controller		*mController;
	
	int				mGen;
	
	ci::Ray			mRay;
	
	int				mLen;
	std::vector<ci::Vec3f>	mPs;
	ci::Vec3f		mPos;
	ci::Vec3f		mVel;
	ci::Vec3f		mDeathAxis;
	float			mSpeed;
	
	float			mCharge;
	float			mAngle;
	float			mAngleDelta;
	float			mAngleDeltaDelta;
	
	ci::Vec3f		mAxis, mPerp1, mPerp2;
	
	ci::Vec3f		mDeathPos;
	ci::Vec3f		mPosLastTick;

	float			mAge, mAgePer;
	float			mLifespan;
	
	bool			mBounced;
	bool			mIsDying;
	float			mDyingCount;
	bool			mIsDead;
	bool			mIntersectsFloor;
};