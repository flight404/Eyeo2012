#pragma once

#include <vector>
#include "cinder/Vector.h"
#include "Integrator.h"
#include "ParticleSystem.h"

namespace traer { namespace physics {

class RungeKuttaIntegrator : public Integrator
{

public:

	std::vector<ci::Vec3f> originalPositions;
	std::vector<ci::Vec3f> originalVelocities;
	std::vector<ci::Vec3f> k1Forces;
	std::vector<ci::Vec3f> k1Velocities;
	std::vector<ci::Vec3f> k2Forces;
	std::vector<ci::Vec3f> k2Velocities;
	std::vector<ci::Vec3f> k3Forces;
	std::vector<ci::Vec3f> k3Velocities;
	std::vector<ci::Vec3f> k4Forces;
	std::vector<ci::Vec3f> k4Velocities;
	
	ParticleSystem* s;
	
	RungeKuttaIntegrator( ParticleSystem* s );
	
	void allocateParticles();
	
	void step( const float &deltaT );

};

} } // namespace traer::physics