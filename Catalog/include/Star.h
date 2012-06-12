#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Quaternion.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/Sphere.h"
#include <vector>

class Star {
public:
	Star( ci::Vec3f pos, 
		  float apparentMag,
		  float absoluteMag, 
		  float color, 
		  std::string name, 
		  std::string spectrum, 
		  const ci::Font &fontS, 
		  const ci::Font &fontM );
	void update( const ci::Camera &cam, float scale );
	void drawName( const ci::Vec2f &mousePos, float power, float alpha );

	ci::Vec3f	mInitPos;
	ci::Vec3f	mPos;
	ci::Sphere	mSphere;
	float		mColor;
	
	ci::Vec2f	mScreenPos;
	float		mScreenRadius;
	float		mDistToCam;
	float		mDistToCamPer;
	
	std::string		mName;
	ci::gl::Texture mNameTex;
	
	float		mRadius;
	float		mRadiusMulti;
	float		mMaxRadius;
	
	float		mApparentMag;
	float		mAbsoluteMag;
	
	float		mDistToMouse;
	
	bool		mIsSelected;
};
