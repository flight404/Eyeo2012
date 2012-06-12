//
//  RDiffusion.h
//  RDTerrain
//
//  Created by Robert Hodgin on 4/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/gl/Gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"

class RDiffusion {
  public:
	RDiffusion();
	RDiffusion( int fboWidth, int fboHeight );
	void			reset();
	void			update( float dt, ci::gl::GlslProg *shader,
							const ci::gl::Texture &glowTex, 
						    const bool &isPressed, 
						    const ci::Vec2f &mousePos,
						    float zoom );
	void			drawIntoHeightsFbo( ci::gl::GlslProg *shader, ci::Vec3f scale );
	void			drawIntoNormalsFbo( ci::gl::GlslProg *shader );
	void			draw();
	void			setMode( int index );
	ci::Vec2i		toFboVec( const ci::Vec3f &pos, float scale, float res );
	ci::gl::Texture getTexture();
	ci::gl::Texture getHeightsTexture();
	ci::gl::Texture getNormalsTexture();
	
	int					mFboWidth, mFboHeight;
	ci::Vec2f			mFboSize;
	ci::Area			mFboBounds;
	ci::Vec2f			mWindowSize;
	ci::Area			mWindowBounds;
	
	float				mXOffset, mYOffset;
	
	int					mThisFbo, mPrevFbo;
	ci::gl::Fbo			mFbo;
	ci::gl::Fbo			mFbos[2];
	ci::gl::Fbo			mHeightsFbo;
	ci::gl::Fbo			mNormalsFbo;

	float				mKernel[9];	
	ci::Vec2f			mOffset[9];

	float				mParamU;
	float				mParamV;
	float				mParamK;
	float				mParamF;
	float				mParamN;
	float				mParamWind;
};