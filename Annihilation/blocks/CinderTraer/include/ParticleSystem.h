/*
 * May 29, 2005
 */
 
#pragma once

#include <vector>
#include "cinder/Vector.h"
#include "Integrator.h"
#include "Spring.h"
#include "Attraction.h"
#include "Force.h"

#define RUNGE_KUTTA 0
#define MODIFIED_EULER 1

#define DEFAULT_GRAVITY  0
#define DEFAULT_DRAG 0.001f

namespace traer { namespace physics {

class ParticleSystem
{
public:

    // FIXME C++: these are public so that Integrator can see them
    // how to hide from everyone else without onerous accessors?
    std::vector<Particle*> mParticles;
    std::vector<Spring*> mSprings;
    std::vector<Attraction*> attractions;
    std::vector<Force*> customForces;
    
    Integrator* integrator;
    ci::Vec3f gravity;
    float drag;    
    
    ParticleSystem();
    
    ParticleSystem( float g, float somedrag );
    
    ParticleSystem( float gx, float gy, float gz, float somedrag );
    
    void setIntegrator( Integrator *i );
  
    void setGravity( float x, float y, float z );
  
    // default down gravity
    void setGravity( float g );
    
    void setDrag( float d );
    
    void tick();
    
    void tick( float t );

    void clear();
    
    Particle* makeParticle();
    
    Particle* makeParticle( float mass, float x, float y, float z );

    Particle* makeParticle( float mass, const ci::Vec3f &v, float shellRadius, float charge );
	
    Spring* makeSpring( Particle* a, Particle* b, float ks, float d, float r );
    
    Attraction* makeAttraction( Particle* a, Particle* b, float k, float minDistance );

    void addCustomForce( Force* f );
    
    int numberOfParticles();
    
    int numberOfSprings();
    
    int numberOfAttractions();
    
    int numberOfCustomForces();
    
    Particle* getParticle( int i );
    
    Spring* getSpring( int i );
    
    Attraction* getAttraction( int i );
    
    Force* getCustomForce( int i );
    
    void removeParticle( int i );
    
    void removeSpring( int i );
    
    void removeAttraction( int i  );

    void removeCustomForce( int i );
    
    void removeParticle( Particle* p );
    
    void removeAttraction( Attraction* s );
    
    void removeSpring( Spring* a );
    
    void removeCustomForce( Force* f );
  
    // FIXME C++: in Java things in the same package can access protected members
    // what's the C++ OOP equivalent for hiding these methods? should the Integrator own them?
//protected: 

    void applyForces();

    void clearForces();
    
};

} } // namespace traer::physics