//
//  Controller.h
//  BubbleChamber
//
//  Created by Robert Hodgin on 5/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/gl/Gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#include "cinder/Camera.h"
#include "cinder/Perlin.h"
#include "Room.h"
#include "Confetti.h"
#include "Streamer.h"
#include "Balloon.h"
#include "Shockwave.h"
#include <vector>
#include <list>

class Controller 
{
  public:
	Controller();
	void init( Room *room );
	void modVert( ci::Vec3f *v );
	void createSphere( ci::gl::VboMesh &mesh, int res );
	void drawSphereTri( ci::Vec3f va, ci::Vec3f vb, ci::Vec3f vc, int div );
	void bang();
	void checkForBalloonPop( const ci::Vec2f &mousePos );
	void update( const ci::Camera &cam );
	void applyBalloonCollisions();
	bool didParticlesCollide( const ci::Vec3f &dir, const ci::Vec3f &dirNormal, const float dist, const float sumRadii, const float sumRadiiSqrd, ci::Vec3f *moveVec );
	void draw();
	void drawConfettis( ci::gl::GlslProg *shader );
	void drawStreamers();
	void drawBalloons( ci::gl::GlslProg *shader );
	void drawPhysics();
	void addConfettis( int amt, const ci::Vec3f &pos, float speedMulti );
	void addStreamers( int amt, const ci::Vec3f &pos );
	void addBalloons( int amt, const ci::Vec3f &pos );
	void clear();
	void preset( int index );
	int						mPresetIndex;
	
	Room					*mRoom;
	ci::Perlin				mPerlin;
	
	float					mTimeSinceBang;
	
	std::vector<Confetti>	mConfettis;
	std::vector<Streamer>	mStreamers;
	std::vector<Balloon>	mBalloons;
	std::vector<Shockwave>	mShockwaves;
	
	ci::gl::VboMesh			mBalloonsVbo;
	std::vector<ci::Vec3f>	mPosCoords;
	std::vector<ci::Vec3f>	mNormals;
};
bool depthSortFunc( Balloon a, Balloon b );

