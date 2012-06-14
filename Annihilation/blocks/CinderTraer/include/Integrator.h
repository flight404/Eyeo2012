
#pragma once

namespace traer { namespace physics {

class Integrator 
{
public:
	virtual void step( const float &t ) = 0;
};

} } // namespace traer::physics