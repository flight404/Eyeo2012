//
//  Room.h
//  Stillness
//
//  Created by Robert Hodgin on 01/01/12.


#pragma once
#include "cinder/gl/Vbo.h"

class Room
{
  public:
	Room();
	Room( const ci::Vec3f &dims, bool isPowerOn, bool isGravityOn );
	void		init();
	void		updateTime();
	void		update();
	void		draw();
	void		drawWalls( float mainPower, 
						  const ci::gl::Texture &backTex, 
						  const ci::gl::Texture &leftTex, 
						  const ci::gl::Texture &rightTex, 
						  const ci::gl::Texture &ceilingTex,
						  const ci::gl::Texture &floorTex,
						  const ci::gl::Texture &blankTex );
	
	void		setDims( const ci::Vec3f &dims ){ mDims = dims; };
	ci::Vec3f	getDims(){ return mDims; };
	ci::Vec3f	getRandCeilingPos();
	ci::Vec3f	getCornerCeilingPos();
	ci::Vec3f	getCornerFloorPos();
	float		getFloorLevel();
	
	void		adjustTimeMulti( float amt );
	float		getTimePer();
	float		getTimeDelta();
	bool		getTick();
	
	void		togglePower(){		mIsPowerOn = !mIsPowerOn;		};
	float		getPower(){			return mPower;					};
	bool		isPowerOn(){		return mIsPowerOn;				};
	
	float		getLightPower();
	
	void		toggleGravity(){	mIsGravityOn = !mIsGravityOn;	};
	ci::Vec3f	getGravity(){		return mGravity;				};
	bool		isGravityOn(){		return mIsGravityOn;			};
	
	ci::gl::VboMesh mVbo;
	
	// TIME
	float			mTime;				// Time elapsed in real world seconds
	float			mTimeElapsed;		// Time elapsed in simulation seconds
	float			mTimeMulti;			// Speed at which time passes
	float			mTimeAdjusted;		// Amount of time passed between last frame and current frame
	float			mTimer;				// A resetting counter for determining if a Tick has occured
	bool			mTick;				// Tick (aka step) for triggering discrete events
	
	// DIMENSIONS
	ci::Vec3f		mDims;				// Hesitant to rename this to 'bounds'. Might make it too easy to
	ci::Vec3f		mDimsDest;			// confuse with mRoomFbo.getBounds() which would return the Fbo Area.
	
	// POWER
	bool			mIsPowerOn;			// Power ranges from 0.0 to 1.0.
	float			mPower;
	
	// GRAVITY
	ci::Vec3f		mGravity;			// Gravity vector
	ci::Vec3f		mDefaultGravity;	// The default Gravity vector, in case you want to mess with gravity
										// but always be able to get back to the original condition.
	bool			mIsGravityOn;		// For turning Gravity on and off
};