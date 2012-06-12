#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include <vector>

class Photon {
public:
	Photon();
	Photon( ci::Vec3f pos, ci::Vec3f vel );
	void update( float t );
	void draw();
	void drawTail();
	
	int			mLen;
	std::vector<ci::Vec3f> mPs;
	
	ci::Vec3f	mPos;
	ci::Vec3f	mVel;
	
	float		mRadius;
	
	float		mAge, mAgePer;
	float		mLifespan;
	bool		mIsDead;
};