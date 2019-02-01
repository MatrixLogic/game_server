#include "LogicInc.h"
#include <iostream>
#include<math.h>

void CalcCoins(int rank, int &coins, int &bs)
{
	if (rank == 1)
	{
		coins = 100;
		bs = 10000;
	}
	else if (rank == 2)
	{
		coins = 80;
		bs = 7000;
	}
	else if (rank == 3)
	{
		coins = 60;
		bs = 6500;
	}
	else if (rank >= 4 && rank <= 1000)
	{
		coins = 50 - ((rank - 4) / 20);
		double temp = pow((float)rank, 2);
		bs = (int)(0.0042 * temp - 10.13 * rank + 6030);
	}
	else
	{
		coins = 0;
		bs = 0;
	}
}

int main(int argc, char *argv[])
{
	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "init kernel fail" << endl;
		return 1;
	}

	Config::SetDomain(0);
	int semgroup = 0;
	int semserver = 0;
	Config::GetDB(semserver);
	CShareMemory shm;
	if (!shm.CreateOrOpen(Config::GetValue(CONFIG_LADDER_PATH).c_str(), sizeof(LadderData), SEM_ID(sem_ladder,semgroup,semserver)))
	{
		cout << "init shm fail" << endl;
		return 1;
	}
	CAutoLock lock(&shm, true);
	LadderData *pdata = (LadderData *)shm.GetAddress();
	if (!pdata)
	{
		cout << "null addr" << endl;
		return 1;
	}
	int ret;
	CDataUser logicUser;
	CLogicUpdates logicUpdates;
	for (int i = 0; i < LADDER_ITEM_NUMBER; i++)
	{
		unsigned uid = pdata->ladder[i].uid;
		if (!IsValidUid(uid) || pdata->ladder[i].level < 10)
			continue;
		int coins = 0;
		int bs = 0;
		CalcCoins(i + 1, coins, bs);
		cout << "uid=" << uid << ",rank=" << i+1 << ",coins=" << coins << ",bs=" << bs << endl;
		Json::Value updates;
		updates.resize(1);
		updates[(unsigned)0]["s"] = "LADDER";
		updates[(unsigned)0]["rank"] = i + 1;
		updates[(unsigned)0]["coins"] = coins;
		updates[(unsigned)0]["bs"] = bs;
		updates[(unsigned)0]["ts"] = (unsigned)time(0);
		ret = logicUpdates.AddUpdates(uid, updates);
		if (0 != ret)
		{
			cout << "AddUpdates error uid=" << uid << endl;
		}
	}
	cout << "ok" << endl;
	return 0;
}
