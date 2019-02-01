#include "LogicInc.h"

int main(int argc, char *argv[]) {
	if (!Kernel::Init(APP_CONFIG_PATH)) {
		cout << "Init kernel fail!" << endl;
		return -1;
	}
	Config::SetDomain(0);

	if (argc != 3) {
		cout << "usage: " << argv[0] << "serverfrom serverto " << endl;
		return 1;
	}
	int serverfrom = atoi(argv[1]);
	int serverto = atoi(argv[2]);
	if (serverfrom<0 || serverto<0 || serverfrom>serverto) {
		cout << "server wrong!" << endl;
		return 1;
	}

	int ret;
	for(int i=serverfrom-1;i<serverto;++i)
	{
		uint64_t uidEnd;
		unsigned userid = 10000000 + 500000*i;
		CLogicIdCtrl logicIdCtrl;
		ret = logicIdCtrl.GetCurrentId(KEY_UID_CTRL, uidEnd,i+1);
		if(ret != 0)
		{
			error_log("get current user id fail! from ClearUserWorldmainpos ---start!");
			return ret;
		}
		for(;userid <= (unsigned)uidEnd;++userid)
		{
			Json::Reader reader;
			Json::FastWriter writer;
			Json::Value stat;
			Json::Value gift;
			CLogicUser logicUser;
			DataUser user;
			ret = logicUser.GetUser(userid, user);
			if (!user.user_stat.empty() && !reader.parse(user.user_stat, stat))
			{
				cout<<userid<<" stat error!"<<endl;
				cout<<user.user_stat<<endl;
				user.user_stat = writer.write(stat);
				cout<<user.user_stat<<endl;
				logicUser.SetUser(userid,user);
			}

			DataUserData userData;
			CLogicUserData logicUserData;
			ret = logicUserData.GetUserData(userid, userData);
			if (!userData.gift.empty() && !reader.parse(userData.gift, gift))
			{
				cout<<userid<<" gift error!"<<endl;
				cout<<userData.gift<<endl;
				userData.gift = writer.write(gift);
				cout<<userData.gift<<endl;
				logicUserData.SetUserData(userid, userData);
			}
		}
	}
	cout<<"ok"<<endl;

	return 0;
}
