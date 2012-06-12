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

Controller::Controller()
{
}

void Controller::init( Room *room )
{
	mRoom				= room;
	mPerlin				= Perlin( 4 );
	
	mTimeSinceBang		= 0.0f;
	
	createSphere( mBalloonsVbo, 3 );
	
	mPresetIndex		= 1;
}

void Controller::createSphere( gl::VboMesh &vbo, int res )
{
	float X = 0.525731112119f; 
	float Z = 0.850650808352f;
	
	static Vec3f verts[12] = {
		Vec3f( -X, 0.0f, Z ), Vec3f( X, 0.0f, Z ), Vec3f( -X, 0.0f, -Z ), Vec3f( X, 0.0f, -Z ),
		Vec3f( 0.0f, Z, X ), Vec3f( 0.0f, Z, -X ), Vec3f( 0.0f, -Z, X ), Vec3f( 0.0f, -Z, -X ),
		Vec3f( Z, X, 0.0f ), Vec3f( -Z, X, 0.0f ), Vec3f( Z, -X, 0.0f ), Vec3f( -Z, -X, 0.0f ) };
	
	static GLuint triIndices[20][3] = { 
		{0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1}, {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
		{7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };
	
	gl::VboMesh::Layout layout;
	layout.setStaticPositions();
	layout.setStaticNormals();
	
	mPosCoords.clear();
	mNormals.clear();
	for( int i=0; i<20; i++ ){
		drawSphereTri( verts[triIndices[i][0]], verts[triIndices[i][1]], verts[triIndices[i][2]], res );
	}
	
	float z = 0.0f;
	float s = 0.05f;
	float y1 = -0.975f;
	float y2 = -1.1f;
	
	Vec3f v0 = Vec3f( z, y1, z );
	Vec3f v1 = Vec3f( s, y2, s );
	Vec3f v2 = Vec3f(-s, y2, s );
	Vec3f v3 = Vec3f(-s, y2,-s );
	Vec3f v4 = Vec3f( s, y2,-s );
	
	mPosCoords.push_back( v1 );	// back
	mPosCoords.push_back( v2 );
	mPosCoords.push_back( v0 );
	
	mPosCoords.push_back( v4 ); // right
	mPosCoords.push_back( v1 );
	mPosCoords.push_back( v0 );
	
	mPosCoords.push_back( v3 ); // front
	mPosCoords.push_back( v4 );
	mPosCoords.push_back( v0 );
	
	mPosCoords.push_back( v2 ); // left
	mPosCoords.push_back( v3 );
	mPosCoords.push_back( v0 );
	
	mPosCoords.push_back( v1 ); // bottom
	mPosCoords.push_back( v2 );
	mPosCoords.push_back( v3 );
	
	mPosCoords.push_back( v1 ); // bottom
	mPosCoords.push_back( v3 );
	mPosCoords.push_back( v4 );

	mNormals.push_back( Vec3f::zAxis() );	// back
	mNormals.push_back( Vec3f::zAxis() );
	mNormals.push_back( Vec3f::zAxis() );
	
	mNormals.push_back( Vec3f::xAxis() );	// right
	mNormals.push_back( Vec3f::xAxis() );
	mNormals.push_back( Vec3f::xAxis() );
	
	mNormals.push_back(-Vec3f::zAxis() );	// front
	mNormals.push_back(-Vec3f::zAxis() );
	mNormals.push_back(-Vec3f::zAxis() );
	
	mNormals.push_back( Vec3f::xAxis() );	// left
	mNormals.push_back( Vec3f::xAxis() );
	mNormals.push_back( Vec3f::xAxis() );
	
	mNormals.push_back( Vec3f::yAxis() );	// bottom
	mNormals.push_back( Vec3f::yAxis() );
	mNormals.push_back( Vec3f::yAxis() );
	
	mNormals.push_back( Vec3f::yAxis() );	// bottom
	mNormals.push_back( Vec3f::yAxis() );
	mNormals.push_back( Vec3f::yAxis() );
	
	vbo = gl::VboMesh( mPosCoords.size(), 0, layout, GL_TRIANGLES );	
	vbo.bufferPositions( mPosCoords );
	vbo.bufferNormals( mNormals );
}

void Controller::modVert( Vec3f *v )
{
	float yPer = 1.0f;
	yPer = pow( v->y * 0.5f + 0.5f, 0.3f );
	
	v->x *= 0.9f * yPer;
	v->z *= 0.9f * yPer;
}

void Controller::drawSphereTri( Vec3f va, Vec3f vb, Vec3f vc, int div )
{
	// TURNS THE SPHERE INTO A BALLOON
	// BUT ULTIMATELY MESSES UP THE NORMALS
	
	if( div <= 0 ){
		modVert( &va );
		modVert( &vb );
		modVert( &vc );
		
		mPosCoords.push_back( va );
		mPosCoords.push_back( vb );
		mPosCoords.push_back( vc );
		Vec3f vn = ( va + vb + vc ) * 0.3333f;
		mNormals.push_back( va.normalized() );
		mNormals.push_back( vb.normalized() );
		mNormals.push_back( vc.normalized() );
	} else {
		Vec3f vab = ( ( va + vb ) * 0.5f ).normalized();
		Vec3f vac = ( ( va + vc ) * 0.5f ).normalized();
		Vec3f vbc = ( ( vb + vc ) * 0.5f ).normalized();
		drawSphereTri( va, vab, vac, div-1 );
		drawSphereTri( vb, vbc, vab, div-1 );
		drawSphereTri( vc, vac, vbc, div-1 );
		drawSphereTri( vab, vbc, vac, div-1 );
	}
}

void Controller::bang()
{
	int numConfettis	= 2000;
	int numStreamers	= 250;
	int numBalloons		= 250;
	
	if( mPresetIndex == 4 ){
		numBalloons		= 500;
	}
	
	addConfettis( numConfettis, Vec3f::zero(), 1.0f );
	addStreamers( numStreamers, Vec3f::zero() );
	addBalloons( numBalloons, Vec3f::zero() );
	
	// PLAY A BIG BANG SOUND HERE
	
	mTimeSinceBang		= 0.0f;
}

void Controller::checkForBalloonPop( const Vec2f &mousePos )
{
	for( vector<Balloon>::reverse_iterator it = mBalloons.rbegin(); it != mBalloons.rend(); ++it ){
		Vec2f dir		= mousePos - it->mScreenPos;
		float distSqrd	= dir.lengthSquared();
		if( distSqrd < 1000.0f ){
			Vec3f pos		= it->mPos;
			float lifespan	= 12.0f;
			float speed		= 20.0f;
			mShockwaves.push_back( Shockwave( it->mPos, Vec3f::yAxis(), lifespan, speed ) );
			
			int numConfettis = 250;
			addConfettis( numConfettis, pos, 0.5f );
			
			// PLAY A BALLOON POP AUDIO FILE HERE
			
			it->mIsDead = true;
			
			break;
		}
	}
}

void Controller::update( const Camera &cam )
{
	float dt  = mRoom->getTimeDelta();
	
	mTimeSinceBang += dt;
	if( mTimeSinceBang > 10.0f )
		applyBalloonCollisions();
	
	// SHOCKWAVES
	for( vector<Shockwave>::iterator it = mShockwaves.begin(); it != mShockwaves.end(); ){
		if( it->mIsDead ){
			it = mShockwaves.erase( it );
		} else {
			it->update( dt );
			++it;
		}
	}
	
	// CONFETTIS
	for( vector<Confetti>::iterator it = mConfettis.begin(); it != mConfettis.end(); ){
		if( it->mIsDead ){
			it = mConfettis.erase( it );
		} else {
			it->update( mRoom->getDims(), dt );
			++it;
		}
	}
	
	// STREAMERS
	for( vector<Streamer>::iterator it = mStreamers.begin(); it != mStreamers.end(); ){
		if( it->mIsDead ){
			it = mStreamers.erase( it );
		} else {
			it->update( mRoom->getDims(), dt, mRoom->getTick() );
			++it;
		}
	}
	
	// BALLOONS
	for( vector<Balloon>::iterator it = mBalloons.begin(); it != mBalloons.end(); ){
		if( it->mIsDead ){
			it = mBalloons.erase( it );
		} else {
			it->update( cam, mRoom->getDims(), dt );
			++it;
		}
	}
	
	// SORT BALLOONS
	sort( mBalloons.begin(), mBalloons.end(), depthSortFunc );
}

void Controller::applyBalloonCollisions()
{
	for( vector<Balloon>::iterator it1 = mBalloons.begin(); it1 != mBalloons.end(); ++it1 )
	{
		
		// APPLY SHOCKWAVES TO BALLOONS
		for( vector<Shockwave>::iterator shockIt = mShockwaves.begin(); shockIt != mShockwaves.end(); ++shockIt )
		{
			Vec3f dirToParticle = shockIt->mPos - it1->mPos;
			float dist = dirToParticle.length();
			if( dist > shockIt->mRadiusPrev && dist < shockIt->mRadius ){
				Vec3f dirToParticleNorm = dirToParticle.normalized();
				it1->mAcc -= dirToParticleNorm * shockIt->mImpulse * 2.5f;
			}
		}
		
		vector<Balloon>::iterator it2 = it1;
		for( std::advance( it2, 1 ); it2 != mBalloons.end(); ++it2 )
		{
			Vec3f dir			= it1->mPos - it2->mPos;
			Vec3f dirNormal		= dir.normalized();
			float dist			= dir.length();
			
			Vec3f moveVec		= it2->mVel - it1->mVel;
			
			float sumRadii		= ( it1->mRadius * 0.7f + it2->mRadius * 0.7f );
			float sumRadiiSqrd	= sumRadii * sumRadii;
			
			bool collision		= didParticlesCollide( dir, dirNormal, dist, sumRadii, sumRadiiSqrd, &moveVec );
			
			if( collision )
			{
				
				float a1	= it1->mVel.dot( dirNormal );
				float a2	= it2->mVel.dot( dirNormal );
				float pVar	= ( 2.0f * ( a1 - a2 ) );
				
				dist -= sumRadii;
				
				if( dist < 0.0f ){
					Vec3f off	= dirNormal * dist;
					
					it1->mPos -= off * 0.25f;
					it1->mVel -= off * 0.125f;
					
					it2->mPos += off * 0.25f;
					it2->mVel += off * 0.125f;
				}
				
				float collisionDecay = 0.375f;
				Vec3f newDir = pVar * dirNormal * collisionDecay;
				it1->mVel -= newDir;
				it2->mVel += newDir;
			}
		}
	}
}

bool Controller::didParticlesCollide( const ci::Vec3f &dir, const ci::Vec3f &dirNormal, const float dist, const float sumRadii, const float sumRadiiSqrd, ci::Vec3f *moveVec )
{
	float moveVecLength = sqrtf( moveVec->x * moveVec->x + moveVec->y * moveVec->y + moveVec->z * moveVec->z );
	float newDist = dist - sumRadii;
	
	if( newDist < 0 )
		return true;
	
	if( moveVecLength < dist )
		return false;
	
	moveVec->normalize();
	float D	= moveVec->dot(dir);
	
	if( D <= 0 )
		return false;
	
	float F	= ( newDist * newDist ) - ( D * D );
	
	if( F >= sumRadiiSqrd )
		return false;
	
	float T = sumRadiiSqrd - F;
	
	if( T < 0 )
		return false;
	
	float distance = D - sqrtf(T);
	
	if( moveVecLength < distance )
		return false;
	
	*moveVec *= distance;
	//			moveVec->set( moveVec * distance );
	//			moveVec->set( moveVec->normalized() * distance );
	
	return true;
}

void Controller::draw()
{
}
	
void Controller::drawConfettis( gl::GlslProg *shader )
{
	for( vector<Confetti>::iterator it = mConfettis.begin(); it != mConfettis.end(); ++it ){
		shader->uniform( "color", it->mColor );
		shader->uniform( "matrix", it->mMatrix );
		it->draw();
	}
}

void Controller::drawStreamers()
{
	for( vector<Streamer>::iterator it = mStreamers.begin(); it != mStreamers.end(); ++it ){
		gl::color( it->mColor );
		it->draw();
	}
}

void Controller::drawBalloons( gl::GlslProg *shader )
{
	for( vector<Balloon>::iterator it = mBalloons.begin(); it != mBalloons.end(); ++it ){
		shader->uniform( "matrix", it->mMatrix );
		shader->uniform( "color", it->mColor );
		gl::draw( mBalloonsVbo );
	}
}

void Controller::drawPhysics()
{
	gl::color( Color( 1.0f, 0.0f, 0.0f ) );
	for( vector<Balloon>::iterator it = mBalloons.begin(); it != mBalloons.end(); ++it ){
		gl::drawCube( it->mPos, Vec3f( it->mRadius, it->mRadius, it->mRadius ) * 0.2f );
		gl::drawCube( it->mSpringPos, Vec3f( it->mRadius, it->mRadius, it->mRadius ) * 0.1f );
		gl::drawLine( it->mPos, it->mSpringPos );
	}
	
	gl::color( Color( 0.0f, 1.0f, 0.0f ) );
	for( vector<Confetti>::iterator it = mConfettis.begin(); it != mConfettis.end(); ++it ){
		gl::drawCube( it->mPos, Vec3f( 2.0f, 2.0f, 2.0f ) );
	}

	gl::color( Color( 0.0f, 0.0f, 1.0f ) );
	for( vector<Streamer>::iterator it = mStreamers.begin(); it != mStreamers.end(); ++it ){
		gl::drawCube( it->mPos, Vec3f( 4.0f, 4.0f, 4.0f ) );
		it->draw();
	}
}

void Controller::addConfettis( int amt, const Vec3f &pos, float speedMulti )
{
	for( int i=0; i<amt; i++ ){
		mConfettis.push_back( Confetti( pos, speedMulti, mPresetIndex ) );
	}
}

void Controller::addStreamers( int amt, const Vec3f &pos )
{
	for( int i=0; i<amt; i++ ){
		mStreamers.push_back( Streamer( pos, mPresetIndex ) );
	}
}

void Controller::addBalloons( int amt, const Vec3f &pos )
{
	for( int i=0; i<amt; i++ ){
		mBalloons.push_back( Balloon( pos, mPresetIndex ) );
	}
}

void Controller::clear()
{
	mBalloons.clear();
	mStreamers.clear();
	mConfettis.clear();
}

void Controller::preset( int index )
{
	mPresetIndex = index;
}

bool depthSortFunc( Balloon a, Balloon b ){
	return a.mPos.z > b.mPos.z;
}

