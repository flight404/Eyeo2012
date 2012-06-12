//
//  Terrain.cpp
//  RDTerrain
//
//  Created by Robert Hodgin on 4/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Terrain.h"

using namespace ci;
using std::vector;

Terrain::Terrain()
{
}

Terrain::Terrain( int vboWidth, int vboHeight )
{
	mVboWidth		= vboWidth;
	mVboHeight		= vboHeight;

	// setup the parameters of the Vbo
	int totalVertices	= mVboWidth * mVboHeight;
	int totalQuads		= ( mVboWidth - 1 ) * ( mVboHeight - 1 );
	gl::VboMesh::Layout layout;
	layout.setStaticIndices();
	layout.setStaticPositions();
	layout.setStaticTexCoords2d();
	mVboMesh = gl::VboMesh( totalVertices, totalQuads * 4, layout, GL_QUADS );
	
	// buffer our static data - the texcoords and the indices
	vector<Vec3f>		positions;
	vector<uint32_t>	indices;
	vector<Vec2f>		texCoords;
	for( int x = 0; x < mVboWidth; ++x ) {
		for( int z = 0; z < mVboHeight; ++z ) {
			// create a quad for each vertex, except for along the bottom and right edges
			if( ( x + 1 < mVboWidth ) && ( z + 1 < mVboHeight ) ) {
				indices.push_back( (x+0) * mVboHeight + (z+0) );
				indices.push_back( (x+1) * mVboHeight + (z+0) );
				indices.push_back( (x+1) * mVboHeight + (z+1) );
				indices.push_back( (x+0) * mVboHeight + (z+1) );
			}
			// the texture coordinates are mapped to [0,1.0)
			texCoords.push_back( Vec2f( x / (float)mVboWidth, z / (float)mVboHeight ) );
			float xp = ( x - mVboWidth * 0.5f );
			float zp = ( z - mVboHeight * 0.5f );
			positions.push_back( Vec3f( xp, 0.0f, zp ) );
		}
	}
	
	mVboMesh.bufferPositions( positions );
	mVboMesh.bufferIndices( indices );
	mVboMesh.bufferTexCoords2d( 0, texCoords );
	mVboMesh.unbindBuffers();
}

void Terrain::setup( float scale )
{
	
}

void Terrain::update()
{
}

void Terrain::draw()
{
	gl::draw( mVboMesh );
}

float Terrain::getAltitude( const ci::Vec3f &pos )
{
	return 0.0f;
}


