#include "LogicInc.h"
#include <iostream>

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		cout << "SetNewGcBase uid value" << endl;
		return 1;
	}

	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);

	unsigned uid = CTrans::STOI(argv[1]);
	unsigned value = CTrans::STOI(argv[2]);

	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUserLimit(uid, user);
	if(ret != 0)
	{
		cout << "query user failed.ret:" << ret << endl;
		return -1;
	}
	user.newgcbase = value;
	ret = logicUser.SetUserLimit(uid, user);
	if(ret != 0)
	{
		cout << "set user failed.ret:" << ret << endl;
		return -1;
	}
	cout << "ok" << endl;

	return 0;
}
