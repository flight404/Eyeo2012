#include "Photon.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;

Photon::Photon()
{
}

Photon::Photon( Vec3f pos, Vec3f vel )
{
	mPos			= pos;
	mVel			= vel;
	
	mLen			= 3;
	for( int i=0; i<mLen; i++ ){
		mPs.push_back( mPos );
	}

	mRadius			= 2.0f;
	mAge			= 0.0f;
	mLifespan		= 20.0f;
	mIsDead			= false;
}	

void Photon::update( float t )
{	
	for( int i=mLen-1; i>0; i-- ){
		mPs[i] = mPs[i-1];
	}
	mPos += mVel * t;
	mPs[0] = mPos;
	
	mAge += 1.0f;
	if( mAge > mLifespan ) mIsDead = true;
	
	mAgePer = 1.0f - mAge/mLifespan;
}

void Photon::draw()
{
	gl::color( Color( 1, 1, 1 ) );
	gl::drawSphere( mPos, mRadius, 8 );
}

void Photon::drawTail()
{
	float perSeg = 1.0f/(float)mLen;
	for( int i=0; i<mLen-1; i++ ){
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, ( 1.0f - perSeg * (float)i ) * mAgePer * 0.3f ) );
		gl::vertex( mPs[i] );
		gl::vertex( mPs[i+1] );
	}
}

