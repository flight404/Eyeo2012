/*
 * May 29, 2005
 */
 
#include "ParticleSystem.h"
#include "RungeKuttaIntegrator.h"
 
namespace traer { namespace physics {

    void ParticleSystem::setIntegrator( Integrator *i )
    {
        delete integrator;
        integrator = i;
    }
    
    void ParticleSystem::setGravity( float x, float y, float z )
    {
        gravity.set( x, y, z );
    }
    
    // default down gravity
    void ParticleSystem::setGravity( float g )
    {
        gravity.set( 0, g, 0 );
    }
    
    void ParticleSystem::setDrag( float d )
    {
        drag = d;
    }
    
    void ParticleSystem::tick()
    {
        tick( 1 );
    }
    
    void ParticleSystem::tick( float t )
    {  
        integrator->step( t );
    }
    
	Particle* ParticleSystem::makeParticle( float mass, float x, float y, float z )
    {
        Particle* p = new Particle( mass );
        p->mPos.set(x, y, z);
        mParticles.push_back( p );
        return p;
    }
	
    Particle* ParticleSystem::makeParticle( float mass, const ci::Vec3f &v, float shellRadius, float charge )
    {
        Particle* p = new Particle( mass );
        p->mPos = v;
		p->mShellRadius = shellRadius;
		p->mCharge = charge;
        mParticles.push_back( p );
        return p;
    }
    
    Particle* ParticleSystem::makeParticle()
    {  
        return makeParticle( 1.0f, 0.0f, 0.0f, 0.0f );
    }
    
    Spring* ParticleSystem::makeSpring( Particle* a, Particle* b, float ks, float d, float r )
    {
        Spring* s = new Spring( a, b, ks, d, r );
        mSprings.push_back( s );
        return s;
    }
    
    Attraction* ParticleSystem::makeAttraction( Particle* a, Particle* b, float k, float minDistance )
    {
        Attraction* m = new Attraction( a, b, k, minDistance );
        attractions.push_back( m );
        return m;
    }
    
    void ParticleSystem::clear()
    {
		for (int i = 0; i < mSprings.size(); i++) {
            delete mSprings[i];
        }
        for (int i = 0; i < mParticles.size(); i++) {
            delete mParticles[i];
        }
        for (int i = 0; i < attractions.size(); i++) {
            delete attractions[i];
        }
        for (int i = 0; i < customForces.size(); i++) {
            delete customForces[i];
        }
        mParticles.clear();
        mSprings.clear();
        attractions.clear();
        customForces.clear();
    }
    
    ParticleSystem::ParticleSystem( float g, float somedrag )
    {
        integrator = new RungeKuttaIntegrator( this );
        gravity.set( 0, g, 0 );
        drag = somedrag;
    }
    
    ParticleSystem::ParticleSystem( float gx, float gy, float gz, float somedrag )
    {
        integrator = new RungeKuttaIntegrator( this );
        gravity.set( gx, gy, gz );
        drag = somedrag;
    }
    
    ParticleSystem::ParticleSystem()
    {
        integrator = new RungeKuttaIntegrator( this );
        gravity.set( 0, DEFAULT_GRAVITY, 0 );
        drag = DEFAULT_DRAG;
    }
    
    void ParticleSystem::applyForces()
    {
        if ( gravity.length() > 0.0f )
        {
            for ( int i = 0; i < mParticles.size(); ++i )
            {
                Particle* p = mParticles[i];
                p->mAcc += gravity;
            }
        }
        
        for ( int i = 0; i < mParticles.size(); ++i )
        {
            Particle* p = mParticles[i];
            p->mAcc -= p->mVel * drag;
        }
        
        for ( int i = 0; i < mSprings.size(); i++ )
        {
            Spring* f = mSprings[i];
            f->apply();
        }
        
        for ( int i = 0; i < attractions.size(); i++ )
        {
            Attraction* f = attractions[i];
            f->apply();
        }
        
        for ( int i = 0; i < customForces.size(); i++ )
        {
            Force* f = customForces[i];
            f->apply();
        }
    }
    
    void ParticleSystem::clearForces()
    {
        for ( int i = 0; i < mParticles.size(); ++i )
        {
            Particle* p = mParticles[i];
            p->mAcc.set(0,0,0);
        }
    }
    
    int ParticleSystem::numberOfParticles()
    {
        return mParticles.size();
    }
    
    int ParticleSystem::numberOfSprings()
    {
        return mSprings.size();
    }
    
    int ParticleSystem::numberOfAttractions()
    {
        return attractions.size();
    }
    
    Particle* ParticleSystem::getParticle( int i )
    {
        return mParticles[i];
    }
    
    Spring* ParticleSystem::getSpring( int i )
    {
        return mSprings[i];
    }
    
    Attraction* ParticleSystem::getAttraction( int i )
    {
        return attractions[i];
    }
    
    void ParticleSystem::addCustomForce( Force* f )
    {
        customForces.push_back( f );
    }
    
    int ParticleSystem::numberOfCustomForces()
    {
        return customForces.size();
    }
    
    Force* ParticleSystem::getCustomForce( int i )
    {
        return customForces[i];
    }
    
    void ParticleSystem::removeCustomForce( int i )
    {
        Force* erased = customForces[i];
        customForces.erase( customForces.begin() + i );
        delete erased;
    }
    
    void ParticleSystem::removeParticle( Particle* p )
    {
        mParticles.erase( std::find(mParticles.begin(), mParticles.end(), p) );
        delete p;
    }

    void ParticleSystem::removeParticle( int i )
    {
        Particle* erased = mParticles[i];
        mParticles.erase( mParticles.begin() + i );
        delete erased;
    }    
    
    void ParticleSystem::removeSpring( int i )
    {
        Spring* erased = mSprings[i];
        mSprings.erase( mSprings.begin() + i );
        delete erased;
    }
    
    void ParticleSystem::removeAttraction( int i  )
    {
        Attraction* erased = attractions[i];
        attractions.erase( attractions.begin() + i );
        delete erased;
    }
    
    void ParticleSystem::removeAttraction( Attraction* s )
    {
        attractions.erase( std::find(attractions.begin(), attractions.end(), s) );
        delete s;
    }
    
    void ParticleSystem::removeSpring( Spring* a )
    {
        mSprings.erase( std::find(mSprings.begin(), mSprings.end(), a) );
        delete a;
    }
    
    void ParticleSystem::removeCustomForce( Force* f )
    {
        customForces.erase( std::find(customForces.begin(), customForces.end(), f) );
        delete f;
    }

} } // namespace traer::physics