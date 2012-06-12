//
//  FieldLine.h
//  Forces
//
//  Created by Robert Hodgin on 5/14/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/Vector.h"
#include <vector>

class FieldLine {
public:
	
	FieldLine();
	FieldLine( const ci::Vec3f &pos, const ci::Vec3f &vel, float charge, float lifespan );
	void update( float dt, bool tick );
	void draw();
	
	ci::Vec3f	mPos;
	ci::Vec3f	mVel;
	
	std::vector<ci::Vec3f>	mPositions;
//	std::vector<ci::Vec3f>	mNormals;
//	std::vector<ci::Vec3f>	mPerps;
	
	float		mMaxSpeed;
	float		mCharge;
	float		mAge, mAgePer, mLifespan;
	float		mColor;
	bool		mIsDead;
	
	static int		sLen;
	static float	sInvLen;
	static int		sNumVerts;
};