//
//  Bubble.h
//  BubbleChamber
//
//  Created by Robert Hodgin on 5/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/Vector.h"
#include "Room.h"

class Bubble {
public:
	Bubble();
	Bubble( const ci::Vec3f &pos, const ci::Vec3f &vec, float age );
	void init( const ci::Vec3f &pos, const ci::Vec3f &vec, float lifespan );
	void update( Room *room, float dt );
	void draw();
	
	ci::Vec3f		mPos;
	ci::Vec3f		mVel;
	ci::Vec3f		mAcc;
	
	float			mAge, mAgePer;
	float			mLifespan;
	
	bool			mIsDead;
	
	static ci::Vec3f mBuoyancy;
};