#include "LogicInc.h"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[])
{
	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "init kernel fail" << endl;
		return 1;
	}
	Config::SetDomain(0);

	if (argc != 3)
	{
		cout << "usage: " << argv[0] << " arenaid file" << endl;
		return 1;
	}

	unsigned arenaid = atoll(argv[1]);
	if (!IsValidFightArenaId(arenaid))
	{
		cout << "invalid arenaid" << endl;
		return 1;
	}
	ifstream fin(argv[2]);
	if (!fin.good())
	{
		cout << "open file fail,arenaid=" << arenaid << endl;
		return 1;
	}
	Json::Value json;
	Json::Reader reader;
	if (!reader.parse(fin, json))
	{
		cout << "parse file fail,arenaid=" << arenaid << endl;
		return 1;
	}
	json["userid"] = arenaid;

	DataFightArena data;
	CLogicFightArena logicArena;
	int ret = logicArena.GetFightArena(arenaid, data);
	if (ret != 0)
	{
		cout << "get arena fail,arenaid=" << arenaid << endl;
		return 1;
	}
	unsigned host = data.host;
	unsigned prize = data.prizePool;
	data.host = 0;
	data.level = 1;
	data.winCount = 0;
	data.challenger = 0;
	data.challengeTs = 0;
	data.breathTs = 0;
	data.preHost = 0;
	data.preChallenger = 0;
	data.prizePool = 0;
	string name = json["name"].asString();
	snprintf(data.name, sizeof(data.name), "%s", name.c_str());
	string pic = json["pic"].asString();
	snprintf(data.pic, sizeof(data.pic), "%s", pic.c_str());
	Json::FastWriter writer;
	snprintf(data.archive, sizeof(data.archive), "%s", writer.write(json).c_str());

	CDataFightArena *arenaDb = CLogicFightArena::GetCDataFightArena();
	if (!arenaDb)
	{
		cout << "init arena fail,arenaid=" << arenaid << endl;
		return 1;
	}
	ret = arenaDb->SetFightArena(arenaid, data);
	if (ret != 0)
	{
		cout << "set arena fail,arenaid=" << arenaid << endl;
		return 1;
	}
	CLogicPay logicPay;
	if (IsValidUid(host) && prize > 0 && logicPay.ChangePay(host, 0, prize, "FIGHTARENARESET") != 0)
	{
		cout << "prize fail,arenaid=" << arenaid << ",host=" << host << ",prize=" << prize << endl;
	}

	cout << "ok,arenaid=" << arenaid << endl;
	return 0;
}
