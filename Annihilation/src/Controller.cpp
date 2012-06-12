#include <boost/foreach.hpp>
#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "Controller.h"
#include "SpringCam.h"

using namespace ci;
using namespace traer::physics;
using std::list;
using std::vector;


#define K 0.25
#define DAMPING 0.1
#define NUM_SHELLS 4
#define TWO_PI 6.28318

Controller::Controller()
{
	mPrevTotalSparkVertices = -1;
	mTotalSparkVertices		= 0;
	mSparkVerts				= NULL;
	
	mDistanceMax	= 540.0f;
	mDistance		= mDistanceMax;
	mDistanceDest	= mDistanceMax;
	mDistPer		= 1.0f;
	
	mBroken = false;
}

void Controller::init( Room *room )
{
	mRoom			 = room;
	for( int i=0; i<NUM_SHELLS; i++ ){
		mRadii[i]	= (i+1) * ( 2.0f * (i+1));
	}
	
	// TRAER PHYSICS
	mPhysics		= new ParticleSystem( 0, 0.5f );
	mPhysics->clear();
	
	createSphere( mSphereLo, 0, false );
	createSphere( mSphereMd, 2, false );
	createSphere( mSphereHi, 3, true );

	makeParticles();
}

void Controller::makeParticles()
{
	// MAKE MATTER
	mMatter			= new Matter( Vec3f( mDistance * 0.5f, 0.0f, 0.0f ) );
	
	// MAKE ANTIMATTER
	mAntimatter		= new Antimatter( Vec3f( mDistance * -0.5f, 0.0f, 0.0f ) );
	
	// MAKE PARTICLES (HAIRS)
	mNumParticles	= mStartPositions.size();
	for( int i=0; i<mNumParticles; i++ ){
		Vec3f dir = mStartPositions[i];
		
		mHairs.push_back( Hair() );
		for( int k=0; k<NUM_SHELLS; k++ ){
			float radius = mAntimatter->mRadius + mRadii[k];
			float charge = k * k + 1.0f;
			Vec3f pos = mAntimatter->mPos + dir * radius;
			mPhysics->makeParticle( 1.0f, pos, radius, charge );
			Particle *newParticle = mPhysics->getParticle( mPhysics->numberOfParticles() - 1 );
			newParticle->mId = (float)k + 1.0f;
			if( k == 0 ){
				newParticle->mAnchorPos = dir;
				newParticle->mIsLocked = true;
			}
			
			mHairs.back().addNode( newParticle );
			mHairs.back().addRadii( 1.0f - ( k/(float)(NUM_SHELLS-1) ) );
			
			if( k > 0 ){
				Particle* p0 = mPhysics->getParticle( mPhysics->numberOfParticles() - 2 );
				Particle* p1 = newParticle;
				float dist = (p0->mPos-p1->mPos).length();
				mPhysics->makeSpring( p1, p0, K, DAMPING, dist * 0.25f );
				
				mHairs.back().addSpring( mPhysics->getSpring( mPhysics->numberOfSprings() - 1 ) );
			}
		}
	}
	
	std::cout << "NUM HAIRS     = " << mHairs.size() << std::endl;
	std::cout << "NUM PARTICLES = " << mPhysics->numberOfParticles() << std::endl;
}

void Controller::createSphere( gl::VboMesh &vbo, int res, bool savePositions )
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
		drawSphereTri( verts[triIndices[i][0]], verts[triIndices[i][1]], verts[triIndices[i][2]], res, savePositions );
	}
	vbo = gl::VboMesh( mPosCoords.size(), mIndices.size(), layout, GL_TRIANGLES );	
	vbo.bufferIndices( mIndices );
	vbo.bufferPositions( mPosCoords );
	vbo.bufferNormals( mNormals );
	vbo.bufferTexCoords2d( 0, mTexCoords );
}

void Controller::drawSphereTri( Vec3f va, Vec3f vb, Vec3f vc, int div, bool savePositions )
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
		
		if( savePositions && mStartPositions.size() < 250 && vn.z < 0.0f )
			mStartPositions.push_back( vn );
		
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
		drawSphereTri( va, vab, vac, div-1, savePositions );
		drawSphereTri( vb, vbc, vab, div-1, savePositions );
		drawSphereTri( vc, vac, vbc, div-1, savePositions );
		drawSphereTri( vab, vbc, vac, div-1, savePositions );
	}
}

void Controller::applyForces()
{
	float range = 115.0f;// + i*20.0f;
	range *= range;
		
	for( vector<Particle*>::iterator it1 = mPhysics->mParticles.begin(); it1 != mPhysics->mParticles.end(); ++it1 )
	{
		// APPLY SHOCKWAVE FORCES
		for( vector<Shockwave>::iterator shockIt = mShockwaves.begin(); shockIt != mShockwaves.end(); ++shockIt )
		{
			Vec3f dirToParticle = shockIt->mPos - (*it1)->mPos;
			float dist = dirToParticle.length();
			if( dist > shockIt->mRadiusPrev && dist < shockIt->mRadius ){
				Vec3f dirToParticleNorm = dirToParticle.normalized();
				(*it1)->mVel -= ( dirToParticleNorm * shockIt->mImpulse )/dist * 1000.0f;
				
				if( mRoom->getTick() ){
					for( int i=0; i<2; i++ ){
						mSparks.push_back( Spark( (*it1)->mPos, Rand::randVec3f() * 2.0f ) );
					}
				}
			}
		}
		

		// APPLY REPULSIVE FORCES
		vector<Particle*>::iterator it2 = it1;
		for( std::advance( it2, 1 ); it2 != mPhysics->mParticles.end(); ++it2 ) {
			Vec3f dir		= (*it1)->mPos - (*it2)->mPos;
			float distSqrd	= dir.lengthSquared();
			
			if( distSqrd < range ){
				float q			= ( (*it1)->mCharge * (*it2)->mCharge ) / distSqrd;
				dir				= dir.normalized() * q * 2.0f;
			
				(*it1)->mVel += dir;
				(*it2)->mVel -= dir;
			}
		}
		
		
		// APPLY ATTRACTIVE FORCES
		Vec3f dirToMatter	= (*it1)->mPos - mMatter->mPos;
		float distToMatter	= dirToMatter.length() - mMatter->mRadius * 0.125f;
		float q				= ( (*it1)->mCharge * mMatter->mCharge ) / (distToMatter*distToMatter);
		(*it1)->mVel -= dirToMatter.normalized() * q * 65.0f * mRoom->getPower();

		// APPLY REPULSIVE FORCE
		Vec3f dirToAntimatter	= (*it1)->mPos - mAntimatter->mPos;
		float distToAntimatter	= dirToAntimatter.length() - mAntimatter->mRadius * 0.125f;
		q						= ( (*it1)->mCharge * mAntimatter->mCharge ) / (distToAntimatter*distToAntimatter);
		(*it1)->mVel -= dirToAntimatter.normalized() * q * 365.0f * mRoom->getPower();
		
		// IF TOUCHING MATTER
		if( distToMatter < mMatter->mRadius * 1.05f ){
			Vec3f dir = (*it1)->mPos - mMatter->mPos;
			(*it1)->mPos = mMatter->mPos + dir.normalized() * mMatter->mRadius * 1.05f + dir * 0.1f;
			(*it1)->mVel *= -0.3f;
			if( mRoom->getPower() > 0.9f )
				zap( *it1 );
		}

		// IF JIGGLY
		if( mRoom->getPower() > 0.5f ){
			(*it1)->mPos += Rand::randVec3f();
		}
		
		// IF LOCKED
		if( (*it1)->mIsLocked ){
			Vec3f dir = (*it1)->mPos - mAntimatter->mPos;
			(*it1)->mPos = mAntimatter->mPos + (*it1)->mAnchorPos * (*it1)->mShellRadius;
		}
		
		if( (*it1)->mId > NUM_SHELLS - 1.0f )
			(*it1)->mVel += Vec3f::yAxis() * ( 1.0f - mRoom->getPower() ) * -0.9f;
	}
}

void Controller::zap( Particle *p )
{
	if( mRoom->getTick() ){
		Vec3f pos	= p->mPos * 0.9f + mMatter->mPos * 0.1f;
		Vec3f vel	= p->mVel;
		Vec3f dir	= p->mPos - mMatter->mPos;
		Vec3f dirNorm = dir.normalized();
		Vec3f axis	= dir.normalized();
		Vec3f right	= axis.cross( Vec3f::zAxis() ).normalized();
		Vec3f up	= axis.cross( right ).normalized();
		right		= axis.cross( up ).normalized();
		
		float lifespan	= 10.0f;
		float speed		= 15.0f;
		mShockwaves.push_back( Shockwave( pos, up, lifespan, speed ) );
		
		gl::drawBillboard( pos, Vec2f( 10.0f, 10.0f ), 0.0f, right, up );
		
		for( int i=0; i<8; i++ ){
			mSmokes.push_back( Smoke( pos, Vec3f::zero(), Rand::randFloat( 2.0f, 4.0f ), Rand::randFloat( 3.0f, 5.0f ) ) ); 
		}
		
		mGlows.push_back( Glow( mMatter->mPos + dirNorm * mMatter->mRadius, Vec3f::zero(), 20.0f, 10.0f, right, up ) );
		
		Vec3f sparkPos = mMatter->mPos + dirNorm * mMatter->mRadius;
		for( int i=0; i<50; i++ ){
			Vec3f d	= dirNorm + Rand::randVec3f();
			Vec3f v	= d * Rand::randFloat( 0.5f, 3.5f );
			mSparks.push_back( Spark( sparkPos, v ) );
		}
	}
}

void Controller::annihilate()
{
	mSmokes.clear();
	mGlows.clear();
	mShockwaves.clear();
	mHairs.clear();
	mSparks.clear();
	mPhysics->clear();
	delete mMatter;
	delete mAntimatter;
	
	mDistPer = 1.0f;
	mDistance = mDistanceMax;
	makeParticles();
	
	mBroken = true;
}


void Controller::update()
{
	float dt = mRoom->getTimeDelta();
	mDistanceDest = mDistanceMax * mDistPer;
	if( mDistance < mDistanceDest ){
		mDistance += 1.0f;
	} else {
		mDistance -= 1.0f;
	}

	if( mDistance < 100.0f ) annihilate();
	
	mMatter->mPos.x = mDistance * 0.5f;
	mAntimatter->mPos.x = mDistance * -0.5f;
	
	applyForces();
	
	updateSparks( dt );
	
	// GLOWS
	for( vector<Glow>::iterator it = mGlows.begin(); it != mGlows.end(); ){
		if( it->mIsDead ){
			it = mGlows.erase( it );
		} else {
			it->update( dt );
			it++;
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
	
	if( mRoom->getPower() < 0.2f ){
		mPhysics->tick( 2.5f );
	} else {
		mPhysics->tick( constrain( mRoom->mTimeMulti/60.0f, 0.002f, 1.0f ) );
	}
	
	mMatter->update( mRoom, dt );
	mAntimatter->update( mRoom, dt );
}

void Controller::updateSparks( float t )
{
	for( list<Spark>::iterator it = mSparks.begin(); it != mSparks.end(); ){
		if( it->mIsDead ){
			it = mSparks.erase( it );
		} else {
			it->update( t );
			it++;
		}
	}
	
	mTotalSparkVertices = mSparks.size();
	
    if (mTotalSparkVertices != mPrevTotalSparkVertices) {
        if (mSparkVerts != NULL) {
            delete[] mSparkVerts;
        }
        mSparkVerts = new SparkVertex[mTotalSparkVertices*2];
        mPrevTotalSparkVertices = mTotalSparkVertices;
    }
	
	int vIndex = 0;
	for( list<Spark>::iterator it = mSparks.begin(); it != mSparks.end(); ++it ){        
		mSparkVerts[vIndex].vertex = it->mPos; //0.7f
		mSparkVerts[vIndex].color  = Vec4f( 1.0f, 0.8f, 0.5f, it->mAgePer );
		vIndex++;
		mSparkVerts[vIndex].vertex = it->mPos - it->mVel; //0.7f
		mSparkVerts[vIndex].color  = Vec4f( 1.0f, 0.3f, 0.0f, 0.0f );
        vIndex++;
	}
}

void Controller::drawNodes( gl::GlslProg *shader )
{
	// draw vertices
    gl::color( Color( 1,1,1 ) );
    for ( int i=0; i<mPhysics->numberOfParticles(); ++i )
    {
        Particle* v = mPhysics->getParticle( i );
		if( v->mShellRadius > ( mRadii[NUM_SHELLS-1] - 0.1f + mAntimatter->mRadius ) ){
			shader->uniform( "pos", v->mPos );
			shader->uniform( "radius", 1.0f );
			gl::draw( mSphereLo );
		}
    }
}

void Controller::drawSprings()
{
	glBegin( GL_LINES );
    for ( int i = 0; i < mPhysics->numberOfSprings(); ++i )
    {
        Spring* e	= mPhysics->getSpring( i );
        Particle* a = e->getOneEnd();
        Particle* b = e->getTheOtherEnd();
		gl::vertex( a->mPos );
		gl::vertex( b->mPos );
    }
	glEnd();
}

void Controller::draw( gl::GlslProg *shader )
{

}

void Controller::drawMatter()
{
	mMatter->draw( mSphereHi );
}

void Controller::drawAntimatter()
{
	mAntimatter->draw( mSphereHi );
}

void Controller::drawMatterShadow( Room *room )
{
	float xWall = room->getDims().x;
	
	// DRAW PARTICLES
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );

	float xBase				= mMatter->mPos.x - mMatter->mRadius;
	float xDistFromWall		= xWall + xBase;
	float maxDist			= mMatter->mRadius * 4.0f;
		
	float perDist			= xDistFromWall/maxDist;
		
	if( perDist < 1.0f ){
		gl::color( ColorA( 0.0f, 0.0f, 0.0f, 1.0f - perDist ) );
		gl::drawBillboard( Vec3f( -xWall, mMatter->mPos.y, mMatter->mPos.z ), Vec2f( mMatter->mRadius, mMatter->mRadius ) * ( 4.0f + perDist * 15.0f ), 0.0f, Vec3f::yAxis(), Vec3f::zAxis() );
	}
}

void Controller::drawShockwaves( const Vec3f &camPos )
{
	for( vector<Shockwave>::iterator it = mShockwaves.begin(); it != mShockwaves.end(); ++it ){
		if( it->mAge < 1.5f ){
			gl::color( ColorA( 1.0f, 0.0f, 0.0f, 1.0f ) );
			drawSphericalBillboard( camPos, it->mPos, Vec2f( 1000.0f, 1000.0f ), 0.0f );
			gl::color( ColorA( 0.0f, 0.0f, 1.0f, 1.0f ) );
			drawSphericalBillboard( camPos, it->mPos, Vec2f( 400.0f, 400.0f ), 0.0f );
		} else if( it->mAge < 3.5f ){
			gl::color( ColorA( 1.0f, 0.6f, 0.05f, 1.0f ) );
			drawSphericalBillboard( camPos, it->mPos, Vec2f( 245.0f, 42.0f ), 0.0f );
		} else if( it->mAge < 5.5f ){
			gl::color( ColorA( 0.05f, 0.05f, 1.0f, 1.0f ) );
			drawSphericalBillboard( camPos, it->mPos, Vec2f( 825.0f, 125.0f ), 0.0f );
		}
	}
}

void Controller::drawSparks()
{
	if( mTotalSparkVertices > 0 ){
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_COLOR_ARRAY );
		glVertexPointer( 3, GL_FLOAT, sizeof(SparkVertex), mSparkVerts );
		glColorPointer( 4, GL_FLOAT, sizeof(SparkVertex), &mSparkVerts[0].color );
		
		glDrawArrays( GL_LINES, 0, mTotalSparkVertices );
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_COLOR_ARRAY );
	}
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
}

void Controller::drawSmokes( const Vec3f &right, const Vec3f &up )
{
	BOOST_FOREACH( Smoke &smoke, mSmokes ){
		smoke.draw( right, up );
	}
}
						   
void Controller::drawGlows()
{
	BOOST_FOREACH( Glow &glow, mGlows ){
		glow.draw();
	}
}

void Controller::drawHairs()
{
	mHairMesh.clear();
	
	int numFacets		= 6;
	float invNumFacets	= 1.0f/(float)numFacets;
	int numSegments		= NUM_SHELLS-1;
	float invNumSegments= 1.0f/(float)numSegments;
	
	BOOST_FOREACH( Hair &hair, mHairs ){
		for( int s=0; s<numSegments; s++ ){
			float sPer	= (float)s * invNumSegments;
			
			Vec3f p0 = hair.getPos(s);
			Vec3f p1 = hair.getPos(s+1);
			
			Vec3f dir = p1 - p0;
			dir.normalize();
			Vec3f perp1 = dir.cross( Vec3f::zAxis() );
			perp1.normalize();
			Vec3f perp2 = dir.cross( perp1 );
			perp2.normalize();
			
			for( int f=0; f<numFacets; f++ ){
				float per	= (float)f * invNumFacets;
				float angle = hair.mInitRot + per * TWO_PI;
				float cosa	= cos( angle );
				float sina	= sin( angle );
				Vec3f offset  = ( perp1 * cosa + perp2 * sina );
				
				mHairMesh.appendVertex( p0 + offset * 14.0f * hair.getRadii(s) );
				mHairMesh.appendNormal( offset );
				mHairMesh.appendTexCoord( Vec2f( per, sPer ) );
			}
		}
		mHairMesh.appendVertex( hair.getTipPos() );
		mHairMesh.appendNormal( hair.getTipDir() );
		mHairMesh.appendTexCoord( Vec2f( 1.0f, 1.0f ) );
	}
	
	
	int index = 0;
	for( int i=0; i<mHairs.size(); i++ ){
		for( int s=0; s<NUM_SHELLS-2; s++ ){
			mHairMesh.appendTriangle( index + 0, index + 6, index + 1 );
			mHairMesh.appendTriangle( index + 6, index + 7, index + 1 );
			mHairMesh.appendTriangle( index + 1, index + 7, index + 2 );
			mHairMesh.appendTriangle( index + 7, index + 8, index + 2 );
			mHairMesh.appendTriangle( index + 2, index + 8, index + 3 );
			mHairMesh.appendTriangle( index + 8, index + 9, index + 3 );
			mHairMesh.appendTriangle( index + 3, index + 9, index + 4 );
			mHairMesh.appendTriangle( index + 9, index +10, index + 4 );
			mHairMesh.appendTriangle( index + 4, index +10, index + 5 );
			mHairMesh.appendTriangle( index +10, index +11, index + 5 );
			mHairMesh.appendTriangle( index + 5, index +11, index + 0 );
			mHairMesh.appendTriangle( index +11, index + 6, index + 0 );
			index += numFacets;
		}
		index += numFacets;

		mHairMesh.appendTriangle( index, index - 1, index - 2 );
		mHairMesh.appendTriangle( index, index - 2, index - 3 );
		mHairMesh.appendTriangle( index, index - 3, index - 4 );
		mHairMesh.appendTriangle( index, index - 4, index - 5 );
		mHairMesh.appendTriangle( index, index - 5, index - 6 );
		mHairMesh.appendTriangle( index, index - 6, index - 1 );
		index ++;
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


