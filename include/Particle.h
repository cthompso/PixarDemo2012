#pragma once
#include "cinder/Channel.h"
#include "cinder/Vector.h"

#include <vector>

class Particle {
 public:
	Particle();
	Particle( ci::Vec2f, float, float );
	void update();
	void draw();
    void Move();
    void Kill();
    bool Dead();
	
	ci::Vec2f	mLoc;
	ci::Vec2f	mDir;
	float		mVel;
	float       mAge;
	float		mRadius;
    float       mDeath;
    float       mBirth;
};