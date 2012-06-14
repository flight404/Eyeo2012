#include "RungeKuttaIntegrator.h"

namespace traer { namespace physics {

	RungeKuttaIntegrator::RungeKuttaIntegrator( ParticleSystem* system )
	{
		s = system;
	}
	
	void RungeKuttaIntegrator::allocateParticles()
	{
		while ( s->mParticles.size() > originalPositions.size() )
		{
			originalPositions.push_back( ci::Vec3f() );
			originalVelocities.push_back( ci::Vec3f() );
			k1Forces.push_back( ci::Vec3f() );
			k1Velocities.push_back( ci::Vec3f() );
			k2Forces.push_back( ci::Vec3f() );
			k2Velocities.push_back( ci::Vec3f() );
			k3Forces.push_back( ci::Vec3f() );
			k3Velocities.push_back( ci::Vec3f() );
			k4Forces.push_back( ci::Vec3f() );
			k4Velocities.push_back( ci::Vec3f() );
		}
	}
	
	void RungeKuttaIntegrator::step( const float &deltaT )
	{	
		allocateParticles();
		/////////////////////////////////////////////////////////
		// save original position and velocities
		
		for ( int i = 0; i < s->mParticles.size(); ++i )
		{
			Particle* p = s->mParticles[i];
			if ( !p->mIsFixed )
			{		
				originalPositions[i].set( p->mPos );
				originalVelocities[i].set( p->mVel );
			}
			
			p->mAcc.set(0,0,0);	// and clear the forces
		}
		
		////////////////////////////////////////////////////////
		// get all the k1 values
		
		s->applyForces();
		
		// save the intermediate forces
		for ( int i = 0; i < s->mParticles.size(); ++i )
		{
			Particle* p = s->mParticles[i];
			if ( !p->mIsFixed )
			{
				k1Forces[i].set( p->mAcc );
				k1Velocities[i].set( p->mVel );                
			}
			
			p->mAcc.set(0,0,0);	// and clear the forces
		}
		
		////////////////////////////////////////////////////////////////
		// get k2 values
		
		for ( int i = 0; i < s->mParticles.size(); ++i )
		{
			Particle* p = s->mParticles[i];
			if ( !p->mIsFixed )
			{
				ci::Vec3f originalPosition = originalPositions[i];
				ci::Vec3f k1Velocity = k1Velocities[i];
				
                p->mPos.set( originalPosition + (k1Velocity * (0.5f * deltaT)) );
				
				ci::Vec3f originalVelocity = originalVelocities[i];
				ci::Vec3f k1Force = k1Forces[i];

				p->mVel.set( originalVelocity + k1Force * 0.5f * deltaT / p->mMass );
			}
		}
		
		s->applyForces();

		// save the intermediate forces
		for ( int i = 0; i < s->mParticles.size(); ++i )
		{
			Particle* p = s->mParticles[i];
			if ( !p->mIsFixed )
			{
				k2Forces[i].set( p->mAcc );
				k2Velocities[i].set( p->mVel );                
			}
			
			p->mAcc.set(0,0,0);	// and clear the forces now that we are done with them
		}
		
		
		/////////////////////////////////////////////////////
		// get k3 values
		
		for ( int i = 0; i < s->mParticles.size(); ++i )
		{
			Particle* p = s->mParticles[i];
			if ( !p->mIsFixed )
			{
                ci::Vec3f originalPosition = originalPositions[i];
                ci::Vec3f k2Velocity = k2Velocities[i];
				
                p->mPos.set( originalPosition + k2Velocity * 0.5f * deltaT );
				
                ci::Vec3f originalVelocity = originalVelocities[i];
                ci::Vec3f k2Force = k2Forces[i];

                p->mVel.set( originalVelocity + k2Force * 0.5f * deltaT / p->mMass );
			}
		}
        
		s->applyForces();
		
		// save the intermediate forces
		for ( int i = 0; i < s->mParticles.size(); ++i )
		{
			Particle* p = s->mParticles[i];
			if ( !p->mIsFixed )
			{
				k3Forces[i].set( p->mAcc );
				k3Velocities[i].set( p->mVel );                
			}
			
			p->mAcc.set(0,0,0);	// and clear the forces now that we are done with them
		}
		
		
		//////////////////////////////////////////////////
		// get k4 values
		
		for ( int i = 0; i < s->mParticles.size(); ++i )
		{
			Particle* p = s->mParticles[i];
			if ( !p->mIsFixed )
			{
                ci::Vec3f originalPosition = originalPositions[i];
				ci::Vec3f k3Velocity = k3Velocities[i];
				
				p->mPos.set( originalPosition + k3Velocity * deltaT);
				
				ci::Vec3f originalVelocity = originalVelocities[i];
				ci::Vec3f k3Force = k3Forces[i];

				p->mVel.set( originalVelocity + k3Force * deltaT / p->mMass );
			}
		}
		
		s->applyForces();

		// save the intermediate forces
		for ( int i = 0; i < s->mParticles.size(); ++i )
		{
			Particle* p = s->mParticles[i];
			if ( !p->mIsFixed )
			{
				k4Forces[i].set( p->mAcc );
				k4Velocities[i].set( p->mVel );                
			}			
		}
		
		
		/////////////////////////////////////////////////////////////
		// put them all together and what do you get?
		
		for ( int i = 0; i < s->mParticles.size(); ++i )
		{
			Particle* p = s->mParticles[i];
            p->mAge += deltaT;
			if ( !p->mIsFixed )
			{
				// update position
				
                ci::Vec3f originalPosition = originalPositions[i];
				ci::Vec3f k1Velocity = k1Velocities[i];
				ci::Vec3f k2Velocity = k2Velocities[i];
				ci::Vec3f k3Velocity = k3Velocities[i];
				ci::Vec3f k4Velocity = k4Velocities[i];
				
				p->mPos.set( originalPosition + deltaT / 6.0f * ( k1Velocity + 2.0f*k2Velocity + 2.0f*k3Velocity + k4Velocity ) );
				
				// update velocity
				
                ci::Vec3f originalVelocity = originalVelocities[i];
				ci::Vec3f k1Force = k1Forces[i];
				ci::Vec3f k2Force = k2Forces[i];
				ci::Vec3f k3Force = k3Forces[i];
				ci::Vec3f k4Force = k4Forces[i];
				
                p->mVel.set( originalVelocity + deltaT / ( 6.0f * p->mMass ) * ( k1Force + 2.0f*k2Force + 2.0f*k3Force + k4Force ) );
			}
		}
	}

} } // namespace traer::physics