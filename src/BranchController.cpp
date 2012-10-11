#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "BranchController.h"

using namespace ci;
using std::list;

BranchController::BranchController()
{
}

void BranchController::update()
{
	for( list<Branch>::iterator p = mBranches.begin(); p != mBranches.end(); ++p ){
        if (p->Dead()) {
            p = mBranches.erase(p);
            continue;
        }
		p->update();
	}
}

void BranchController::draw()
{
	for( list<Branch>::iterator p = mBranches.begin(); p != mBranches.end(); ++p ){
		p->draw();
	}
}

void BranchController::addBranches( int amt, float radius, float time )
{
	for( int i=0; i<amt; i++ )
	{
		float x = Rand::randFloat( app::getWindowWidth() );
		float y = Rand::randFloat( app::getWindowHeight() );
		mBranches.push_back( Branch( Vec2f( x, y ), radius, time ) );
	}
}

void BranchController::removeBranches( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		mBranches.pop_back();
	}
}

