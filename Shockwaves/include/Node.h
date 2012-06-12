//
//  Confetti.h
//  BigBang
//
//  Created by Robert Hodgin on 5/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once


class Node {
  public:
	Node();
	Node( const ci::Vec3f &pos, const ci::Vec3f &vel );
	void update( float dt );
	void draw();
	
	ci::Matrix44f	mMatrix;
	ci::Vec3f		mPosInit;
	ci::Vec3f		mPos;
	ci::Vec3f		mVel;
	ci::Vec3f		mAcc;
	
	float			mRadius;
	ci::Color		mColor;
	
	float			mAge;
};