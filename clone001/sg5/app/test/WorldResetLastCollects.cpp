/*
 * WorldResetLastCollects.cpp
 *
 *  Created on: 2013-1-31
 *      Author: Administrator
 */


#include "LogicInc.h"
#include <fstream>

int main(int argc ,char *argv[])
{
	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "init kernel fail" << endl;
		return 1;
	}

	Config::SetDomain(0);

	CDataWorldNpc *pdata;
	CLogicWorldNpc logicWorldNpc;
	CLogicBaseExtra logicBaseExtra;
	DataBaseExtra baseExtra;
	NpcItem NpcItemList[1000];
	int iCount;

	pdata = logicWorldNpc.GetDataWorldNpc();
	if (pdata==NULL)
	{
		cout<<"failed"<<endl;
		return -1;
	}
	int ret = pdata->GetAllNpc(NpcItemList,iCount);
	if(0 != ret)
	{
		cout<<"GetAllNpc failed "<<endl;
		return 0;
	}
	ret = pdata->MakeReward();
	if(0 != ret)
		cout<<"MakeReward failed "<<endl;
	for(int i = 0; i < iCount; i++)
	{
		if(IsValidUid(NpcItemList[i].uid))
		{
			ret = logicBaseExtra.GetBaseExtra(NpcItemList[i].uid,NpcItemList[i].worldpos,baseExtra);
			if(0 != ret)
			{
				cout<<"get sub base failed"<<endl;
			}
			baseExtra.last_collect_ts = 0;
			ret = logicBaseExtra.SetBaseExtra(NpcItemList[i].uid,NpcItemList[i].worldpos,baseExtra);
			if(0 != ret)
			{
				cout<<"Set sub base failed"<<endl;
			}
		}
	}

	cout<<"ok"<<endl;
	return 0;
}

