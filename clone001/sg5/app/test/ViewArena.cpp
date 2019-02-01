#include "LogicInc.h"
#include <iostream>

int main(int argc, char *argv[])
{
	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);

	if (argc != 2)
	{
		cout << "usage: ViewArena id [detail]" << endl;
		return 1;
	}
	unsigned arenaid = CTrans::STOI(argv[1]);
	DataArena data;
	CLogicArena logicArena;
	int ret = logicArena.GetArena(arenaid, data);
	if (ret != 0)
	{
		cout << "get error,ret:" << ret << endl;
		return 1;
	}
	cout << "host: " << data.host << endl;
	cout << "level: " << data.level << endl;
	cout << "winCount: " << data.winCount << endl;
	cout << "challenger: " << data.challenger << endl;
	cout << "challengeTs: " << data.challengeTs << endl;
	cout << "breathTs: " << data.breathTs << endl;
	cout << "preHost: " << data.preHost << endl;
	cout << "preChallenger: " << data.preChallenger << endl;
	cout << "prizePool: " << data.prizePool << endl;
	cout << "name: " << data.name << endl;
	cout << "pic: " << data.pic << endl;
	if (argc > 2 && string(argv[2]) == "detail")
	{
		cout << "archive: " << data.archive << endl;
	}

	return 0;
}
