#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "Room.h"
#include <vector>

class Particle {
public:
	Particle();
	Particle( const ci::Vec3f &pos, const ci::Vec3f &vel );
	Particle( const ci::Vec3f &pos, const ci::Vec3f &vel, float charge );
	void init( const ci::Vec3f &pos, const ci::Vec3f &vel, float charge );
	float getMassFromRadius( float r );
	void update( Room *room, float t );
	void draw( ci::gl::VboMesh &sphereLo, ci::gl::VboMesh &sphereHi );
	void finish();

	ci::Vec3f	mPos;
	ci::Vec3f	mVel;
	ci::Vec3f	mAcc;
	
	float		mDistPer;
	float		mDistToClosestNeighbor;
	
	float		mFusionThresh;
	
	ci::Color	mCol;
	
	float		mCharge;
	float		mMass, mInvMass;
	float		mDecay;
	float		mRadius, mRadiusInit, mRadiusDest;
	
	bool		mIsDead;
};