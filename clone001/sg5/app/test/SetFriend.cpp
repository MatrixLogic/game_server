#include "LogicInc.h"
#include <fstream>

int main(int argc, char *argv[])
{
	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);

	if (argc != 3)
	{
		cout << "usage: " << argv[0] << " [uid] [fuid]" << endl;
		return 1;
	}
	unsigned uid = CTrans::STOI(argv[1]);
	unsigned fuid = CTrans::STOI(argv[2]);
	if (uid == fuid)
	{
		cout << "uid:" << uid << ",fuid:" << fuid << endl;
		return 5;
	}
	CDataUserBasic userBasicDB;
	DataUserBasic userBasic;
	int ret = userBasicDB.GetUserBasicExt(uid, 0, userBasic);
	if (ret != 0)
	{
		cout << "get user basic failed.uid:" << uid << ",ret:" << ret << endl;
		return 2;
	}
	if (userBasic.fnum >= 300)
	{
		cout << "fnum:" << userBasic.fnum << endl;
		return 0;
	}
	CDataUser userDB;
	DataUser user;
	ret = userDB.GetUserLimit(fuid, user);
	if (ret != 0)
	{
		cout << "get user failed.fuid:" << fuid << ",ret:" << ret << endl;
		return 3;
	}

	bool change = false;
	string sfind("|");
	sfind.append(argv[2]).append("|");
	if (userBasic.friends.empty())
	{
		userBasic.oid_friends = "|nicho001";
		userBasic.friends = sfind;
		userBasic.fnum = 1;
		change = true;
	}
	else
	{
		if (userBasic.friends.find(sfind) == string::npos)
		{
			userBasic.oid_friends.append("nicho001").append("|");
			userBasic.friends.append(argv[2]).append("|");
			userBasic.fnum++;
			change = true;
		}
	}
	if (change)
	{
		ret = userBasicDB.SetUserBasicExt(uid, 0, userBasic);
		if (ret != 0)
		{
			cout << "set user basic failed.uid:" << uid << ",ret:" << ret << endl;
			return 4;
		}
	}
	cout << "ok.fnum:" << userBasic.fnum << endl;

	return 0;
}
