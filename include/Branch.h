#pragma once
#include "cinder/Channel.h"
#include "cinder/Vector.h"

#include <vector>

class Branch {
 public:
	Branch();
	Branch( ci::Vec2f, float, float );
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
    float       opacity;
    float       speed;
};

//class Branch {
//public:
//    Branch(int xPos, int yPos);
//    ~Branch();
//    bool isDead ();
//    void display ();
//private:
//    float age;
//    float maxAge = 100;
//    int birth;
//    int x;
//    int y;
//    int px;
//    int py;
//    Vec2f dir;
//    float opacity;
//    float speed = 0.00;
//};
