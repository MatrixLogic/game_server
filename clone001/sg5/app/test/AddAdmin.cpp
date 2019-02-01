#include "LogicInc.h"
#include <iostream>

int main(int argc, char *argv[])
{
	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);

	if (argc != 3)
	{
		cout << "usage: " << argv[0] << " [openid] [platform]" << endl;
		return 1;
	}
	unsigned uid = 10000000;
	int pt = atoi(argv[2]);
	if (pt >= PT_MAX || pt <= PT_UNKNOW)
	{
		cout << "platform error" << endl;
		return 0;
	}
	PlatformType platform = (PlatformType)pt;
	string openid = argv[1];

	OPUserInfo userInfo;
	userInfo.OpenId = openid;
	userInfo.Name = "xiangxiang";
	userInfo.Gender = 0;
	userInfo.ExtraIntInfo["bIsYearVip"] = false;
	userInfo.ExtraIntInfo["bIsVip"] = false;
	userInfo.ExtraIntInfo["nVipLevel"] = 0;
	OPFriendList friendList;

	CLogicUserBasic logicUserBasic;
	int ret = logicUserBasic.GetUid(uid, platform, userInfo.OpenId);
	if (ret != R_ERR_NO_DATA)
	{
		if (ret == 0)
		{
			cout << "This mapping exist." << endl;
			return 0;
		}
		cout << "get uid fail.ret:" << ret << endl;
		return 0;
	}
	CLogicUser logicUser;
	ret = logicUser.AddUser(uid, platform, 0);
	if (ret != 0)
	{
		cout << "add user fail.ret:" << ret << ",errMsg:" << ::GetErrorMessage() << endl;
		return 0;
	}
	ret = logicUserBasic.SetUserBasic(uid, true, platform, userInfo, friendList);
	if (ret != 0)
	{
		cout << "SetUserBasic fail.ret:" << ret << ",errMsg:" << ::GetErrorMessage() << endl;
		int result = logicUser.RemoveUser(uid);
		if (result != 0)
		{
			cout << "RemoveUser fail.ret:" << ret << ",errMsg:" << ::GetErrorMessage() << endl;
		}
		return 0;
	}

	cout << "ok" << endl;
	return 0;
}
