
#include "Particle.h"

namespace traer { namespace physics {

    Particle::Particle( const float &m )
    {
        mMass		= m;
		mRadius		= 3.0f;
		mShellRadius = 10.0f;
		mCharge		= 1.0f;
        mAge		= 0;
		mId			= 1.0f;
		mAnchorPos.set(0,0,0);
        mPos.set(0,0,0);
        mVel.set(0,0,0);
        mAcc.set(0,0,0);
		mCol = ci::Color( 1.0f, 0.0f, 0.0f );
		
		mIsFixed	= false;
		mIsLocked	= false;
    }

    void Particle::reset()
    {
		mMass		= 50.0f;
		mRadius		= 3.0f;
		mShellRadius = 10.0f;
		mCharge		= 1.0f;
        mAge		= 0;
		mId			= 1.0f;
		mAnchorPos.set(0,0,0);
        mPos.set(0,0,0);
        mVel.set(0,0,0);
        mAcc.set(0,0,0);
		mCol = ci::Color( 1.0f, 0.0f, 0.0f );
		
        mIsFixed	= false;
		mIsLocked	= false;
    }

} } // namespace traer::physics