#pragma once
#include "Branch.h"
#include <list>

class BranchController {
 public:
	BranchController();
	void update();
	void draw();
	void addBranches( int amt , float radius, float time);
	void removeBranches( int amt );
	
	std::list<Branch>	mBranches;
};
