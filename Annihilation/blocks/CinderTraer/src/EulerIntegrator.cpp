#include "EulerIntegrator.h"
#include "Particle.h"

namespace traer { namespace physics {

	EulerIntegrator::EulerIntegrator( ParticleSystem* system )
	{
		s = system;
	}
	
	void EulerIntegrator::step( const float &t )
	{
		s->clearForces();
		s->applyForces();
		
		for ( int i = 0; i < s->numberOfParticles(); i++ )
		{
			Particle* p = s->getParticle( i );
			if ( !p->mIsFixed )
			{
                p->mVel += p->mAcc / (p->mMass*t);
                p->mPos += p->mVel / t;
			}
		}
	}

} } // namespace traer::physics