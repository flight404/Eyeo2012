//
//  Controller.cpp
//  BubbleChamber
//
//  Created by Robert Hodgin on 5/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "Controller.h"

using namespace ci;
using std::vector;
using std::list;

#define PARTICLE_SPEED 25.0f

Controller::Controller()
{
}

void Controller::init( Room *room )
{
	mRoom					= room;
	
	mPrevTotalBubbleVerts	= -1;
	mTotalBubbleVerts		= 0;
    mBubbleVerts			= NULL;
	
	mIntensity				= 0.0f;
}

void Controller::update()
{
	float dt  = mRoom->getTimeDelta();
	bool tick = mRoom->getTick();
	
	if( tick ){
		if( Rand::randFloat() < mIntensity ){
			int amt = 1;
			for( int i=0; i<amt; i++ ){
				vector<Vec3f> posAndVel = mRoom->getRandRoomEntryPos();
				int len = 5;
				addParticle( 0, posAndVel[0], posAndVel[1], PARTICLE_SPEED, len );
			}
		}
	}
	updateParticles( dt, tick );
	updateBubbles( dt );
	updateDecals( dt, tick );
	updateSmokes( dt );
	updateShockwaves( dt );
	applyForcesToMoths();
	updateMoths( dt );
	updateGibs( dt );
	updateGlowCubes( dt );
}

bool Controller::checkMothCollisions( Particle *particle )
{
	for( vector<Moth>::iterator it = mMoths.begin(); it != mMoths.end(); ){
		Vec3f dirToMoth = it->mPos - particle->mPos;
		float distToMoth = dirToMoth.length();
		
		if( distToMoth < 5.0f ){
			mExplodingMoths.push_back( &(*it) );
			it->mIsDead = true;
			return true;
		}
		++it;
	}
	return false;
}

void Controller::updateParticles( float dt, bool tick )
{
	mNewParticles.clear();
	mExplodingMoths.clear();

	for( list<Particle>::iterator it = mParticles.begin(); it != mParticles.end(); ){
		if( it->mIsDead ){
			it = mParticles.erase( it );
		} else {
			bool mothExploded = false;
			if( it->mGen == 0 || Rand::randFloat() < 0.01f ){
				mothExploded = checkMothCollisions( &(*it) );
				if( mothExploded ){
					explode( &(*it) );
				}
			}
			
			it->update( mRoom, dt, tick );
			if( tick || it->mBounced ){ // ADD BUBBLES ALONG THE LENGTH OF ONE TICK
										// WORTH OF PARTICLE MOVEMENT
				Vec3f dir	= it->mPos - it->mPosLastTick;
				float dist	= dir.length();
				
				int numDivs = dist/( it->mGen * 4 + 2 );	// THE NUMBER OF BUBBLES PROPORTIONAL TO LENGTH
				for( int i=0; i<numDivs; i++ ){
					float per = (float)i/(float)numDivs;
					Vec3f pos = lerp( it->mPosLastTick, it->mPos, per );
					addBubble( pos, Rand::randVec3f() * 0.025f, dt*( 1.0f - per ) );
				}
				
				it->mPosLastTick = it->mPos;
			}
			
			// IF PARTICLE HIT A WALL
			if( it->mBounced && it->mDeathAxis.z < 0.5f ){
				mDecals.push_back( Decal( it->mDeathPos, it->mDeathAxis, it->mVel.length() * 1.5f, 25.0f ) );
				it->mIsDying = true;
				it->mBounced = false;
			}
			
			
			++it;
		}
	}
	
	for( vector<Moth*>::iterator it = mExplodingMoths.begin(); it != mExplodingMoths.end(); ++it ){
		// MAKE NEW PARTICLES
		int numParticles = 30;
		for( int i=0; i<numParticles; i++ ){
			Vec3f dir		= Rand::randVec3f();
			int len			= 25;
			mNewParticles.push_back( Particle( this, 3, (*it)->mPos, dir, PARTICLE_SPEED * 0.3f, len ) );
		}

		// MAKE EXTRA BUBBLES
		for( int i=0; i<400; i++ ){
			float r = Rand::randFloat( 5.0f );
			Vec3f rv = Rand::randVec3f() * 3.0f;
			Vec3f pos = (*it)->mPos + rv * r * r;
			addBubble( pos, rv, 0.0f );
		}
		
//		// MAKE SMOKES
//		for( int i=0; i<3; i++ ){
//			mSmokes.push_back( Smoke( (*it)->mPos, Vec3f::zero(), Rand::randFloat( 25.0f, 27.0f ), Rand::randFloat( 4.0f, 8.0f ) ) ); 
//		}
	}
	
	// ADD NEW PARTICLES
	for( list<Particle>::iterator it = mNewParticles.begin(); it != mNewParticles.end(); ++it ){
		mParticles.push_back( *it );
	}
}

void Controller::updateBubbles( float dt )
{
	for( vector<Bubble>::iterator it = mBubbles.begin(); it != mBubbles.end(); ){
		if( it->mIsDead ){
			it = mBubbles.erase( it );
		} else {
			it->update( mRoom, dt );
			++it;
		}
	}	
	
	mTotalBubbleVerts = mBubbles.size();
	
    if (mTotalBubbleVerts != mPrevTotalBubbleVerts) {
        if (mBubbleVerts != NULL) {
            delete[] mBubbleVerts;
        }
        mBubbleVerts = new BubbleVertex[mTotalBubbleVerts];
        mPrevTotalBubbleVerts = mTotalBubbleVerts;
    }
	
	int vIndex = 0;
	for( vector<Bubble>::iterator it = mBubbles.begin(); it != mBubbles.end(); ++it ){        
		mBubbleVerts[vIndex].vertex = it->mPos;
		mBubbleVerts[vIndex].color  = Vec4f( 1.0f, 1.0f, 1.0f, 1.0f );
        vIndex++;
	}
}

void Controller::updateDecals( float dt, bool tick )
{
	for( vector<Decal>::iterator it = mDecals.begin(); it != mDecals.end(); ){
		if( it->mIsDead ){
			it = mDecals.erase( it );
		} else {
			it->update( dt );
			
			if( tick ){
				// MAKE EXTRA BUBBLES
				for( int i=0; i<2; i++ ){
					Vec3f pos = it->mPos + Rand::randVec3f() * Rand::randFloat( 2.0f );
					addBubble( pos, Rand::randVec3f(), 0.0f );
				}
			}
			
			it++;
		}
	}
}

void Controller::updateSmokes( float dt )
{
	for( vector<Smoke>::iterator it = mSmokes.begin(); it != mSmokes.end(); ){
		if( it->mIsDead ){
			it = mSmokes.erase( it );
		} else {
			it->update( dt );
			++it;
		}
	}
}

void Controller::updateMoths( float dt )
{
	for( vector<Moth>::iterator it = mMoths.begin(); it != mMoths.end(); ){
		if( it->mIsDead ){
			it = mMoths.erase( it );
		} else {
			it->update( mRoom, dt );
			++it;
		}
	}
}

void Controller::updateGibs( float dt )
{
	for( vector<Gib>::iterator it = mGibs.begin(); it != mGibs.end(); ){
		if( it->mIsDead ){
			it = mGibs.erase( it );
		} else {
			it->update( mRoom->getGravity(), mRoom->getDims(), dt );
			++it;
		}
	}
}

void Controller::updateShockwaves( float dt )
{
	// SHOCKWAVES
	for( vector<Shockwave>::iterator it = mShockwaves.begin(); it != mShockwaves.end(); ){
		if( it->mIsDead ){
			it = mShockwaves.erase( it );
		} else {
			it->update( dt );
			++it;
		}
	}
}

void Controller::updateGlowCubes( float dt )
{
	// GLOWCUBES
	for( vector<GlowCube>::iterator it = mGlowCubes.begin(); it != mGlowCubes.end(); ){
		if( it->mIsDead ){
			it = mGlowCubes.erase( it );
		} else {
			it->update( dt );
			++it;
		}
	}
}

void Controller::applyForcesToMoths()
{
	float zoneRadiusSqrd = 4000.0f;
	float lowThresh = 0.7f;
	float hiThresh  = 0.9f;
	
	float twoPI = M_PI * 2.0f;
	for( vector<Moth>::iterator p1 = mMoths.begin(); p1 != mMoths.end(); ++p1 ){
		
		for( vector<Shockwave>::iterator shockIt = mShockwaves.begin(); shockIt != mShockwaves.end(); ++shockIt )
		{
			Vec3f dirToMoth = shockIt->mPos - p1->mPos;
			float dist = dirToMoth.length();
			if( dist > shockIt->mRadiusPrev && dist < shockIt->mRadius ){
				Vec3f dirToMothNorm = dirToMoth.normalized();
				p1->mAcc -= dirToMothNorm * shockIt->mImpulse * 5.0f;
			}
		}
		
		vector<Moth>::iterator p2 = p1;
		for( ++p2; p2 != mMoths.end(); ++p2 ) {
			Vec3f dir = p1->mPos - p2->mPos;
			float distSqrd = dir.lengthSquared();

			if( distSqrd < zoneRadiusSqrd ){			// Neighbor is in the zone
				float percent = distSqrd/zoneRadiusSqrd;
				
				if( percent < lowThresh ){			// Separation
					float F = ( lowThresh/percent - 1.0f ) * 0.01f;
					dir = dir.normalized() * F;
					
					p1->mAcc += dir;
					p2->mAcc -= dir;
				} else if( percent < hiThresh ){	// Alignment
					float threshDelta		= hiThresh - lowThresh;
					float adjustedPercent	= ( percent - lowThresh )/threshDelta;
					float F					= ( 1.0 - ( cos( adjustedPercent * twoPI ) * -0.5f + 0.5f ) ) * 0.04f;
					
					p1->mAcc += p2->mAxis * F;
					p2->mAcc += p1->mAxis * F;
					
				} else {								// Cohesion
					float threshDelta		= 1.0f - hiThresh;
					float adjustedPercent	= ( percent - hiThresh )/threshDelta;
					float F					= ( 1.0 - ( cos( adjustedPercent * twoPI ) * -0.5f + 0.5f ) ) * 0.01f;
					
					dir = dir.normalized() * F;
					
					p1->mAcc -= dir;
					p2->mAcc += dir;
				}
			}
		}
	}
}

void Controller::explode( Particle *particle )
{
	int numParticles	= Rand::randInt( 2, 6 );
	
	// MAKE NEW PARTICLES
	for( int i=0; i<numParticles; i++ ){
		float angle		= Rand::randFloat( M_PI * 2.0f );
		Vec3f dir		= ( cos( angle ) * particle->mPerp1 + sin( angle ) * particle->mPerp2 );
		int len = 25;
		mNewParticles.push_back( Particle( this, particle->mGen++, particle->mPos, dir, particle->mSpeed * 0.5f, len ) );
	}
	
	// MAKE SMOKES
	for( int i=0; i<3; i++ ){
		mSmokes.push_back( Smoke( particle->mPos, Vec3f::zero(), Rand::randFloat( 5.0f, 7.0f ), Rand::randFloat( 4.0f, 8.0f ) ) ); 
	}
	
	// MAKE EXTRA BUBBLES
	for( int i=0; i<200; i++ ){
		float r = Rand::randFloat( 5.0f );
		Vec3f rv = Rand::randVec3f();
		Vec3f pos = particle->mPos + rv * r * r;
		addBubble( pos, rv, 0.0f );
	}
	
	// MAKE BANK
//	mDecals.push_back( Bank( particle->mPos, -Vec3f::zAxis(), particle->mVel.length() * 4.0f, 30.0f ) );
	
	// MAKE SHOCKWAVE
	float lifespan	= 25.0f;
	float speed		= 10.0f;
	mShockwaves.push_back( Shockwave( particle->mPos, lifespan, speed ) );

	// MAKE GIBS
	int numGibs = Rand::randInt( 100, 200 );
	for( int i=0; i<numGibs; i++ ){
		float radius = Rand::randFloat( 0.25f, 3.0f );
		mGibs.push_back( Gib( particle->mPos, Rand::randVec3f() * Rand::randFloat( 1.0f ), radius ) );
	}
	
	// MAKE GLOWCUBES
	int numCubes = Rand::randInt( 10, 15 );
	for( int i=0; i<numCubes; i++ ){
		mGlowCubes.push_back( GlowCube( particle->mPos + Rand::randVec3f() * 5.0f ) );
	}
}

void Controller::draw()
{
	
}

void Controller::drawParticles( float power )
{
	
	
	glBegin( GL_LINES );
	for( list<Particle>::iterator it = mParticles.begin(); it != mParticles.end(); ++it ){
		float invLen = 1.0f/(float)it->mLen;
		float prevPer = 1.0f;
		for( int i=1; i<it->mLen; i++ ){
			float per = 1.0f - (float)i * invLen;
			gl::color( ColorA( power, power, power, prevPer * 0.5f ) );
			gl::vertex( it->mPs[i-1] );
			gl::color( ColorA( power, power, power, per * 0.5f ) );
			gl::vertex( it->mPs[i] );
			prevPer = per;
		}
	}
	glEnd();
}

void Controller::drawBubbles( float power )
{
	glBegin( GL_POINTS );
	for( vector<Bubble>::iterator it = mBubbles.begin(); it != mBubbles.end(); ++it ){
		gl::color( ColorA( power, power, power, it->mAgePer ) );
		gl::vertex( it->mPos );
	}
	glEnd();
	
	if( mTotalBubbleVerts > 0 ){
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_COLOR_ARRAY );
		glVertexPointer( 3, GL_FLOAT, sizeof(BubbleVertex), mBubbleVerts );
		glColorPointer( 4, GL_FLOAT, sizeof(BubbleVertex), &mBubbleVerts[0].color );
		
		glDrawArrays( GL_POINTS, 0, mTotalBubbleVerts );
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_COLOR_ARRAY );
	}
}

void Controller::drawDecals()
{
	for( vector<Decal>::iterator it = mDecals.begin(); it != mDecals.end(); ++it ){
		it->draw();
	}
}

void Controller::drawSmokes( const Vec3f &right, const Vec3f &up )
{
	for( vector<Smoke>::iterator it = mSmokes.begin(); it != mSmokes.end(); ++it ){
		it->draw( right, up );
	}
}

void Controller::drawMoths()
{
	for( vector<Moth>::iterator it = mMoths.begin(); it != mMoths.end(); ++it ){
		it->draw();
	}
}

void Controller::drawGibs()
{
	for( vector<Gib>::iterator it = mGibs.begin(); it != mGibs.end(); ++it ){
		it->draw();
	}
}

void Controller::drawGlowCubes( gl::GlslProg *shader )
{
	for( vector<GlowCube>::iterator it = mGlowCubes.begin(); it != mGlowCubes.end(); ++it ){
		shader->uniform( "matrix", it->mMatrix );
		it->draw();
	}
}

void Controller::addBubble( const Vec3f &pos, const Vec3f &vel, float age )
{
	mBubbles.push_back( Bubble( pos, vel, age ) );
}

void Controller::addParticle( int gen, const Vec3f &pos, const Vec3f &dir, float speed, int len )
{
	mParticles.push_back( Particle( this, gen, pos, dir, speed, len ) );
	mDecals.push_back( Decal( pos, Vec3f::yAxis(), speed, 30.0f ) );
}

void Controller::addParticles( int amt, bool isShort )
{
	for( int i=0; i<amt; i++ ){
		int gen = 0;
		vector<Vec3f> posAndVel = mRoom->getRandRoomEntryPos();
		Vec3f pos = posAndVel[0];
		Vec3f vel = posAndVel[1];
		float speed = 25.0f;
		int len;
		if( isShort )	len = 12;
		else			len = 24;
		addParticle( gen, pos, vel, speed, len );
	}
}

void Controller::addMoth( const Vec3f &pos )
{
	mMoths.push_back( Moth( pos ) );
}

void Controller::releaseMoths()
{
	for( int i=0; i<50; i++ ){
		addMoth( Rand::randVec3f() * 50.0f );
	}
}

void Controller::preset( int i )
{
	switch( i ) {
		case 0: mIntensity = 0.0f;		break;
		case 1: mIntensity = 0.05f;		break;
		case 2: mIntensity = 0.1f;		break;
		case 3: mIntensity = 0.2f;		break;
		case 4: mIntensity = 0.4f;		break;
		case 5: mIntensity = 0.7f;		break;
		case 6: mIntensity = 1.0f;		break;
		default :						break;
	}
}

void Controller::clearRoom()
{
	mParticles.clear();
	mBubbles.clear();
	mDecals.clear();
	mSmokes.clear();
	mExplodingMoths.clear();
	mMoths.clear();
	mShockwaves.clear();
	mGlowCubes.clear();
	mGibs.clear();
}
