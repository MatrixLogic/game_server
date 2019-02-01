#include "LogicInc.h"
#include <iostream>

int main(int argc, char *argv[])
{
	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);

	if (argc != 2)
	{
		cout << "usage: " << argv[0] << " [uid]" << endl;
		return 1;
	}
	unsigned uid = CTrans::STOI(argv[1]);
	DataUser user;
	CLogicUser logicUser;
	int ret = logicUser.GetUser(uid, user);
	if (ret != 0)
	{
		cout << "get user fail,ret:" << ret << endl;
		return 0;
	}
	user.soldier = "[]";
	ret = logicUser.SetUser(uid, user);
	if (ret != 0)
	{
		cout << "set user fail,ret:" << ret << endl;
		return 0;
	}
	cout << "ok" << endl;
	return 0;
}
