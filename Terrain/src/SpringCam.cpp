//
//  SpringCam.cpp
//  Matter
//
//  Created by Robert Hodgin on 3/28/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "SpringCam.h"

using namespace ci;

SpringCam::SpringCam()
{
}

SpringCam::SpringCam( float camDist, float aspectRatio )
{
	mCamDist		= camDist;
	
	mEye			= Vec3f( 0.0f, 0.0f, mCamDist );
	mCenter			= Vec3f( 0.0f, 0.0f, 0.0f );
	mUp				= Vec3f::yAxis();
	
	mEyeNode		= SpringNode( mEye );
	mCenNode		= SpringNode( mCenter );
	mUpNode			= SpringNode( mUp );
	
	mFov			= 65.0f;
	
	mCam.setPerspective( mFov, aspectRatio, 1.0f, 3000.0f );
}

void SpringCam::update( float power, float dt )
{	
	if( power > 0.5f ){
		mFov -= ( mFov - 30.0f ) * 0.025f;
	} else {
		mFov -= ( mFov - 60.0f ) * 0.025f;		
	}
	mCam.setFov( mFov );
	
	mEyeNode.update( dt );
	mCenNode.update( dt );
	
	mCam.lookAt( mEyeNode.mPos, mCenNode.mPos, mUpNode.mPos );
	mMvpMatrix = mCam.getProjectionMatrix() * mCam.getModelViewMatrix();
}

void SpringCam::dragCam( const Vec2f &posOffset, float distFromCenter )
{
	mEyeNode.mAcc += Vec3f( posOffset.xy(), distFromCenter );
}

void SpringCam::setEye( const ci::Vec3f &eye )
{
	mEyeNode.setPos( eye );
}

void SpringCam::resetEye()
{
	mEyeNode.setPos( Vec3f( 0.0f, 0.0f, mCamDist ) );
}

void SpringCam::setPreset( int i )
{
	if( i == 0 ){
		mEyeNode.setPos( Vec3f( 0.0f, 0.0f, mCamDist ) );
		mCenNode.setPos( Vec3f( 0.0f, -100.0f, 0.0f ) );
	} else if( i == 1 ){
		mEyeNode.setPos( Vec3f( mCamDist * 0.4f, -175.0f, -100.0f ) );
		mCenNode.setPos( Vec3f( 0.0f, -190.0f, 0.0f ) );
	} else if( i == 2 ){
		mEyeNode.setPos( Vec3f( -174.0f, -97.8f, -20.0f ) );
		mCenNode.setPos( Vec3f( 0.0f, -190.0f, 0.0f ) );
	}
}