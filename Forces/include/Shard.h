//
//  Shard.h
//  Sol
//
//  Created by Robert Hodgin on 12/10/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/TriMesh.h"

#include <vector>

class Shard {
public:
	Shard( ci::Vec3f pos, ci::Vec3f dir, float height, float radius );
	void init( ci::TriMesh *mesh, const ci::Color &c );
	ci::Vec3f calcSurfaceNormal( const ci::Vec3f &p1, const ci::Vec3f &p2, const ci::Vec3f &p3 );
	void update();
	void draw();

	ci::Vec3f	mPos;
	ci::Vec3f	mDir;
	ci::Vec3f	mTip;	// tip of shard
	ci::Vec3f	mMid;	// where tapering facets begin

	std::vector<ci::Vec3f> mBasePs;
	std::vector<ci::Vec3f> mMidPs;
	std::vector<ci::Vec3f> mTipPs;

	float		mHeight;
	float		mRadius;
};
