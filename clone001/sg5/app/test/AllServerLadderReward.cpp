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

void CheckUserLevel(unsigned uid, int level, bool &needChang)
{
	if (level < 1 || level > 3)
	{
		return;
	}
	const int levelMap[3][2] =
	{
		{1,70},
		{71,90},
		{91,1000}
	};
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUserLimit(uid,user);
	if (0 != ret)
	{
		return;
	}
	if (user.level < levelMap[level - 1][0] || user.level > levelMap[level - 1][1])
	{
		needChang = true;
	}
	return;
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

	string dataPath = Config::GetValue(CONFIG_ALL_SERVER_LADDER_PATH);
	const semdat sem[3] = {sem_ladderallserver_1, sem_ladderallserver_2, sem_ladderallserver_3};
	const double multiple[3] = {1.0,1.5,2.0};

	Json::Value ladderRank[3] = Json::Value(Json::arrayValue);

	for (int level=1; level<=3; level++)
	{
		string filePath;
		String::Format(filePath, "%s.%d", dataPath.c_str(), level);
		CShareMemory shm;
		if (!shm.CreateOrOpen(filePath.c_str(), sizeof(LadderData), SEM_ID(sem[level-1],semgroup,semserver)))
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

			if (ladderRank[level-1].size() <= 300) {
				Json::Value &ladder = ladderRank[level-1][ladderRank[level-1].size()];
				ladder["uid"] = pdata->ladder[i].uid;
				ladder["level"] = pdata->ladder[i].level;
				ladder["name"] = pdata->ladder[i].name;
				CLogicUser logicUser;
				DataUser user;
				int power = 0;
				ret = logicUser.GetUser(pdata->ladder[i].uid, user);
				if (0 == ret) {
					Json::Value user_stat;
					if (Json::FromString(user_stat, user.user_stat)) {
						if (user_stat.isMember("power")) {
							Json::GetInt(user_stat, "power", power);
						}
					}
				}
				ladder["rank"] = power;
				ladder["kingdom"] = user.kingdom;
			}

			int coins = 0;
			int bs = 0;
			CalcCoins(i + 1, coins, bs);
			cout << "bs=" << bs << endl;
			bs = (int)(bs * multiple[level - 1]);
			cout << "uid=" << uid << ",rank=" << i+1 << ",coins=" << coins << ",bs=" << bs << endl;
			Json::Value updates;
			updates.resize(1);
			updates[(unsigned)0]["s"] = "CROSSEDLADDER";
			updates[(unsigned)0]["rank"] = i + 1;
			updates[(unsigned)0]["coins"] = coins;
			updates[(unsigned)0]["bs"] = bs;
			updates[(unsigned)0]["ts"] = (unsigned)time(0);
			ret = logicUpdates.AddUpdates(uid, updates);
			if (0 != ret)
			{
				cout << "AddUpdates error uid=" << uid << endl;
			}
			if(IsValidUid(uid))
			{
				bool needChange = false;
				CheckUserLevel(uid,level,needChange);
				if (needChange)
				{
					pdata->ladder[i].uid = 401;
					pdata->ladder[i].level = 13;
					pdata->ladder[i].upts = Time::GetGlobalTime();
					snprintf(pdata->ladder[i].name, sizeof(pdata->ladder[i].name), "挑战我吧");
					snprintf(pdata->ladder[i].pic, sizeof(pdata->ladder[i].pic), "http://s4.app100630637.qqopenapp.com/images/arena.png");
					//error_log("ladder_change : %u |",uid);
				}
			}
		}
	}

	string ranklist_path("../webroot/data/allserverranklist");
	string ranklist_buff;

	Json::Value ranklist;
	ranklist["ladder1"] = ladderRank[0];
	ranklist["ladder2"] = ladderRank[1];
	ranklist["ladder3"] = ladderRank[2];

	int ret = File::Write(ranklist_path, Json::ToString(ranklist));
	if (0 != ret) {
		cout << "Write file faile path=" << ranklist_path << endl;
		return 1;
	}

	cout << "ok" << endl;
	return 0;
}
