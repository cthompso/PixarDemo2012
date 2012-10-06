#pragma once
#include "Particle.h"
#include <list>

class ParticleController {
 public:
	ParticleController();
	void update();
	void draw();
	void addParticles( int amt , float radius, float time);
	void removeParticles( int amt );
	
	std::list<Particle>	mParticles;
};