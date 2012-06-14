// attract positive repel negative

#pragma once

#include "Force.h"
#include "Particle.h"

namespace traer { namespace physics {

class Attraction : public Force
{
public:

	Particle* a;
	Particle* b;
	float k;
	bool on;
	float distanceMin;
	float distanceMinSquared;
	
	Attraction( Particle* a, Particle* b, float k, float distanceMin );

	float getMinimumDistance();

	void setMinimumDistance( float d );

	void turnOff();

	void turnOn();

	void setStrength( float k );

	Particle* getOneEnd();

	Particle* getTheOtherEnd();

	void apply();

	float getStrength();

	bool isOn() const;
	
	bool isOff() const;
	
private:
    // http://en.wikipedia.org/wiki/Fast_inverse_square_root
    float Q_rsqrt( float number );
    
};

} } // namespace traer::physics