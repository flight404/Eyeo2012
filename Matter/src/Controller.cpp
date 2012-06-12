#include <boost/foreach.hpp>
#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "Controller.h"
#include "SpringCam.h"

using namespace ci;
using std::list;
using std::vector;

Controller::Controller()
{
}

void Controller::init( Room *room )
{
	mRoom			 = room;

	mNumMatter		= 0;
	mNumAntimatter	= 0;
	
	mPreset			= 0;
	mRecycle		= false;
	
	createSphere( mSphereLo, 2 );
	createSphere( mSphereHi, 3 );
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
	layout.setStaticIndices();
	layout.setStaticPositions();
	layout.setStaticNormals();
	layout.setStaticTexCoords2d();
	
	mIndex = 0;
	mPosCoords.clear();
	mNormals.clear();
	mIndices.clear();
	mTexCoords.clear();
	for( int i=0; i<20; i++ ){
		drawSphereTri( verts[triIndices[i][0]], verts[triIndices[i][1]], verts[triIndices[i][2]], res );
	}
	vbo = gl::VboMesh( mPosCoords.size(), mIndices.size(), layout, GL_TRIANGLES );	
	vbo.bufferIndices( mIndices );
	vbo.bufferPositions( mPosCoords );
	vbo.bufferNormals( mNormals );
	vbo.bufferTexCoords2d( 0, mTexCoords );
}

void Controller::drawSphereTri( Vec3f va, Vec3f vb, Vec3f vc, int div )
{
	if( div <= 0 ){
		mPosCoords.push_back( va );
		mPosCoords.push_back( vb );
		mPosCoords.push_back( vc );
		mIndices.push_back( mIndex++ );
		mIndices.push_back( mIndex++ );
		mIndices.push_back( mIndex++ );
		Vec3f vn = ( va + vb + vc ) * 0.3333f;
		mNormals.push_back( va );
		mNormals.push_back( vb );
		mNormals.push_back( vc );
		
		float u1 = ( (float)atan( va.y/(va.x+0.0001f) ) + (float)M_PI * 0.5f ) / (float)M_PI;
		float u2 = ( (float)atan( vb.y/(vb.x+0.0001f) ) + (float)M_PI * 0.5f ) / (float)M_PI;
		float u3 = ( (float)atan( vc.y/(vc.x+0.0001f) ) + (float)M_PI * 0.5f ) / (float)M_PI;
		
		if( va.x <= 0.0f ){
			u1 = u1 * 0.5f + 0.5f;
		} else {
			u1 *= 0.5f;
		}
		
		if( vb.x <= 0.0f ){
			u2 = u2 * 0.5f + 0.5f;
		} else {
			u2 *= 0.5f;
		}
		
		if( vc.x <= 0.0f ){
			u3 = u3 * 0.5f + 0.5f;
		} else {
			u3 *= 0.5f;
		}
		
		
		float v1 = (float)acos( va.z ) / (float)M_PI;
		float v2 = (float)acos( vb.z ) / (float)M_PI;
		float v3 = (float)acos( vc.z ) / (float)M_PI;
		
		mTexCoords.push_back( Vec2f( u1, v1 ) );
		mTexCoords.push_back( Vec2f( u2, v2 ) );
		mTexCoords.push_back( Vec2f( u3, v3 ) );
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

void Controller::applyShockwavesToTime()
{
	for( vector<Shockwave>::iterator shockIt = mShockwaves.begin(); shockIt != mShockwaves.end(); ++shockIt )
	{
		Vec3f roomCenter = Vec3f::zero();
		Vec3f dirToParticle = shockIt->mPos - roomCenter;
		float dist = dirToParticle.length();
		if( dist > shockIt->mRadiusPrev && dist < shockIt->mRadius ){
			mRoom->mTimeElapsed += ( dist - shockIt->mRadiusPrev )/( shockIt->mRadius - shockIt->mRadiusPrev ) * 10.0f;
		}
	}
}

void Controller::applyShockwavesToCam( SpringCam &cam )
{
	for( vector<Shockwave>::iterator shockIt = mShockwaves.begin(); shockIt != mShockwaves.end(); ++shockIt )
	{
		Vec3f dirToParticle = shockIt->mPos - cam.mEyeNode.mPos;
		float dist = dirToParticle.length();
		if( dist > shockIt->mRadiusPrev && dist < shockIt->mRadius ){
			Vec3f dirToParticleNorm = dirToParticle.normalized();
			cam.mEyeNode.mAcc -= dirToParticleNorm * shockIt->mImpulse * 5.0f;
		}
		
		dirToParticle = shockIt->mPos - cam.mCenNode.mPos;
		dist = dirToParticle.length();
		if( dist > shockIt->mRadiusPrev && dist < shockIt->mRadius ){
			Vec3f dirToParticleNorm = dirToParticle.normalized();
			cam.mCenNode.mAcc -= dirToParticleNorm * shockIt->mImpulse * 1.0f;
		}
		
		dirToParticle = shockIt->mPos - cam.mUpNode.mPos;
		dist = dirToParticle.length();
		if( dist > shockIt->mRadiusPrev && dist < shockIt->mRadius ){
			Vec3f dirToParticleNorm = dirToParticle.normalized();
			cam.mUpNode.mAcc -= dirToParticleNorm * shockIt->mImpulse * 1.0f;
		}
	}
}

void Controller::applyForceToParticles()
{
	float prevNumMatter		= mNumMatter;
	float prevNumAntimatter = mNumAntimatter;
	float matterRange		= 1000.0f - math<float>::min( prevNumMatter, 400.0f ) * 2.0f;
	matterRange				*= matterRange * 3.0f;
	float antimatterRange	= 1000.0f - math<float>::min( prevNumAntimatter, 400.0f ) * 2.0f;
	antimatterRange			*= antimatterRange * 3.0f;
	
//	matterRange = 1000.0f;
//	antimatterRange = 1000.0f;
	
	mNumMatter		= 0;
	mNumAntimatter	= 0;
	
	for( list<Particle>::iterator it1 = mParticles.begin(); it1 != mParticles.end(); ++it1 )
	{
		float zoneRadiusSqrd;
		if( it1->mCharge < 0.0f ){
			mNumAntimatter ++;
			zoneRadiusSqrd = antimatterRange;
		} else {
			mNumMatter ++;
			zoneRadiusSqrd = matterRange;
		}
		
		// APPLY SHOCKWAVES TO OTHER PARTICLES
		for( vector<Shockwave>::iterator shockIt = mShockwaves.begin(); shockIt != mShockwaves.end(); ++shockIt )
		{
			Vec3f dirToParticle = shockIt->mPos - it1->mPos;
			float dist = dirToParticle.length();
			if( dist > shockIt->mRadiusPrev && dist < shockIt->mRadius ){
				Vec3f dirToParticleNorm = dirToParticle.normalized();
				it1->mAcc -= dirToParticleNorm * shockIt->mImpulse * 2.5f;
				it1->mFusionThresh = 1.0f;
			}
		}
		
		list<Particle>::iterator it2 = it1;
		for( std::advance( it2, 1 ); it2 != mParticles.end(); ++it2 ) {
			Vec3f dir = it1->mPos - it2->mPos;
			float distSqrd = dir.lengthSquared();
			
			if( distSqrd < zoneRadiusSqrd ){
				float totalCharge = it1->mCharge * it2->mCharge;
				bool shouldAnnihilate = false;
				if( totalCharge < 0.0f )
					shouldAnnihilate = distSqrd < ( 2.0f + ( it1->mFusionThresh + it2->mFusionThresh ) * 10.5f );
				
				// lit room
				if( mRoom->getPower() < 0.5f ){
					Vec3f moveVec		= it2->mVel - it1->mVel;
					Vec3f dirNormal		= dir.normalized();
					
					float dist			= sqrt( distSqrd );
					float invDistSqrd	= 1.0f/distSqrd;
					
					float sumRadii		= ( it1->mRadius + it2->mRadius );
					float sumRadiiSqrd	= sumRadii * sumRadii;
					float sumMass		= it1->mMass + it2->mMass + 0.001f;
					
					bool collision		= didParticlesCollide( dir, dirNormal, dist, sumRadii, sumRadiiSqrd, &moveVec );
					
					if( collision )
					{
						float invSumMass	= 1.0f/(float)sumMass;
						
						float a1	= it1->mVel.dot( dirNormal );
						float a2	= it2->mVel.dot( dirNormal );
						float pVar	= ( 2.0f * ( a1 - a2 ) ) * invSumMass;
						
						dist -= sumRadii;
						
						if( dist < 0.0f ){
							float per1	= it1->mMass * invSumMass;
							float per2	= 1.0f - per1;
							Vec3f off	= dirNormal * dist;
							
							it1->mPos -= off * per2;
							it1->mVel -= off * per2 * 0.5f;
							
							it2->mPos += off * per1;
							it2->mVel += off * per1 * 0.5f;
						}
						
						float collisionDecay = 0.75f;
						Vec3f newDir = pVar * dirNormal * collisionDecay;
						it1->mVel -= it2->mMass * newDir;
						it2->mVel += it1->mMass * newDir;
						
						
						

//						float cDecay = 0.75f;
//						Vec3f newDir = pVar * dirNormal * cDecay;
////							it1->mPos	-= moveVec * 0.5f;
////							it2->mPos	+= moveVec * 0.5f;
//						it1->mVel	-= it2->mMass * newDir;
//						it2->mVel	+= it1->mMass * newDir;	
					}
					
					float F = ( it1->mMass * it2->mMass ) * invDistSqrd * 0.000015 * mRoom->getTimeDelta();
					it1->mVel -= F*dirNormal*it1->mInvMass;
					it2->mVel += F*dirNormal*it2->mInvMass;
					
				// dark room
				} else {
			
					if( it1->mDistToClosestNeighbor > distSqrd ){
						it1->mDistToClosestNeighbor = distSqrd;
					}
					
					if( it2->mDistToClosestNeighbor > distSqrd ){
						it2->mDistToClosestNeighbor = distSqrd;
					}
					
					
					
					if( shouldAnnihilate ) annihilate(  &(*it1), &(*it2) );
				}
				
				float q = totalCharge / distSqrd;
				dir		= dir.normalized() * 5.0f * q * mRoom->getPower();
				
				it1->mAcc += dir;
				it2->mAcc -= dir;
			}
		}
		
		it1->mAcc += Vec3f::yAxis() * mRoom->getGravity();
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
	return true;
}

void Controller::annihilate( Particle *p1, Particle *p2 )
{
	p1->mIsDead = true;
	p2->mIsDead = true;
	Vec3f p		= ( p1->mPos + p2->mPos ) * 0.5f;
	Vec3f v		= ( p1->mVel + p2->mVel ) * 0.5f;
	Vec3f dir	= p1->mPos - p2->mPos;
	Vec3f axis	= dir.normalized();
	Vec3f right	= axis.cross( Vec3f::zAxis() ).normalized();
	Vec3f up	= axis.cross( right ).normalized();
	right		= axis.cross( up ).normalized();
	
	float lifespan	= 25.0f;
	float speed		= 20.0f;
	mShockwaves.push_back( Shockwave( p, up, lifespan, speed ) );
	
	// MAKE PHOTONS
	int numPhotons = 20;
	float perSeg = 1.0f/(float)numPhotons;
	for( int i=0; i<numPhotons; i++ ){
		float angle = (float)i*perSeg*M_PI*2.0f;
		Vec3f dir	= ( cos( angle ) * right + sin( angle ) * up );
		Vec3f vel	= v + dir * 15.0f;
		
		mPhotons.push_back( Photon( p + dir * 50.0f, vel ) );
	}

	// MAKE SMOKES
	for( int i=0; i<3; i++ ){
		mSmokes.push_back( Smoke( p, Vec3f::zero(), Rand::randFloat( 50.0f, 120.0f ), Rand::randFloat( 4.0f, 8.0f ) ) ); 
	}
}

void Controller::update()
{
	float dt = mRoom->getTimeDelta();
	
	// PARTICLES
	int numNewParticles = 0;
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ){
		if( p->mIsDead ){
			p = mParticles.erase( p );
			numNewParticles ++;
		} else {
			p->update( mRoom, dt );
			++p;
		}
	}
	
	// PHOTONS
	for( list<Photon>::iterator p = mPhotons.begin(); p != mPhotons.end(); ){
		if( p->mIsDead ){
			p = mPhotons.erase( p );
		} else {
			p->update( dt );
			++p;
		}
	}
	
	// SHOCKWAVES
	for( vector<Shockwave>::iterator it = mShockwaves.begin(); it != mShockwaves.end(); ){
		if( it->mIsDead ){
			it = mShockwaves.erase( it );
		} else {
			it->update( dt );
			++it;
		}
	}
	
	// SMOKES
	for( vector<Smoke>::iterator it = mSmokes.begin(); it != mSmokes.end(); ){
		if( it->mIsDead ){
			it = mSmokes.erase( it );
		} else {
			it->update( dt );
			++it;
		}
	}
	
	if( mRecycle )
		addParticles( numNewParticles );
}

void Controller::draw( gl::GlslProg *shader )
{
	// DRAW PARTICLES
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		shader->uniform( "pos", p->mPos );
		shader->uniform( "radius", p->mRadius );
		shader->uniform( "charge", p->mCharge * 0.5f + 0.5f );
//		shader->uniform( "distPer", p->mDistPer + ( p->mCharge * 0.5f + 0.5f ) );
		p->draw( mSphereLo, mSphereHi );
	}
}

void Controller::drawParticleShadows()
{
	float yFloor = mRoom->getFloorLevel();
	
	// DRAW PARTICLES
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		float yBase				= p->mPos.y - p->mRadius;
		float yDistFromFloor	= ( yBase - yFloor );
		float maxDist			= p->mRadius * 4.0f;
		float perDist			= yDistFromFloor/maxDist;
		if( perDist < 1.0f ){
//			float per = ( p->mPos.y - p->mRadius - y )/(yFloor * -0.25f );
			gl::color( ColorA( 0.0f, 0.0f, 0.0f, 1.0f - perDist ) );
			gl::drawBillboard( Vec3f( p->mPos.x, yFloor, p->mPos.z ), Vec2f( p->mRadius, p->mRadius ) * ( 4.0f + perDist * 7.0f ), 0.0f, Vec3f::xAxis(), Vec3f::zAxis() );
		}
	}
}
	
void Controller::drawPhotons()
{
	glBegin( GL_LINES );
	for( list<Photon>::iterator p = mPhotons.begin(); p != mPhotons.end(); ++p ){
		p->drawTail();
	}
	glEnd();
}

void Controller::drawShockwaves( const Vec3f &camPos )
{
	for( vector<Shockwave>::iterator it = mShockwaves.begin(); it != mShockwaves.end(); ++it ){
//		gl::color( ColorA( 1.0f, 1.0f, 1.0f, it->mAgePer ) );
//		drawSphericalBillboard( camPos, it->mPos, Vec2f( it->mRadius, it->mRadius ), 0.0f );
		
		if( it->mAge < 1.5f ){
			gl::color( ColorA( 1.0f, 0.05f, 0.05f, 1.0f ) );
			drawSphericalBillboard( camPos, it->mPos, Vec2f( 800.0f, 800.0f ), it->mRot );
//		} else if( it->mAge < 2.5f ){
//			gl::color( ColorA( 0.0f, 1.0f, 0.0f, 1.0f ) );
//			drawSphericalBillboard( camPos, it->mPos, Vec2f( 800.0f, 800.0f ), 0.0f );
		} else if( it->mAge < 2.5f ){
			gl::color( ColorA( 0.05f, 0.05f, 1.0f, 1.0f ) );
			drawSphericalBillboard( camPos, it->mPos, Vec2f( 600.0f, 600.0f ), it->mRot );
		}
	}
}

void Controller::drawSphericalBillboard( const Vec3f &camEye, const Vec3f &objPos, const Vec2f &scale, float rotInRadians )
{	
	glPushMatrix();
	glTranslatef( objPos.x, objPos.y, objPos.z );
	
	Vec3f lookAt = Vec3f::zAxis();
	Vec3f upAux;
	float angleCosine;
	
	Vec3f objToCam = ( camEye - objPos ).normalized();
	Vec3f objToCamProj = Vec3f( objToCam.x, 0.0f, objToCam.z );
	objToCamProj.normalize();
	
	upAux = lookAt.cross( objToCamProj );
	
	// Cylindrical billboarding
	angleCosine = constrain( lookAt.dot( objToCamProj ), -1.0f, 1.0f );
	glRotatef( toDegrees( acos(angleCosine) ), upAux.x, upAux.y, upAux.z );	
	
	// Spherical billboarding
	angleCosine = constrain( objToCamProj.dot( objToCam ), -1.0f, 1.0f );
	if( objToCam.y < 0 )	glRotatef( toDegrees( acos(angleCosine) ), 1.0f, 0.0f, 0.0f );	
	else					glRotatef( toDegrees( acos(angleCosine) ),-1.0f, 0.0f, 0.0f );
	
	
	Vec3f verts[4];
	GLfloat texCoords[8] = { 0, 0, 0, 1, 1, 0, 1, 1 };
	
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glVertexPointer( 3, GL_FLOAT, 0, &verts[0].x );
	glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
	
	float sinA = ci::math<float>::sin( rotInRadians );
	float cosA = ci::math<float>::cos( rotInRadians );
	
	float scaleXCosA = 0.5f * scale.x * cosA;
	float scaleXSinA = 0.5f * scale.x * sinA;
	float scaleYSinA = 0.5f * scale.y * sinA;
	float scaleYCosA = 0.5f * scale.y * cosA;
	
	verts[0] = Vec3f( ( -scaleXCosA - scaleYSinA ), ( -scaleXSinA + scaleYCosA ), 0.0f );
	verts[1] = Vec3f( ( -scaleXCosA + scaleYSinA ), ( -scaleXSinA - scaleYCosA ), 0.0f );
	verts[2] = Vec3f( (  scaleXCosA - scaleYSinA ), (  scaleXSinA + scaleYCosA ), 0.0f );
	verts[3] = Vec3f( (  scaleXCosA + scaleYSinA ), (  scaleXSinA - scaleYCosA ), 0.0f );
	
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
	
	glPopMatrix();
}

void Controller::drawSmokes( const Vec3f &right, const Vec3f &up )
{
	BOOST_FOREACH( Smoke &smoke, mSmokes ){
		smoke.draw( right, up );
	}
}

void Controller::finish()
{
	for( list<Particle>::iterator it = mParticles.begin(); it != mParticles.end(); ++it )
	{
		it->finish();
	}
}

void Controller::reset()
{
	clearRoom();
	preset( mPreset );
}

void Controller::clearRoom()
{
	mParticles.clear();
	mPhotons.clear();
	mShockwaves.clear();
	mSmokes.clear();
}

void Controller::addParticles( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		Vec3f bounds = mRoom->getDims();
			
		Vec3f pos = Vec3f( Rand::randFloat( -bounds.x, bounds.x ),
						   Rand::randFloat( -bounds.y, bounds.y ),
						   Rand::randFloat( -bounds.z, bounds.z ) );
		Vec3f vel = Vec3f::zero();
		mParticles.push_back( Particle( pos, vel ) );
	}
}

void Controller::addParticle( float charge, const Vec3f &pos )
{
	mParticles.push_back( Particle( pos, Vec3f::zero(), charge ) );
}

void Controller::preset( int i )
{
	clearRoom();
	mRecycle	= false;
	mPreset		= i;
	
	float yFloor = mRoom->getFloorLevel() + 20.0f;
	
	
	if( i == 1 ){				// 2 Matter
		float d = 5.0f;
		addParticle( 1.0f, Vec3f( d, yFloor, 0.0f ) );
		addParticle( 1.0f, Vec3f(-d, yFloor, 0.0f ) );
		
	} else if( i == 2 ){		// 100 Matter
		float dim = 12.0f;
		for( int z=-12; z<12; z++ ){
			for( int x=-12; x<12; x++ ){
				float charge = 1.0f;
				Vec3f pos = Vec3f( (float)x * dim, yFloor, (float)z * dim );
				addParticle( charge, pos );
			}
		}
		
	} else if( i == 3 ){		// 1 Matter 1 Antimatter
		float d = 5.0f;
		addParticle( 1.0f, Vec3f( d, yFloor, 0.0f ) );
		addParticle(-1.0f, Vec3f(-d, yFloor, 0.0f ) );
		
	} else if( i == 4 ){		// Segregated grid
		float dim = 12.0f;
		for( int z=-4; z<4; z++ ){
			for( int x=-4; x<4; x++ ){
				float charge = 1.0f;
				if( x >= 0 ) charge = -1.0f;
				Vec3f pos = Vec3f( (float)x * dim, yFloor, (float)z * dim );
				addParticle( charge, pos );
			}
		}
	} else if( i == 5 ){		// 1 row of Matter 1 row of Antimatter
		for( int i=0; i<30; i++ ){
			float x1 = -20.0f;
			float x2 = 20.0f;
			float z  = ( (float)i - 15.0f ) * 15.0f;
			addParticle( 1.0, Vec3f( z, yFloor, x1 ) );
			addParticle(-1.0, Vec3f( z, yFloor, x2 ) );
		}
	} else if( i == 6 ){		// 2 concentric rings, 1 of Matter, 1 of Antimatter
		float numParticles = 100;	
		for( int k=0; k<2; k++ ){
			float charge = 1.0f;
			if( k == 1 ) charge = -1.0f;
			
			for( int i=0; i<numParticles; i++ ){
				float per	 = (float)i/(float)(numParticles);
				float angle  = per * M_PI * 2.0f;
				float cosA	 = cos( angle );
				float sinA	 = sin( angle );
				float radius = 100.0f + k * 25.0f;
				
				addParticle( charge, Vec3f( cosA * radius, yFloor, sinA * radius ) );
			}
		}
	} else if( i == 7 ){		// 2 concentric rings, 1 of Matter, 1 of Antimatter
		float numParticles = 200;	
		for( int k=0; k<2; k++ ){
			float charge = 1.0f;
			if( k == 1 ) charge = -1.0f;
			
			for( int i=0; i<numParticles; i++ ){
				float per	 = (float)i/(float)(numParticles);
				float angle  = per * M_PI * 2.0f;
				float cosA	 = cos( angle );
				float sinA	 = sin( angle );
				float radius = 180.0f + k * 25.0f;
				
				addParticle( charge, Vec3f( cosA * radius, yFloor, sinA * radius ) );
			}
		}
		
	} else if( i == 8 ){		// MATTER grid
		float dim = 16.0f;
		for( int z=-12; z<12; z++ ){
			for( int x=-12; x<12; x++ ){
				float charge = 1.0f;
				if( x < 3 && x > -3 && z < 3 && z > -3 ) charge = -1.0f;
				Vec3f pos = Vec3f( (float)x * dim, yFloor, (float)z * dim );
				addParticle( charge, pos );
			}
		}
	} else if( i == 9 ){		// RANDOM grid
		float dim = 16.0f;
		for( int z=-12; z<12; z++ ){
			for( int x=-12; x<12; x++ ){
				float charge = 1.0f;
				if( Rand::randBool() ) charge = -1.0f;
				Vec3f pos = Vec3f( (float)x * dim, yFloor, (float)z * dim );
				addParticle( charge, pos );
			}
		}
		
	} else if( i == 10 ){		// RANDOM grid with recycling
		float dim = 16.0f;
		for( int z=-12; z<12; z++ ){
			for( int x=-12; x<12; x++ ){
				float charge = 1.0f;
				if( Rand::randBool() ) charge = -1.0f;
				Vec3f pos = Vec3f( (float)x * dim, yFloor, (float)z * dim );
				addParticle( charge, pos );
			}
		}
		
//		float dim = 8.0f;
//		for( int z=-24; z<24; z++ ){
//			for( int x=-24; x<24; x++ ){
//				float charge = 1.0f;
//				if( Rand::randBool() ) charge = -1.0f;
//				Vec3f pos = Vec3f( (float)x * dim, -mRoom->getBounds().y + 20.0f, (float)z * dim );
//				addParticle( charge, pos );
//			}
//		}
		
		mRecycle = true;
	}
}





