#pragma once
#include "cinder/cairo/Cairo.h"

#include "Branch.h"
#include <list>

using namespace ci;
using namespace cinder;

class BranchController {
 public:
	BranchController();
	void update();
	void draw( cairo::Context &ctx );
	void addBranches( int amt , float radius, float time);
	void removeBranches( int amt );
	
	std::list<Branch>	mBranches;
};
