//
//  GlowCube.h
//  BubbleChamber
//
//  Created by Robert Hodgin on 5/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/Vector.h"

class GlowCube {
public:
	GlowCube();
	GlowCube( const ci::Vec3f &pos );
	void update( float dt );
	void draw();
	
	ci::Vec3f		mPos;
	ci::Vec3f		mVel;
	ci::Vec3f		mAcc;
	
	ci::Vec3f		mAxis;
	float			mAngle, mAngleVel;
	float			mRadius;
	
	ci::Matrix44f	mMatrix;
	
	float			mAge;
	float			mAgePer;
	float			mLifespan;
	bool			mIsDead;
};