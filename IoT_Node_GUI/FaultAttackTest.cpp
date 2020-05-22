#include "FaultAttackTest.h"

extern std::vector<MyItem*> GlobalItemVector;
void FaultAttackTest::addFAItemVector(std::vector<FAItem> Items)
{
	for (int i=0;i<Items.size();i++)
	{
		//从vector里一个个把action设置好，需要设置recv和Consensus两个类，send暂不设置，因为对实验没有影响
		switch (Items[i].action)
		{
		case 0:
			GlobalItemVector[Items[i].index]->receiver->addFAAction(0, Items[i].startTime, Items[i].endTime);
			GlobalItemVector[Items[i].index]->consensus->addFAAction(0, Items[i].startTime, Items[i].endTime);
			break;
		case 1:
			GlobalItemVector[Items[i].index]->receiver->addFAAction(1, Items[i].startTime, Items[i].endTime);
			GlobalItemVector[Items[i].index]->consensus->addFAAction(1, Items[i].startTime, Items[i].endTime);
			break;
		case 2:
			GlobalItemVector[Items[i].index]->receiver->addFAAction(2, Items[i].startTime, Items[i].endTime);
			GlobalItemVector[Items[i].index]->consensus->addFAAction(2, Items[i].startTime, Items[i].endTime);
			break;
		}
		
	}
}
