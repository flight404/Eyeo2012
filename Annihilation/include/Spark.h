//
//  Spark.h
//  Attraction
//
//  Created by Robert Hodgin on 4/24/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"

class Spark {
public:
	Spark();
	Spark( ci::Vec3f pos, ci::Vec3f vel );
	void update( float timeDelta );
	void draw();
	
	// properties
	ci::Vec3f	mPos;
	ci::Vec3f	mVel;
	float		mAge, mLifespan;
	float		mAgePer;
	
	// state
	bool		mIsDead;
};