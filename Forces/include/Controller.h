//
//  Controller.h
//  BubbleChamber
//
//  Created by Robert Hodgin on 5/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/TriMesh.h"
#include "Room.h"
#include "Particle.h"
#include "FieldLine.h"
#include "Glow.h"
#include "Glob.h"
#include "Nebula.h"
#include <vector>
#include <list>

class Controller 
{
  public:
	struct VboVertex {
        ci::Vec3f vertex;
//		ci::Vec3f normal;
        ci::Vec4f color;
    };
	
	Controller();
	void init( Room *room, int maxParticles );
	void initParticles();
	void applyForce();
	void update( const ci::Camera &cam, float dt, bool tick );
	void updateFieldLines( float dt, bool tick );
	void draw();
	void drawFieldLines( ci::gl::GlslProg *shader );
	void drawGlows( const ci::Vec3f &right, const ci::Vec3f &up );
	void drawNebulas( const ci::Vec3f &right, const ci::Vec3f &up );
	void drawCoronas( const ci::Vec3f &right, const ci::Vec3f &up );
	void drawGlobs( const ci::Vec3f &right, const ci::Vec3f &up );
	void drawShards( ci::gl::GlslProg *shader );
	void clear();
	void addParticle( const ci::Vec3f &pos, float charge );
	void addFieldLines( Particle *p, int amt );
	void addGlows( Particle *p, int amt );
	void addNebulas( Particle *p, int amt );
	void addGlobs( const ci::Vec3f &pos, int amt );
	void checkForParticleTouch( const ci::Vec2f &mousePos );
	void releaseDraggedParticles();
	void dragParticle( const ci::Vec2f &mousePos );
	
	Room					*mRoom;
	int						mMaxParticles;
	
	std::vector<Particle>	mParticles;
	Particle				*mDraggedParticle;
	
	std::vector<FieldLine>	mFieldLines;
	int						mTotalVerts;
    int						mPrevTotalVerts;
    VboVertex				*mFieldLineVerts;
	
	std::vector<Glow>		mGlows;
	std::vector<Nebula>		mNebulas;
	std::vector<Glob>		mGlobs;
};

