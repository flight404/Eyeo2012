//
//  Terrain.h
//  RDTerrain
//
//  Created by Robert Hodgin on 4/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/gl/Vbo.h"
#include "cinder/gl/GlslProg.h"

class Terrain {
  public:
	Terrain();
	Terrain( int vboWidth, int vboHeight );
	void setup( float scale );
	void update();
	void draw();
	float getAltitude( const ci::Vec3f &pos );
	
	int					mVboWidth, mVboHeight;
	ci::gl::VboMesh		mVboMesh;
};