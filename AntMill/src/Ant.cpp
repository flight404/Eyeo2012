//
//  Ant.cpp
//  AntMill
//
//  Created by Robert Hodgin on 5/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/gl/Gl.h"
#include "cinder/Rand.h"
#include "Controller.h"
#include "Ant.h"

#define SPRING_STRENGTH 0.3
#define SPRING_DAMPING 0.025
#define REST_LENGTH 20.0


using namespace ci;
using std::vector;

uint32_t Ant::sNextUniqueId = 0;
float Ant::sCenterOffsetMulti = 2.15f;
float Ant::sCenterRadiusMulti = 2.5f;

Ant::Ant()
{
}

Ant::Ant( Controller *controller, const Vec3f &pos )
	: mController( controller ), mPos( pos )
{
	mId				= sNextUniqueId;
	sNextUniqueId ++;
	
	mIsSpecial		= false;

	mAcc			= Vec3f::zero();
	mVel			= Rand::randVec3f();
	mVel.y			= 0.0f;
	mVel.normalize();

	mAnchorPos		= mPos + Vec3f( 0.0f, 100.0f, 0.0f );
	mSpringPos		= mPos + Vec3f( 0.0f, 100.0f, 0.0f );
	mSpringVel		= mVel;
	mSpringPos		= mAcc;
	
	mDir			= mVel;
	mDirPerp		= Vec3f( -mDir.z, mDir.y, mDir.x );
	mLeftSensorPos	= Vec3f::zero();
	mRightSensorPos = Vec3f::zero();
	mLength			= 3.0f;
	mRadius			= 3.0f;
	mMaxSpeed		= 1.0f;// Rand::randFloat( 0.5f, 1.0f );
	mColor			= Rand::randFloat();
	
	mHasFood		= false;
	mIsHome			= true;
	
	mIsDead			= false;
	mIsInjured		= false;
	
	mGrabbedFood	= NULL;
}

void Ant::checkForHome()
{
	Vec3f dirToHome = mPos - mController->mHomePos;
	float distToHome = dirToHome.length();
	if( distToHome < mController->mHomeRadius && Rand::randFloat() < 0.05f ){
		mHasFood = false;
		mVel *= -1.5f;
		mPos += mVel;
	}
}

void Ant::foundFood( Food *food )
{
	mGrabbedFood = food;
	mHasFood = true;
	mVel *= -1.5f;
	mPos += mVel;
}

void Ant::update( float dt, const Vec3f &roomDims )
{
	if( !mIsDead ){
		mVel += mAcc * dt;
		applySpeedLimit();
		applyJitter();
	} else {
		mVel += Vec3f( 0.0f, -0.2f, 0.0f );
	}
	mPos += mVel * dt;
	
	if( mIsDead ){
		mVel -= mVel * 0.03f * dt;
	}
	
	mAnchorPos = mPos + Vec3f( 0.0f, 100.0f, 0.0f );
	
	if( mGrabbedFood != NULL ){
		mGrabbedFood->mPos = mPos;
	}
	
	if( mIsDead ){
		mColor = 0.0f;
	} else {
		if( mHasFood )  mColor = 0.4f;
		else			mColor = 0.15f;
	}
	
	stayInBounds( &mPos, roomDims );
	
	mDir		= mVel.safeNormalized();
	mDirPerp	= Vec3f( -mDir.z, mDir.y, mDir.x );
	
	updateSensors();
	
	
	mTailPos	= mPos - mDir * mLength;
	if( !mIsDead )
		mTailPos.y  = -roomDims.y;
	
	if( mIsSpecial ){
		updateSpring( dt );
	}
	
	mAge += dt;
	mAcc = Vec3f::zero();
}

void Ant::applySpeedLimit()
{
	float velLength = mVel.length();
	if( velLength > mMaxSpeed ){
		mVel = mVel.normalized() * mMaxSpeed;
	}
}

void Ant::applyJitter()
{
	if( !mHasFood ){
		Vec3f jitterOffset = Rand::randVec3f() * 0.2f;
		jitterOffset.y = 0.0f;
		mVel += jitterOffset;
	}
}

void Ant::stayInBounds( Vec3f *v, const Vec3f &roomDims )
{
	float boundsOffset = 1.0f;
	if( v->x < -roomDims.x ){
		v->x = -roomDims.x;
		mVel.x += boundsOffset;
	} else if( v->x > roomDims.x ){
		v->x = roomDims.x;
		mVel.x -= boundsOffset;
	}
	
	if( !mIsDead ){
		v->y = -roomDims.y;
	} else {
		if( v->y < -roomDims.y ){
			v->y = -roomDims.y;
			mVel.y *= -0.5f;
		} else if( v->y > roomDims.y ){
			v->y = roomDims.y;
			mVel.y *= -0.5f;
		}
	}
	
	if( v->z < -roomDims.z ){
		v->z = -roomDims.z;
		mVel.z += boundsOffset;
	} else if( v->z > roomDims.z ){
		v->z = roomDims.z;
		mVel.z -= boundsOffset;
	}
}

void Ant::updateSensors()
{
	float sensorLength = 3.0f;
	mLeftSensorPos	= mPos + mDir * mLength * sensorLength + mDirPerp * mLength * sensorLength ;
	mRightSensorPos = mPos + mDir * mLength * sensorLength - mDirPerp * mLength * sensorLength ;
}


void Ant::updateSpring( float dt )
{
	ci::Vec3f dir		= mSpringPos - mAnchorPos;
	float dist			= dir.length();
	dir.safeNormalize();
	float springForce	= -( dist - REST_LENGTH ) * SPRING_STRENGTH;
	float dampingForce	= -SPRING_DAMPING * ( dir.x*mSpringVel.x + dir.y*mSpringVel.y + dir.z*mSpringVel.z );
	float r				= springForce + dampingForce;
	dir *= r;
	mSpringAcc += dir;
	
	mSpringVel += mSpringAcc;
	mSpringPos += mSpringVel;
	mSpringVel -= mSpringVel * 0.04;
	mSpringAcc = ci::Vec3f::zero();
	
	mPinUp = mSpringPos - mPos;
	mPinUp.normalize();
	
	mPinPerp	-= ( mPinPerp - mDir.cross( mPinUp ) ) * 0.2f;
	mPinPerp.normalize();
}

void Ant::findAntsInZone()
{
	mVisibleAnts.clear();
	
	float hitAreaRadius			= mLength * 5.0f;
	float hitAreaRadiusSqrd		= hitAreaRadius * hitAreaRadius;
	float hitAreaDistFromHead	= mLength * 5.0f;
	Vec3f hitAreaCenter			= mPos + mDir * hitAreaDistFromHead;
	
	for( vector<Ant>::iterator it = mController->mAnts.begin(); it != mController->mAnts.end(); ++it ){
		if( mId != it->mId ){		// IF IT ISNT ME
			Vec3f dirFromAntToCenter = it->mPos - hitAreaCenter;
			float distSqrd = dirFromAntToCenter.lengthSquared();
			
			if( distSqrd < hitAreaRadiusSqrd ){
				mVisibleAnts.push_back( &(*it) );
			}
		}
	}
}

void Ant::followAntsInZone()
{
	for( vector<Ant*>::iterator it = mVisibleAnts.begin(); it != mVisibleAnts.end(); ++it ){
		Ant *ant = *it;
		
		Vec3f dirToAntTail = mPos - ant->mTailPos;
		float invDist = 1.0f/( dirToAntTail.length() + 1.0f );
		mAcc -= dirToAntTail.normalized() * invDist * 0.001f;
		mAcc += ant->mVel * 0.0015f;
	}
}

void Ant::turn( float amt )
{
	mAcc += mDirPerp * amt;
}

void Ant::draw()
{
	gl::vertex( mPos );
	gl::vertex( mTailPos );

	gl::vertex( mPos );
	gl::vertex( mPos + ( mDir + mDirPerp ) );

	gl::vertex( mPos );
	gl::vertex( mPos + ( mDir - mDirPerp ) );
}




