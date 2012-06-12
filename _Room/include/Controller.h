//
//  Controller.h
//  PROTOTYPE

#pragma once
#include "Room.h"
#include <vector>
#include <list>

class Controller 
{
public:
	Controller();
	void init( Room *room );
	void update( float dt, bool tick );
	void draw();
	void clear();

	Room					*mRoom;
};

