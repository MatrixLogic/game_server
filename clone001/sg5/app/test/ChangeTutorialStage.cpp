#include "LogicInc.h"
#include <iostream>

void Usage()
{
	cout << "usage:" << endl
			<< "-p    platform" << endl
			<< "-o    openid, must be used with -p" << endl
			<< "-u    uid, ignore -o" << endl
			<< "-t    tutorial stage, default 10000" << endl
			<< "-h    show this help" << endl;
}

int main(int argc, char *argv[])
{
	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);

	int ret = 0;
	PlatformType platform = PT_UNKNOW;
	string openid;
	unsigned uid = 0;
	int tutorial_stage = 10000;

	int o = 0;
	while(-1 != (o = getopt(argc, argv, "p:o:u:h:t:")))
	{
		switch(o)
		{
		case 'h': Usage(); return 0;
		case 'u':
			uid = CTrans::STOI(optarg);
			if(!IsValidUid(uid))
			{
				cout << "invalid uid" << endl;
				return -1;
			}
			break;
		case 'p':
			ret = CTrans::STOI(optarg);
			if (ret <= PT_UNKNOW || ret >= PT_MAX)
			{
				cout << "unknown platform" << endl;
				return -1;
			}
			platform = static_cast<PlatformType>(ret);
			break;
		case 'o':
			openid = optarg;
			break;
		case 't':
			tutorial_stage = CTrans::STOI(optarg);
			break;
		default:
			Usage();
			return -1;
		}
	}
	if(uid == 0)
	{
		if(openid.empty() || platform == PT_UNKNOW)
		{
			Usage();
			return -1;
		}
		CLogicUserBasic logicUserBasic;
		ret = logicUserBasic.GetUid(uid, platform, openid);
		if (ret != 0)
		{
			cout << "query openid failed.ret:" << ret << endl;
			return -1;
		}
	}

	CLogicUser logicUser;
	DataUser user;
	ret = logicUser.GetUserLimit(uid, user);
	if(ret != 0)
	{
		cout << "query user failed.ret:" << ret << endl;
		return -1;
	}
	user.tutorial_stage = tutorial_stage;
	ret = logicUser.SetUserLimit(uid, user);
	if(ret != 0)
	{
		cout << "set user failed.ret:" << ret << endl;
		return -1;
	}
	cout << "ok" << endl;

	return 0;
}
