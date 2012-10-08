#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;

Particle::Particle()
{
}

Particle::Particle( Vec2f loc, float radius, float time )
{
    mBirth = time;
    mAge = 0.0f;
    mDeath = 140.0f;

	mLoc	= loc;
	mDir	= Rand::randVec2f();
	mVel	= Rand::randFloat( 5.0f );
	mRadius	= 3.0f * radius;
}

void Particle::update()
{
	mLoc += mDir * mVel;
    mAge += 1.0f;
    mVel *= 0.98f;
}

void Particle::draw()
{
    float b = 1.0f - (mAge/mDeath);
    ColorA myCol = ColorA(CM_HSV,fmod(mBirth,1.0f),1.0f,1.0f,1.0f*b);
    gl::color(myCol);
	gl::drawSolidCircle( mLoc, mRadius );
}

bool Particle::Dead() {
    if ( mAge >= mDeath ) {
        return true;
    }
    return false;
}

void Particle::Kill() {
    mAge = mDeath;
}

void Particle::Move() {
    
}
