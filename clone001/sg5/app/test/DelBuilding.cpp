#include "LogicInc.h"
#include <iostream>

int main(int argc, char *argv[])
{
	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);

	if (argc != 3)
	{
		cout << "usage: " << argv[0] << " [uid] [id]" << endl;
		return 1;
	}
	unsigned uid = CTrans::STOI(argv[1]);
	unsigned id = CTrans::STOI(argv[2]);
	CDataBuilding bdDB;
	int ret = bdDB.RemoveBuilding(uid, id);
	if (ret != 0)
	{
		cout << "fail.ret:" << ret << endl;
		return 2;
	}
	cout << "ok" << endl;
	return 0;
}
