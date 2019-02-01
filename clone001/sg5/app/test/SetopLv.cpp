#include "LogicInc.h"
#include <iostream>

int main(int argc, char *argv[])
{
	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);

	if (argc != 3)
	{
		cout << "usage: " << argv[0] << " [uid] [opLv]" << endl;
		return 1;
	}
	unsigned uid = CTrans::STOI(argv[1]);
	int opLv = CTrans::STOI(argv[2]);
	cout << "start,uid:" << uid << endl;
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUser(uid, user);
	if (ret != 0)
	{
		cout << "get user failed.ret:" << ret << endl;
		return 2;
	}
	Json::Reader reader;
	Json::FastWriter writer;
	Json::Value stats;
	if (!reader.parse(user.user_stat, stats))
	{
		cout << "parse failed.ret:" << ret << endl;
		return 2;
	}
	stats["opLv"] = opLv;
	user.user_stat = writer.write(stats);
	ret = logicUser.SetUser(uid, user);
	if (ret != 0)
	{
		cout << "set user failed.ret:" << ret << endl;
		return 4;
	}
	cout << "ok.uid:" << uid << endl;
	return 0;
}
