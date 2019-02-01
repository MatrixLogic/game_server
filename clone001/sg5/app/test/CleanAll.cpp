#include "LogicInc.h"
#include <iostream>

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cout << "usage: " << argv[0] << " [uid]" << endl;
		return 1;
	}

	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);

	unsigned uid = CTrans::STOI(argv[1]);
	if (!IsValidUid(uid))
	{
		cout << "invalid uid" << endl;
		return 2;
	}
	CLogicUserBasic logicUserBasic;
	int ret = logicUserBasic.RemoveUserMapping(uid);
	if (ret != 0)
	{
		cout << "remove mapping failed" << endl;
		return 3;
	}
	CLogicUser logicUser;
	ret = logicUser.RemoveUser(uid);
	if (ret != 0)
	{
		cout << "remove user failed" << endl;
		return 4;
	}
	cout << "ok" << endl;
	return 0;
}
