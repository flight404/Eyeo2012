//
//  Room.cpp
//  Stillness
//
//  Created by Robert Hodgin on 12/20/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "Room.h"

const float MAX_TIMEMULTI	= 120.0f;
const float GRAVITY			= -0.02f;

using namespace ci;

Room::Room()
{
}

Room::Room( const Vec3f &dims, bool isPowerOn, bool isGravityOn )
{	
	// TIME
	mTime			= (float)app::getElapsedSeconds();
	mTimeElapsed	= 0.0f;
	mTimeMulti		= 60.0f;
	mTimer			= 0.0f;
	mTick			= false;
	
	mDims		= dims;
	mDimsDest	= dims;
	
	mIsPowerOn		= isPowerOn;
	if( mIsPowerOn ) mPower = 1.0f;
	else			 mPower = 0.0f;
	
	mIsGravityOn	= isGravityOn;
	mDefaultGravity = Vec3f( 0.0f, GRAVITY, 0.0f );
}

void Room::init()
{
	int						index;
	std::vector<uint32_t>	indices;
	std::vector<ci::Vec3f>	posCoords;
	std::vector<ci::Vec3f>	normals;
	std::vector<ci::Vec2f>	texCoords;
	
	float X = 1.0f;
	float Y = 1.0f;
	float Z = 1.0f;
	
	static Vec3f verts[8] = {
		Vec3f(-X,-Y,-Z ), Vec3f(-X,-Y, Z ), 
		Vec3f( X,-Y, Z ), Vec3f( X,-Y,-Z ),
		Vec3f(-X, Y,-Z ), Vec3f(-X, Y, Z ), 
		Vec3f( X, Y, Z ), Vec3f( X, Y,-Z ) };
	
	static GLuint vIndices[12][3] = { 
		{0,1,3}, {1,2,3},	// floor
		{4,7,5}, {7,6,5},	// ceiling
		{0,4,1}, {4,5,1},	// left
		{2,6,3}, {6,7,3},	// right
		{1,5,2}, {5,6,2},	// back
		{3,7,0}, {7,4,0} }; // front
	
	static Vec3f vNormals[6] = {
		Vec3f( 0, 1, 0 ),	// floor
		Vec3f( 0,-1, 0 ),	// ceiling
		Vec3f( 1, 0, 0 ),	// left	
		Vec3f(-1, 0, 0 ),	// right
		Vec3f( 0, 0,-1 ),	// back
		Vec3f( 0, 0, 1 ) };	// front
	
	static Vec2f vTexCoords[4] = {
		Vec2f( 0.0f, 0.0f ),
		Vec2f( 0.0f, 1.0f ), 
		Vec2f( 1.0f, 1.0f ), 
		Vec2f( 1.0f, 0.0f ) };
	
	static GLuint tIndices[12][3] = {
		{0,1,3}, {1,2,3},	// floor
		{0,1,3}, {1,2,3},	// ceiling
		{0,1,3}, {1,2,3},	// left
		{0,1,3}, {1,2,3},	// right
		{0,1,3}, {1,2,3},	// back
		{0,1,3}, {1,2,3} };	// front
	
	gl::VboMesh::Layout layout;
	layout.setStaticIndices();
	layout.setStaticPositions();
	layout.setStaticNormals();
	layout.setStaticTexCoords2d();
	
	index = 0;
	posCoords.clear();
	normals.clear();
	indices.clear();
	texCoords.clear();
	
	for( int i=0; i<12; i++ ){
		posCoords.push_back( verts[vIndices[i][0]] );
		posCoords.push_back( verts[vIndices[i][1]] );
		posCoords.push_back( verts[vIndices[i][2]] );
		indices.push_back( index++ );
		indices.push_back( index++ );
		indices.push_back( index++ );
		normals.push_back( vNormals[i/2] );
		normals.push_back( vNormals[i/2] );
		normals.push_back( vNormals[i/2] );
		texCoords.push_back( vTexCoords[tIndices[i][0]] );
		texCoords.push_back( vTexCoords[tIndices[i][1]] );
		texCoords.push_back( vTexCoords[tIndices[i][2]] );
	}
	
	//	std::cout << "posCoords size = " << posCoords.size() << std::endl;
	//	std::cout << "indices size = " << indices.size() << std::endl;
	//	std::cout << "normals size = " << normals.size() << std::endl;
	//	std::cout << "texCoords size = " << texCoords.size() << std::endl;	
	
	mVbo = gl::VboMesh( posCoords.size(), indices.size(), layout, GL_TRIANGLES );	
	mVbo.bufferIndices( indices );
	mVbo.bufferPositions( posCoords );
	mVbo.bufferNormals( normals );
	mVbo.bufferTexCoords2d( 0, texCoords );
	mVbo.unbindBuffers();
}

void Room::updateTime()
{
	float prevTime	= mTime;
	mTime			= (float)app::getElapsedSeconds();
	float dt		= mTime - prevTime;
	mTimeAdjusted	= dt * mTimeMulti;
	mTimeElapsed	+= mTimeAdjusted;
	
	mTimer += mTimeAdjusted;
	mTick = false;
	if( mTimer > 1.0f ){
		mTick = true;
		mTimer = 0.0f;
	}
}

void Room::update()
{
	if( mIsPowerOn )	mPower -= ( mPower - 1.0f ) * 0.2f;
	else				mPower -= ( mPower - 0.0f ) * 0.2f;
	
	if( mIsGravityOn )	mGravity -= ( mGravity - mDefaultGravity ) * 0.2f;
	else				mGravity -= ( mGravity - Vec3f::zero() ) * 0.2f;
	
	mDims -= ( mDims - mDimsDest ) * 0.1f;
	
	updateTime();
}

void Room::draw()
{
	gl::draw( mVbo );
}

void Room::adjustTimeMulti( float amt )
{
	mTimeMulti = constrain( mTimeMulti - amt, 0.0f, (float)MAX_TIMEMULTI );
}

float Room::getTimePer()
{
	return mTimeMulti/MAX_TIMEMULTI;
}

float Room::getTimeDelta()
{
	return mTimeAdjusted;
}

bool Room::getTick()
{
	return mTick;
}

float Room::getLightPower()
{
	float p = getPower() * 5.0f * M_PI;
	float lightPower = cos( p ) * 0.5f + 0.5f;	
	return lightPower;
}

Vec3f Room::getRandRoomPos()
{
	return Vec3f( Rand::randFloat( -mDims.x, mDims.x ) * 0.9f,
				  Rand::randFloat( -mDims.y, mDims.y ) * 0.9f,
				  Rand::randFloat( -mDims.z, mDims.z ) * 0.9f );
}

Vec3f Room::getRandCeilingPos()
{
	return Vec3f( Rand::randFloat( -mDims.x, mDims.x ) * 0.8f, 
				  mDims.y, 
				  Rand::randFloat( -mDims.z, mDims.z ) * 0.5f );
}

Vec3f Room::getCornerCeilingPos()
{
	return Vec3f( mDims.x, mDims.y,-mDims.z ) * 0.9f;
}

Vec3f Room::getCornerFloorPos()
{
	return Vec3f(-mDims.x,-mDims.y,-mDims.z ) * 0.9f;	
}

Vec3f Room::getLeftWallPos()
{
	return Vec3f( mDims.x, 0.0f, 0.0f ) * 0.9f;
}

float Room::getFloorLevel()
{
	return -mDims.y;
}

