#include "cinder/Vector.h"
#include "ModifiedEulerIntegrator.h"

namespace traer { namespace physics {

	ModifiedEulerIntegrator::ModifiedEulerIntegrator( ParticleSystem *system )
	{
		s = system;
	}
	
	void ModifiedEulerIntegrator::step( const float &t )
	{
		s->clearForces();
		s->applyForces();
		
		const float halftt = 0.5f*t*t;
		
		for ( int i = 0; i < s->numberOfParticles(); i++ )
		{
			Particle* p = s->getParticle( i );
			if ( !p->mIsFixed )
			{
                const ci::Vec3f a = p->mAcc / p->mMass;				
                p->mPos += p->mVel/t;
                p->mPos += a * halftt;
                p->mVel += a / t;
			}
		}
	}

} } // namespace traer::physics