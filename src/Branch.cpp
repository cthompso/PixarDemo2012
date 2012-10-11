#include "Branch.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;

Branch::Branch()
{
}

Branch::Branch( Vec2f loc, float radius, float time )
{
    mBirth = time;
    mAge = 0.0f;
    mDeath = 140.0f;
    speed = 0.0f;
    opacity = 1.0f;
	mLoc	= loc;
//	mDir	= Rand::randVec2f();
    mDir    = Vec2f(Rand::randFloat(-0.5f,0.5f),-1.0);
	mVel	= Rand::randFloat( 5.0f );
	mRadius	= 3.0f * radius;
}

void Branch::update()
{
	//mLoc += mDir * mVel;
    mAge += 1.0f;
    mVel *= 0.98f;
}

void Branch::draw()
{
    float b = 1.0f - (mAge/mDeath);
    ColorA myCol = ColorA(CM_HSV,fmod(mBirth,1.0f),1.0f,1.0f,1.0f*b);
    myCol = ColorA(1.0f,1.0f,1.0f,1.0f);
    gl::color(myCol);
	gl::drawSolidCircle( mLoc, mRadius, 32 );


//    opacity = 1 - mAge/mDeath;
//    mLoc.x += mDir.x * speed;
//    mLoc.y += mDir.y * speed;
//    beginShape();
//    float cx,cy;
//    cx = 0;
//    cy = 0;
//    float r = 10;
//    pushMatrix();
//    translate(x,y);
//    for (float i = 0; i < age * .1; i += .1) {
//        curveVertex(cx,cy);
//        cx += mDir.x + (r * cos(cx));
//        cy += mDir.y + (r * sin(cy));
//        r *= .96;
//        
//    }
//    endShape();
//    popMatrix();
}

bool Branch::Dead() {
    if ( mAge >= mDeath ) {
        return true;
    }
    return false;
}

void Branch::Kill() {
    mAge = mDeath;
}

void Branch::Move() {
    
}
