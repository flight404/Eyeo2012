//
//  Glob.h
//  Forces
//
//  Created by Robert Hodgin on 5/24/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

class Glob {
public:
	Glob();
	Glob( const ci::Vec3f &pos, const ci::Vec3f &vel, float radius, float lifespan );
	
	void update( float floorLevel, float dt );
	void draw( const ci::Vec3f &right, const ci::Vec3f &up );
	
	ci::Vec3f	mPos, mVel;
	float		mColor;
	float		mRadius;
	float		mAge, mAgePer;
	float		mLifespan;
	bool		mIsDead;
	
	bool		mBounced;
	
};
