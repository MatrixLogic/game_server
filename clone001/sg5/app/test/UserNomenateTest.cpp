#include "LogicInc.h"
#include <iostream>

int main(int argc, char *argv[])
{
	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "init kernel fail" << endl;
		return 1;
	}
	Config::SetDomain(0);

	int ret;
	unsigned uid[12] = {10000123, 10000096, 10000029, 10000243, 10000290, 10000313, 10000314, 10000315, 10000316, 10000317, 10001984, 10000003};
	string rec_list("|10000123|10000096|10000029|10000243|10000290|10000313|10000314|10000315|10000316|10000317|10001984|10000003|");
	string add_list(""), friend_list(""), black_list(""), rep("|"), retlist(""), rec_list1("");
	rec_list1 = rec_list;

	CLogicUserNomenate logicUserNomenate;
	DataUserNomenate userNomenateDB;

	/*for(int i=0;i<12;i++){

		ret = logicUserNomenate.DeleteUserNomenate(uid[i]);
		if(ret != 0)
		{
			cout << "del userNomenate err!errno=" << ret << endl;
			return ret;
		}
		userNomenateDB.uid = uid[i];
		userNomenateDB.update_time = Time::GetGlobalTime();
		ret = logicUserNomenate.AddUserNomenate(userNomenateDB);
		if(ret != 0)
		{
			cout << "add userNomenate err!errno=" << ret << endl;
			return ret;
		}
	}

		string temp("|");
		temp.append(Convert::UIntToString(uid[i])).append("|");
		userNomenateDB.uid = uid[i];
		userNomenateDB.add_list = add_list;
		userNomenateDB.black_list = black_list;
		userNomenateDB.friend_list = friend_list;
		CBasic::StringReplace(rec_list, temp, rep);
		userNomenateDB.rec_list = rec_list;
		rec_list = rec_list1;
		userNomenateDB.update_time = Time::GetGlobalTime();
		//ret = logicUserNomenate.SetUserNomenate(uid[i], userNomenateDB);
		ret = logicUserNomenate.AddUserNomenate(userNomenateDB);
		if(ret != 0)
		{
			cout << "add userNomenate err!errno=" << ret << endl;
			return ret;
		}
		temp.clear();*/

	ret = logicUserNomenate.setFriendRecList(10000243, "|10001984");
	if(ret!=0)
	{
		cout << "get nomenate err, ret=" << ret << endl;
	}


	return 0;
}
