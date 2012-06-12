//
//  Controller.h
//  BubbleChamber
//
//  Created by Robert Hodgin on 5/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#include "Room.h"
#include "Node.h"
#include "Shockwave.h"
#include "Smoke.h"
#include "Glow.h"
#include <vector>
#include <list>

class Controller 
{
public:
	Controller();
	void init( Room *room, int gridDim );
	void createNodes( int gridDim );
	void createSphere( ci::gl::VboMesh &mesh, int res );
	void drawSphereTri( ci::Vec3f va, ci::Vec3f vb, ci::Vec3f vc, int div );
	void update( float dt, bool tick );
	void explode();
	void draw();
	void drawNodes( ci::gl::GlslProg *shader );
	void drawShockwaves( ci::gl::GlslProg *shader );
	void drawShockwaveCenters();
	void drawConnections();
	void drawSmokes( const ci::Vec3f &right, const ci::Vec3f &up );
	void drawGlows( const ci::Vec3f &right, const ci::Vec3f &up );
	void addSmokes( const ci::Vec3f &vec, int amt );
	void addGlows( const ci::Vec3f &vec, int amt );
	void clear();
	void preset( int index );
	int mPresetIndex;
	
	int	mGridDim;
	
	Room					*mRoom;
	
	std::vector<Node>		mNodes;
	std::vector<Node>		mNewNodes;
	std::vector<Shockwave>	mShockwaves;
	std::vector<Smoke>		mSmokes;
	std::vector<Glow>		mGlows;
	
	ci::gl::VboMesh			mSphereVbo;
	std::vector<ci::Vec3f>	mPosCoords;
	std::vector<ci::Vec3f>	mNormals;
};
bool depthSortFunc( Shockwave a, Shockwave b );
