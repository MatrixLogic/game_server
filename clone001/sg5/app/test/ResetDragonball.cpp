#include "LogicInc.h"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		cout << "usage: " << argv[0] << " ballid file" << endl;
		return 1;
	}

	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "init kernel fail" << endl;
		return 1;
	}
	Config::SetDomain(0);
	int ret = 0;
	int ballid = atoi(argv[1]);
	if (!IsValidDragonballId(ballid))
	{
		cout << "invalid ballid" << endl;
		return 1;
	}

	CDataDragonball dragonball;
	CLogicUpdates logicUpdates;
	ret = dragonball.Init(Config::GetValue(CONFIG_DRAGONBALL_DATA));
	if (ret != 0)
	{
		cout << "init dragonball fail" << endl;
		return 1;
	}
	unsigned now = time(NULL);
	string action = argv[2];
	if (action == "reward")
	{
		int ret = 0;
		Dragonball ball;
		ret = dragonball.GetDragonballStatus(ballid, ball);
		if (0 != ret)
		{
			cout << "ViewAllDragonball fail,ret=" << ret << endl;
			return ret;
		}
		if (IsValidUid(ball.holderUid))
		{
			Json::Value updates;
			updates.resize(1);
			updates[(unsigned)0]["s"] = "DRAGONBALLREWARD";
			updates[(unsigned)0]["ballid"] = ballid;
			updates[(unsigned)0]["tm"] = now - ball.ts;
			updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
			logicUpdates.AddUpdates(ball.holderUid, updates);
		}
	}
	else if (action == "finish")
	{
		string DragonDataPath = Config::GetValue(CONFIG_DRAGONBALL_DATA);

		int semgroup = 0;
		int semserver = 0;
		Config::GetDB(semserver);
		CShareMemory shm;
		if (!shm.CreateOrOpen(DragonDataPath.c_str(), sizeof(DragonballData), SEM_ID(sem_dragonball,semgroup,semserver)))
		{
			cout << "init fail" << endl;
			return 1;
		}
		CAutoLock lock(&shm, true);
		DragonballData *pdata = (DragonballData *)shm.GetAddress();
		if (!pdata)
		{
			cout << "null addr" << endl;
			return 1;
		}
		memset(pdata, 0, sizeof(*pdata));
	}
	else if (action == "start")
	{
		ifstream fin(argv[3]);
		if (!fin.good())
		{
			cout << "open file fail" << endl;
			return 1;
		}
		unsigned num = 0;
		unsigned uids[100000];
		unsigned temp = 0;
		while (!fin.eof() && fin.good())
		{
			if (num >= 100000)
				break;
			temp = 0;
			fin >> temp;
			if (!IsValidUid(temp))
				continue;
			uids[num++] = temp;
		}

		if (num == 0)
		{
			cout << "uid is empty" << endl;
			return 1;
		}
		unsigned index = 0;
		if (num > 1)
		{
			unsigned r = rand();
			index = r % num;
		}
		if (!IsValidUid(uids[index]))
		{
			cout << "error" << endl;
			return 1;
		}
		unsigned uid = uids[index];

		DataUserBasic userBasic;
		CDataUserBasic userBasicDB;
		ret = userBasicDB.GetUserBasicRegisterLimit(uid, userBasic);
		if (ret != 0)
		{
			cout << "get user basic fail" << endl;
			return 1;
		}
		now = time(NULL);
		unsigned preUid = 0;
		unsigned preTs = 0;
		string preName;
		ret = dragonball.SetDragonball(ballid, uid, userBasic.name, now, preUid, preTs, preName);
		if (ret != 0)
		{
			cout << "set dragonball fail" << endl;
			return 1;
		}
		Json::Value updates2;
		updates2.resize(1);
		updates2[(unsigned)0]["s"] = "DRAGONBALLINIT";
		updates2[(unsigned)0]["ballid"] = ballid;
		updates2[(unsigned)0]["ballts"] = Time::GetGlobalTime();
		updates2[(unsigned)0]["ts"] = Time::GetGlobalTime();
		logicUpdates.AddUpdates(uid, updates2,true);

		if(ballid == 7){
			CLogicDragonball logicBall;
			AllianceBall aBall;
			dragonball.SetAlliance2(aBall);
			logicBall.PayAlliance(aBall);
			cout << "set alliance ok" << endl;
		}
	}
	cout << "ok," << ballid << endl;
	return 0;
}
