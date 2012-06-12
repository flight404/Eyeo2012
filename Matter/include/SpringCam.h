//
//  SpringCam.h
//  Matter
//
//  Created by Robert Hodgin on 3/28/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#define SPRING_STRENGTH 0.02
#define SPRING_DAMPING 0.05
#define REST_LENGTH 0.0

#include "cinder/Camera.h"
#include "cinder/Vector.h"

class SpringCam {
  public:
	struct SpringNode {
		SpringNode() {}
		SpringNode( ci::Vec3f pos ){
			mRestPos	= pos;
			mPos		= pos;
			mVel		= ci::Vec3f::zero();
			mAcc		= ci::Vec3f::zero();
		}
		
		void apply(){
			ci::Vec3f dir	= mPos - mRestPos;
			float dist		= dir.length();
			dir.safeNormalize();
			float springForce	= -( dist - REST_LENGTH ) * SPRING_STRENGTH;
			float dampingForce	= -SPRING_DAMPING * ( dir.x*mVel.x + dir.y*mVel.y + dir.z*mVel.z );
			float r				= springForce + dampingForce;
			dir *= r;
			mAcc += dir;
		}
		
		void update( float timeDelta ){
			mVel += mAcc * timeDelta;
			mPos += mVel * timeDelta;
			mVel -= mVel * 0.04 * timeDelta;
			mAcc = ci::Vec3f::zero();
		}

		ci::Vec3f mRestPos;
		ci::Vec3f mPos, mVel, mAcc;
	};
	
	
	
	SpringCam();
	SpringCam( float camDist, float aspectRatio );
	void apply( float timeDelta );
	void update( float timeDelta );
	void dragCam( const ci::Vec2f &posOffset, float distFromCenter );
	void draw();
	void setFov( float amt ){ mFovDest = amt; }
	
	ci::CameraPersp getCam(){ return mCam; }
	
	ci::CameraPersp		mCam;
	float				mCamDist;
	ci::Vec3f			mEye, mCenter, mUp;
	ci::Vec3f			mRestPos;
	float				mFov, mFovDest;
	ci::Vec3f			mVel;
	ci::Vec3f			mAcc;
	
	ci::Matrix44f		mMvpMatrix;
	
	SpringNode			mEyeNode;
	SpringNode			mCenNode;
	SpringNode			mUpNode;
};