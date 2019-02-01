#include "LogicInc.h"
#include <iostream>

int main(int argc, char *argv[])
{
	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);

	if (argc != 3)
	{
		cout << "usage: " << argv[0] << " [uid] [value]" << endl;
		return 1;
	}
	unsigned uid = CTrans::STOI(argv[1]);
	int value = CTrans::STOI(argv[2]);
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
	{
		cout << "get user failed.ret:" << ret << endl;
		return 2;
	}
	int newv = user.gcbase + value;
	if (newv < 0)
	{
		cout << "value error.gcbase:" << user.gcbase << endl;
		return 3;
	}
	user .gcbase = newv;
	ret = logicUser.SetUserLimit(uid ,user);
	if (ret != 0)
	{
		cout << "set user failed.ret:" << ret << endl;
		return 4;
	}
	cout << "ok.gcbase:" << user.gcbase << endl;
	return 0;
}
