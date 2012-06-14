/*
 * May 29, 2005
 */

#pragma once

#include "Particle.h"
#include "Force.h"

namespace traer { namespace physics {

/**
 * @author jeffrey traer bernstein
 *
 */
class Spring : public Force
{

public:

    float springConstant;
    float damping;
    float restLength;
    Particle* a;
    Particle* b;
    bool on;
    
    Spring( Particle* A, Particle* B, const float &ks, const float &d, const float &r );
    
    void turnOff();
    
    void turnOn();
    
    bool isOn() const;
    
    bool isOff() const;
    
    Particle* getOneEnd() const;
    
    Particle* getTheOtherEnd() const;
    
    float currentLength() const;
    
    float getRestLength() const;
    
    float getStrength() const;
    
    void setStrength( const float &ks );
    
    float getDamping() const;
    
    void setDamping( const float &d );
    
    void setRestLength( const float &l );
    
    void apply();

protected:

    void setA( Particle* p );
    
    void setB( Particle* p );
  
};

} } // namespace traer::physics