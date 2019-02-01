#include "LogicInc.h"
#include <iostream>

int main(int argc, char *argv[])
{
	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);

	if (argc != 3)
	{
		cout << "usage: " << argv[0] << " [uid] [ext]" << endl;
		return 1;
	}
	unsigned uid = CTrans::STOI(argv[1]);
	int ext = CTrans::STOI(argv[2]);
	DataUser user;
	CDataUser userDb;
	int ret = userDb.GetUserLimit(uid, user);
	if (ret != 0)
	{
		cout << "GetUserLimit fail" << endl;
		return 1;
	}
	user.ext = ext;
	ret = userDb.SetActiveTimeAndExt(uid, user.last_active_time, user.ext);
	if (ret != 0)
	{
		cout << "SetActiveTimeAndExt fail" << endl;
		return 1;
	}
	cout << "ok" << endl;

	return 0;
}
