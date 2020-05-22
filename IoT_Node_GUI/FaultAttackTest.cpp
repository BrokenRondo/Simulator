#include "FaultAttackTest.h"

extern std::vector<MyItem*> GlobalItemVector;
void FaultAttackTest::addFAItemVector(std::vector<FAItem> Items)
{
	for (int i=0;i<Items.size();i++)
	{
		//��vector��һ������action���úã���Ҫ����recv��Consensus�����࣬send�ݲ����ã���Ϊ��ʵ��û��Ӱ��
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
