#include "LogicInc.h"
#include <iostream>

int main(int argc, char *argv[])
{
	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);

	if (argc != 3)
	{
		cout << "usage: " << argv[0] << " [begin uid] [end uid]" << endl;
		return 1;
	}
	unsigned beginUid =  CTrans::STOI(argv[1]);
	unsigned endUid = CTrans::STOI(argv[2]);
	CDataUser userDb;
	DataUser user;
	Json::Reader reader;
	Json::FastWriter writer;
	int ret;
	int count = 0;
	bool modify = false;
	for (; beginUid <= endUid; beginUid++)
	{
		ret = userDb.GetUser(beginUid, user);
		if (ret != 0)
			continue;
		Json::Value stat;
		if (!reader.parse(user.user_stat, stat))
			continue;
		modify = false;
		if (stat.isMember("ds") && stat["ds"].isArray() && stat["ds"].size() == 8)
		{
			if (stat["ds"][(unsigned)6].asInt() == 1)
			{
				stat["ds"][(unsigned)6] = 0;
				modify = true;
			}
			if (stat["ds"][(unsigned)7].asInt() == 1)
			{
				stat["ds"][(unsigned)7] = 0;
				modify = true;
			}
		}
		if (stat.isMember("dr") && stat["dr"].isArray() && stat["dr"].size() == 2
				&& stat["dr"][(unsigned)1].asInt() == 1)
		{
			stat["dr"][(unsigned)0] = 0;
			stat["dr"][(unsigned)1] = 0;
			modify = true;
		}
		if (modify)
		{
			user.user_stat = writer.write(stat);
			ret = userDb.SetUser(beginUid, user);
			if (ret == 0)
			{
				count++;
				cout << user.uid << endl;
				if (count % 50 == 0)
				{
					cout << count << " done" << endl;
				}
			}
		}
	}
	cout << "finish,count:" << count << endl;
	return 0;
}
