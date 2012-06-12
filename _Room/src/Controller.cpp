//
//  Controller.cpp
//  PROTOTYPE

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "Controller.h"

using namespace ci;
using std::vector;
using std::list;

Controller::Controller()
{
}

void Controller::init( Room *room )
{
	mRoom					= room;
}

void Controller::update( float dt, bool tick )
{
}

void Controller::draw()
{
}
