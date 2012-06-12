//
//  SpringCam.cpp
//  PROTOTYPE

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
	
	mCam.setPerspective( 65.0f, aspectRatio, 5.0f, 3000.0f );
}

void SpringCam::update( float dt )
{	
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