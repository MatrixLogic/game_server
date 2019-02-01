#include "LogicInc.h"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[])
{
	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "Init kernel fail!" << endl;
		return -1;
	}
	Config::SetDomain(0);
	if (argc != 2)
	{
		if (argc == 3){
			CLogicUser logicUser;
			ifstream fin(argv[2]);
			if (!fin.good()) {
				cout << "open file fail" << endl;
				return -1;
			}
			while (!fin.eof() && fin.good()) {
				unsigned uid = 0;
				fin >> uid;
				logicUser.SetMainpos(uid,0);
			}
			cout<<"all ok!"<<endl;
			return 0;
		}
		cout << "require ./ClearUserWorldmainpos yes! to confirm you really wanna clear db world info!" << endl;
		return 1;
	}
	int ret;
	uint64_t uidEnd;
	unsigned userid = Config::GetIntValue(CONFIG_UID_MIN);
	CLogicIdCtrl logicIdCtrl;
	ret = logicIdCtrl.GetCurrentId(KEY_UID_CTRL, uidEnd);
	if(ret != 0)
	{
		error_log("get current user id fail! from ClearUserWorldmainpos ---start!");
		return ret;
	}
	int i = time(NULL);
	cout << "i=" << i << endl;
	CLogicUser logicUser;
	bool flag  = true;
	for( userid = Config::GetIntValue(CONFIG_UID_MIN); userid <= (unsigned)uidEnd; ++userid)
	{
		ret = logicUser.SetMainpos(userid,0);
		if(0 != ret){
			flag = false;
			cout<<"set user limit failed:"<<userid<<endl;
		}

	}
	if(flag)
		cout<<"all ok!"<<endl;
	int j = time(NULL);
	cout << "j=" << j << endl;
	cout << "take out:" << j-i << "s" << endl;
	return 0;
}



