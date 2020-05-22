#ifndef FATEST
#define FATEST
#pragma once
#include "Global.h"

class FaultAttackTest
{
public:
	void addFAItemVector(std::vector<FAItem> Items);

private:
	void setFATest();
	std::vector<FAItem> Items;
	
};




#endif


