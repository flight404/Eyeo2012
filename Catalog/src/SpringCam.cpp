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
	
	mCam.setPerspective( 65.0f, aspectRatio, 5.0f, 200000.0f );
}

void SpringCam::update( float timeDelta )
{	
	mEyeNode.apply();
	mCenNode.apply();
	
	mEyeNode.update( timeDelta );
	mCenNode.update( timeDelta );
	
	Vec3f dir		= mCenNode.mPos - mEyeNode.mPos;
	Vec3f dirNorm	= dir.normalized();
//	Vec3f up		= dirNorm.cross( Vec3f::xAxis() );
//	up.normalize();
//	Vec3f temp		= dirNorm.cross( up );
//	temp.normalize();
//	up = dirNorm.cross( temp );
//	up.normalize();
	
	Vec3f newEyePos = mEyeNode.mPos + dirNorm * 10.0f;

	mCam.lookAt( newEyePos, mCenNode.mPos, mUpNode.mPos );
	mMvpMatrix = mCam.getProjectionMatrix() * mCam.getModelViewMatrix();
}

void SpringCam::dragCam( const Vec2f &posOffset, float distFromCenter )
{
	mEyeNode.mAcc += Vec3f( posOffset.xy(), distFromCenter );
}

void SpringCam::setEye( const Vec3f &v )
{
	mEyeNode.mRestPos = v;
}

void SpringCam::setCenter( const Vec3f &v )
{
	mCenNode.mRestPos = v;
}