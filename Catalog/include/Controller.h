#pragma once
#include "cinder/gl/Gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#include "Room.h"
#include "Star.h"
#include "Glow.h"
#include "Nebula.h"

#include <vector>

class Controller {
  public:
	Controller();
	void init( Room *room );
	void update( float dt );
	void drawGlows( ci::gl::GlslProg *shader, const ci::Vec3f &right, const ci::Vec3f &up );
	void drawNebulas( ci::gl::GlslProg *shader, const ci::Vec3f &right, const ci::Vec3f &up );
	void clear();
	void addGlows( const Star &star, int amt );
	void addNebulas( const Star &star, int amt );
	
	Room					*mRoom;
	
	std::vector<Glow>		mGlows;
	std::vector<Nebula>		mNebulas;
};
