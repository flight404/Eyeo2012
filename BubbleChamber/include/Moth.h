//
//  Moth.h
//  BubbleChamber
//
//  Created by Robert Hodgin on 5/7/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/Vector.h"
#include "Room.h"

class Moth {
public:
	Moth();
	Moth( const ci::Vec3f &pos );
	void update( Room *room, float dt );
	void checkBoundary( Room *room );
	void draw();
	
	ci::Vec3f		mPos;
	ci::Vec3f		mVel;
	ci::Vec3f		mAcc;
	
	ci::Vec3f		mAxis;

	float			mColorf;
	
	float			mAge;
	bool			mIsDead;
};