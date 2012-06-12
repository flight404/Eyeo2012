//
//  Confetti.cpp
//  BigBang
//
//  Created by Robert Hodgin on 5/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "Node.h"

#define SPRING_STRENGTH 0.05
#define SPRING_DAMPING 0.1
#define REST_LENGTH 0.0

using namespace ci;

Node::Node(){}

Node::Node( const Vec3f &pos, const Vec3f &vel )
	: mPos( pos ), mVel( vel )
{	
	mPosInit	= mPos;
	mRadius		= 1.3f;
	mColor		= Color( 0.0f, 0.0f, 0.0f );
	mAcc		= Vec3f::zero();
	
	mMatrix.setToIdentity();
	
	mAge = 0.0f;
}

void Node::update( float dt )
{
//	std::cout << "acc = " << mAcc << std::endl;
	mVel += mAcc;
	mPos += mVel * dt;
	mVel -= mVel * 0.015 * dt;
	mAcc = ci::Vec3f::zero();
	
	mColor.r = mVel.length();
	
	mMatrix.setToIdentity();
	mMatrix.translate( mPos );
//	mMatrix.rotate( tiltAxis, 2.0f );
	mMatrix.scale( Vec3f( mRadius, mRadius, mRadius ) );
	
	mAge += dt;
}

void Node::draw()
{
	gl::drawCube( Vec3f::zero(), Vec3f( 2.0f, 2.0f, 2.0f ) );
}