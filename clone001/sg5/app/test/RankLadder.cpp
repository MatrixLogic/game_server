#include "LogicInc.h"
#include <iostream>

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
	Json::Value ladderRank = Json::Value(Json::arrayValue);
	for (int i = 0; i < LADDER_ITEM_NUMBER; i++)
	{
		if (ladderRank.size() > 300)
			break;
		if (!IsValidUid(pdata->ladder[i].uid))
			continue;
		Json::Value &ladder = ladderRank[ladderRank.size()];
		ladder["uid"] = pdata->ladder[i].uid;

		CLogicUser logicUser;
		DataUser user;
		int power = 0;
		ret = logicUser.GetUser(pdata->ladder[i].uid, user);
		if (0 == ret)
		{
			Json::Value user_stat;
			if (Json::FromString(user_stat, user.user_stat))
			{
				if (user_stat.isMember("power"))
				{
					Json::GetInt(user_stat, "power", power);
				}
			}
		}
		ladder["level"] = pdata->ladder[i].level = user.level;
		ladder["rank"] = power;
		ladder["kingdom"] = user.kingdom;

		CLogicUserBasic logicUserBasic;
		DataUserBasic userBasic;
		ret = logicUserBasic.GetUserBasic(pdata->ladder[i].uid, PT_PENGYOU , userBasic);
		ladder["vt"] = userBasic.vip_type;
		ladder["vl"] = userBasic.vip_level;
		strcpy(pdata->ladder[i].name,userBasic.name.c_str());
		ladder["name"] = userBasic.name;
	}

	string ranklist_path("../webroot/data/ranklist");
	string ranklist_buff;
	ret = File::Read(ranklist_path,ranklist_buff);
	if (0 != ret)
	{
		cout << "Read file faile path=" << ranklist_path << endl;
		return 1;
	}
	Json::Reader reader;
	Json::Value ranklist;
	if (!reader.parse(ranklist_buff, ranklist))
	{
		cout << "Parse Json error!" << reader.getFormatedErrorMessages() << endl;
		return 1;
	}
	ranklist["ladder"] = ladderRank;
	ret = File::Write(ranklist_path, Json::ToString(ranklist));
	if (0 != ret)
	{
		cout << "Write file faile path=" << ranklist_path << endl;
		return 1;
	}

	cout << "ok" << endl;
	return 0;
}
