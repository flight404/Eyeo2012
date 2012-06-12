//
//  Particle.h
//  Forces
//
//  Created by Robert Hodgin on 5/14/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/Camera.h"
#include "cinder/TriMesh.h"
#include "Shard.h"
#include <vector>

class Particle {
  public:
	Particle();
	Particle( const ci::Vec3f &pos, float charge );
	void update( const ci::Camera &cam, float dt );
	void draw();
	void addShards( int amt );
	
	ci::Vec3f	mPos;
	ci::Vec3f	mVel;
	ci::Vec3f	mAcc;
	float		mForce;	
	float		mColor;
	float		mRadius;
	float		mShellRadius;
	float		mCharge;
	
	ci::Matrix44f mMatrix;

	ci::Vec2f	mScreenPos;
	float		mScreenRadius;
	
	std::vector<Shard>	mShards;
	ci::TriMesh			mShardMesh;
};