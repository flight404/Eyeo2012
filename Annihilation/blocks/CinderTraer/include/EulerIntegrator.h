#pragma once

#include "Integrator.h"
#include "ParticleSystem.h"

namespace traer { namespace physics {

class EulerIntegrator : public Integrator
{
public:

	ParticleSystem* s;
	
	EulerIntegrator( ParticleSystem* s );
	
	void step( const float &t );

};

} } // namespace traer::physics