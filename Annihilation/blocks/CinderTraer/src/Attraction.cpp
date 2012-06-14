// attract positive repel negative

#include <cmath>
#include "cinder/Vector.h"
#include "Attraction.h"

namespace traer { namespace physics {
	
    Attraction::Attraction( Particle* _a, Particle* _b, float _k, float _distanceMin )
	{
		a = _a;
		b = _b;
		k = _k;
		on = true;
		distanceMin = _distanceMin;
		distanceMinSquared = distanceMin*distanceMin;
	}

	float Attraction::getMinimumDistance()
	{
		return distanceMin;
	}

	void Attraction::setMinimumDistance( float d )
	{
		distanceMin = d;
		distanceMinSquared = d*d;
	}

	void Attraction::turnOff()
	{
		on = false;
	}

	void Attraction::turnOn()
	{
		on = true;
	}

	void Attraction::setStrength( float _k )
	{
		k = k;
	}

	Particle* Attraction::getOneEnd()
	{
		return a;
	}

	Particle* Attraction::getTheOtherEnd()
	{
		return b;
	}

	void Attraction::apply()
	{
		if ( on && ( !a->mIsFixed || !b->mIsFixed ) )
		{
            ci::Vec3f a2b = a->mPos - b->mPos;

			float a2bDistanceSquared = a2b.lengthSquared();

			if ( a2bDistanceSquared < distanceMinSquared )
				a2bDistanceSquared = distanceMinSquared;

			float force = k * a->mMass * b->mMass / a2bDistanceSquared;

			// make unit vector
			
            //a2b /= sqrt(a2bDistanceSquared);
			
			// multiply by force 
			
			//a2b *= force;
            
            a2b *= Q_rsqrt(a2bDistanceSquared) * force;

			// apply
			
			if ( !a->mIsFixed )
				a->mAcc -= a2b;
			if ( !b->mIsFixed )
				b->mAcc += a2b;
		}
	}

	float Attraction::getStrength()
	{
		return k;
	}

	bool Attraction::isOn() const
	{
		return on;
	}

	bool Attraction::isOff() const
	{
		return !on;
	}

    float Attraction::Q_rsqrt( float number )
    {
        long i;
        float x2, y;
        const float threehalfs = 1.5F;
        
        x2 = number * 0.5F;
        y  = number;
        i  = * ( long * ) &y;                       // evil floating point bit level hacking [sic]
        i  = 0x5f3759df - ( i >> 1 );               // what the fuck? [sic]
        y  = * ( float * ) &i;
        y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
        //    y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
        
        return y;
    }

} } // namespace traer::physics