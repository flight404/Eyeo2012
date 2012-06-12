//
//  Food.h
//  AntMill
//
//  Created by Robert Hodgin on 5/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

class Food 
{
  public:
	Food();
	Food( const ci::Vec3f &pos, float charge );
	void update( float dt, const ci::Vec3f &roomDims );
	void draw();
	void stayInBounds( ci::Vec3f *v, const ci::Vec3f &roomDims );
	
	
	ci::Vec3f	mPos;
	ci::Vec3f	mVel;
	float		mCharge;
	ci::Color	mColor;
	bool		mGrabbed;
	bool		mIsGone;
};