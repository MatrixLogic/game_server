#include "LogicInc.h"
#include <iostream>

int main(int argc, char *argv[])
{
	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);
	if (argc != 3)
	{
		cout << "usage: " << argv[0] << " [uid] [data]" << endl;
		return 1;
	}
	unsigned uid = CTrans::STOI(argv[1]);
	if (!IsValidUid(uid))
	{
		cout << "invalid uid.uid:" << uid << endl;
		return 2;
	}
	Json::Reader reader;
	Json::Value json;
	if (!reader.parse(argv[2], json))
	{
		cout << "parse fail.data:" << argv[2] << endl;
		return 3;
	}
	CDataBuilding db;
	unsigned id = 0;
	if (!Json::GetUInt(json, "id", id))
	{
		cout << "ud error" << endl;
		return 4;
	}
	if (json.isMember("status")
			&& json["status"].isIntegral()
			&& json["status"].asInt() == 9)
	{
		cout << "no need update" << endl;
		return 0;
	}
	int ret = db.ReplaceBuilding(uid, id, argv[2]);
	if (ret != 0)
	{
		cout << "replace fail.ret:" << ret << endl;
		return 5;
	}
	cout << "ok" << endl;
	return 0;
}
