#include "LogicInc.h"
#include <iostream>

int main(int argc, char *argv[])
{
	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);

	if (argc != 4)
	{
		cout << "usage: " << argv[0] << " uid eqid amount" << endl;
		return 1;
	}
	unsigned uid = CTrans::STOI(argv[1]);
	unsigned eqid = CTrans::STOI(argv[2]);
	unsigned amount = CTrans::STOI(argv[3]);
	CLogicSecinc logicSecinc;
	int ret = logicSecinc.AddSecinc(uid, eqid, amount);
	if (ret != 0)
	{
		cout << "fail.ret:" << ret << endl;
		return 2;
	}
	cout << "ok" << endl;
	return 0;
}
