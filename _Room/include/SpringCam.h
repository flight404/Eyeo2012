//
//  SpringCam.h
//  PROTOTYPE

#pragma once

#define SPRING_STRENGTH 0.02
#define SPRING_DAMPING 0.25
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
		
		void update( float dt ){
			ci::Vec3f dir		= mPos - mRestPos;
			float dist			= dir.length();
			dir.safeNormalize();
			float springForce	= -( dist - REST_LENGTH ) * SPRING_STRENGTH;
			float dampingForce	= -SPRING_DAMPING * ( dir.x*mVel.x + dir.y*mVel.y + dir.z*mVel.z );
			float r				= springForce + dampingForce;
			dir *= r;
			mAcc += dir;
			
			mVel += mAcc * dt;
			mPos += mVel * dt;
			mVel -= mVel * 0.04 * dt;
			mAcc = ci::Vec3f::zero();
		}
		
		void setPos( const ci::Vec3f &v ){ mRestPos = v; }

		ci::Vec3f mRestPos;
		ci::Vec3f mPos, mVel, mAcc;
	};
	
	
	
	SpringCam();
	SpringCam( float camDist, float aspectRatio );
	void update( float dt );
	void dragCam( const ci::Vec2f &posOffset, float distFromCenter );
	void draw();
	ci::CameraPersp getCam(){ return mCam; }
	ci::Vec3f getEye(){ return mCam.getEyePoint(); }
	void setEye( const ci::Vec3f &eye );
	void resetEye();
	
	ci::CameraPersp		mCam;
	float				mCamDist;
	ci::Vec3f			mEye, mCenter, mUp;
	
	ci::Matrix44f		mMvpMatrix;
	
	SpringNode			mEyeNode;
	SpringNode			mCenNode;
	SpringNode			mUpNode;
};