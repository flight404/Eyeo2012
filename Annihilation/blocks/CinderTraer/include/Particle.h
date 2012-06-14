#pragma once

#include "cinder/Vector.h"
#include "cinder/Color.h"

namespace traer { namespace physics {

class Particle {
  public:
    Particle( const float &m );
	void setLocked( bool b ){ mIsLocked = b; };
	void setFixed( bool b ){ mIsFixed = b; };
	float		mId;
	ci::Vec3f	mAnchorPos;
    ci::Vec3f	mPos;
    ci::Vec3f	mVel;
    ci::Vec3f	mAcc;
	ci::Color	mCol;
	float		mRadius;
    float		mMass, mInvMass;
	float		mShellRadius;
	float		mCharge;
    float		mAge;
    bool		mIsFixed;
	bool		mIsLocked;
    
    void reset();
  
};

} } // namespace traer::physics