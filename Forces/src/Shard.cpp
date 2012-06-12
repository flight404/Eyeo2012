//
//  Shard.cpp
//  Sol
//
//  Created by Robert Hodgin on 12/10/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "Shard.h"
#include "cinder/CinderMath.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"
#include <boost/foreach.hpp>

#define TWO_PI 6.28318531

using namespace ci;
using std::vector;

Shard::Shard( Vec3f pos, Vec3f dir, float height, float radius )
: mPos( pos ), mDir( dir ), mHeight( height ), mRadius( radius )
{
}


void Shard::init( TriMesh *mesh, const Color &c )
{
	int numVerts = mesh->getNumVertices();
	
	float per = 0.2 - mHeight;
	float initAngle = Rand::randFloat( M_PI * per );
	
	float facetPer	= Rand::randFloat();
	if( mRadius < 0.03f || facetPer < 0.2f )
		facetPer = 0.0f;
	
	
	float uc1 = 0.0f;
	float uc2 = 1.0f;
	
	float vc1 = 0.0f;//Rand::randFloat( 0.15f );
	float vc2 = mHeight * 0.8f;//0.75f;
	float vc3 = mHeight * 0.8f;//1.0f;
	float vc4 = mHeight;//1.0f;
	
	mMid	= mPos + mDir * ( mHeight - Rand::randFloat( mRadius * 0.75f, mRadius * 1.25f ) * ( 1.0f - facetPer ) * 2.0f );
	mTip	= mPos + mDir * mHeight;
	
	Vec3f perp1 = mDir.cross( Vec3f::yAxis() );
	perp1.normalize();
	Vec3f perp2 = perp1.cross( mDir );
	perp2.normalize();
	perp1		= perp2.cross( mDir );
	perp1.normalize();
	
	float numSides = 6.0f;
	float invNumSides = 1.0f/numSides;
	vector<Vec3f> tipPositions;
	for( int i=0; i<numSides; i++ ){
		Vec3f tempV1, tempV2, tempV3, tempV4;
		Vec3f v1, v2, v3, v4;
		Vec2f t1, t2, t3, t4;

		for( int k=0; k<2; k++ ){
			float angle = initAngle + ( i + k ) * invNumSides * TWO_PI;
			float cosa	= cos( angle );
			float sina	= sin( angle );
			Vec3f offset = ( perp1 * cosa + perp2 * sina ) * mRadius;
			
			v1 = mPos + offset;
			v2 = mMid + offset;
			v3 = mMid + offset;
			v4 = mTip + offset * facetPer;
			
			if( k == 0 ){
				tempV1 = v1;
				tempV2 = v2;
				tempV3 = v3;
				tempV4 = v4;
			}
			
			mesh->appendVertex( v1 );
			mesh->appendVertex( v2 );
			mesh->appendVertex( v3 );
			mesh->appendVertex( v4 );
			
			if( k == 0 ){
				t1	= Vec2f( uc1, vc1 );
				t2	= Vec2f( uc1, vc2 );
				t3	= Vec2f( uc1, vc3 );
				t4	= Vec2f( uc1, vc4 );
			} else {
				t1	= Vec2f( uc2, vc1 );
				t2	= Vec2f( uc2, vc2 );
				t3	= Vec2f( uc2, vc3 );
				t4	= Vec2f( uc2, vc4 );
			}
			
			
			
			mesh->appendTexCoord( t1 );
			mesh->appendTexCoord( t2 );
			mesh->appendTexCoord( t3 );
			mesh->appendTexCoord( t4 );
			
			mesh->appendColorRGB( c );
			mesh->appendColorRGB( c );
			mesh->appendColorRGB( c );
			mesh->appendColorRGB( c );
		}
		
		Vec3f n1 = calcSurfaceNormal( tempV1, tempV2, v1 );
		Vec3f n2 = calcSurfaceNormal( tempV3, tempV4, v3 );
		
		
		mesh->appendNormal( n1 );
		mesh->appendNormal( n1 );
		mesh->appendNormal( n2 );
		mesh->appendNormal( n2 );
		
		mesh->appendNormal( n1 );
		mesh->appendNormal( n1 );
		mesh->appendNormal( n2 );
		mesh->appendNormal( n2 );
		
		tipPositions.push_back( v4 );
	}
	
	BOOST_FOREACH( Vec3f &v, tipPositions ){
		mesh->appendVertex( v );
		mesh->appendTexCoord( Vec2f( uc2, vc4 ) );
		mesh->appendColorRGB( c );
		mesh->appendNormal( mDir.normalized() );
	}

//	// base
//	mesh->appendTriangle( 0 + numVerts, 16 + numVerts,  8 + numVerts );
//	mesh->appendTriangle( 0 + numVerts, 24 + numVerts, 16 + numVerts );
//	mesh->appendTriangle( 0 + numVerts, 32 + numVerts, 24 + numVerts );
//	mesh->appendTriangle( 0 + numVerts, 40 + numVerts, 32 + numVerts );
	
	// sides
	for( int i=0; i<6; i++ ){
		int index = numVerts + i * 8;
		mesh->appendTriangle( index + 0, index + 1, index + 4 );
		mesh->appendTriangle( index + 1, index + 5, index + 4 );
		mesh->appendTriangle( index + 2, index + 3, index + 6 );
		mesh->appendTriangle( index + 3, index + 7, index + 6 );
	}
	
	// cap
	numVerts = mesh->getNumVertices();
	mesh->appendTriangle( numVerts - 1, numVerts - 3, numVerts - 2 );
	mesh->appendTriangle( numVerts - 1, numVerts - 4, numVerts - 3 );
	mesh->appendTriangle( numVerts - 1, numVerts - 5, numVerts - 4 );
	mesh->appendTriangle( numVerts - 1, numVerts - 6, numVerts - 5 );
}

Vec3f Shard::calcSurfaceNormal( const ci::Vec3f &p1, const ci::Vec3f &p2, const ci::Vec3f &p3 )
{
	ci::Vec3f normal;
	ci::Vec3f u = p1 - p3;
	ci::Vec3f v = p1 - p2;
	
	normal.x = u.y * v.z - u.z * v.y;
	normal.y = u.z * v.x - u.x * v.z;
	normal.z = u.x * v.y - u.y * v.x;
	return normal.normalized();
}

void Shard::update()
{
}

void Shard::draw()
{
}

