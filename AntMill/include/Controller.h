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
#include "Room.h"
#include "Ant.h"
#include <vector>
#include <list>

#include "Food.h"

class Controller 
{
public:
	Controller();
	Controller( Room *room );
	void init( int maxAnts, int numSpecialAnts );
	void repelAnts();
	void update( float dt, bool step );
	void updateAnts( float dt );
	void annihilate( Food *f1, Food *f2 );
	void pickupFood( Ant *ant );
	void dropFood( Ant *ant );
	void draw();
	void drawAnts();
	void drawAntTails( const ci::Color &outBound, const ci::Color &inBound );
	void drawHome();
	void drawFoods();
	int getNumAnts(){ return mAnts.size(); };
	
	Room					*mRoom;
	
	std::vector<Ant>		mAnts;
	std::vector<Ant*>		mSpecialAnts;
	int						mNumSpecialAnts;
	
	std::vector<Food>		mFoods;
	
	ci::Vec3f				mHomePos;
	float					mHomeRadius, mHomeRadiusSqrd;
	
	ci::Vec3f				mFoodPos;
	float					mFoodRadius, mFoodRadiusSqrd;
};

bool depthSortFunc( Ant *a, Ant *b );

