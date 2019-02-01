#include "LogicInc.h"
typedef pair<unsigned, int> U_L_MAP;

#define ONLINE_INTERACT (24 * 3600)
#define MAX_REC_COUNT 26
static bool cmp(const U_L_MAP &x, const U_L_MAP &y)
{
	return x.second < y.second;
}
int main()
{
	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "init kernel fail" << endl;
		return 1;
	}
	Config::SetDomain(0);

	int ret, max = 0, mark;
	uint64_t u64Id;
	string rec_list("") ;
	DataUser userDB;
	CLogicUser logicUser;
	CLogicIdCtrl logicIdCtrl;
	CLogicUserNomenate logicNomenate;
	DataUserNomenate userNomenateDB;
	vector <U_L_MAP> vecUsers;
	vector <U_L_MAP> vecOnlineUsers;

	ret = logicIdCtrl.GetCurrentId(KEY_UID_CTRL, u64Id);
	if(ret != 0)
	{
		printf("get user id fail %d\n", ret);
		return ret;
	}

	unsigned now = Time::GetGlobalTime();
	unsigned userid = UID_MIN + 1;
	DataUser user;
	for( userid = Config::GetIntValue(CONFIG_UID_MIN); userid <= (unsigned)u64Id; ++userid)
	{
		ret = logicUser.GetUser(userid, user);
		if(ret == 0)
		{
				//users[user.uid] = user.level;
				vecUsers.push_back(make_pair(user.uid,user.level));
				if(now -user.last_active_time <= ONLINE_INTERACT )
				{
					vecOnlineUsers.push_back(make_pair(user.uid,user.level));
					++max ;
				}
		}
		else if(ret != R_ERR_NO_DATA)
		{
			printf("get user fail uid=[%u] ret=[%d]\n", userid, ret);
		}
	}

	sort(vecUsers.begin(), vecUsers.end(), cmp);
	sort(vecOnlineUsers.begin(),vecOnlineUsers.end(),cmp);

	mark = max - 26;
	int last_begin_index = 0;
	int onlineIndex = 0;
	int userIndex = 0;
	for(vector<U_L_MAP>::iterator itr=vecUsers.begin(); itr != vecUsers.end(); itr++)
	{
		ret = logicNomenate.GetUserNomenate(itr->first, userNomenateDB);
		if(ret != 0)
		{
			cout << "GetUserNomenate fail!, ret = " << ret << endl;
			break;
		}
		//add by aaron 20121129
		while(itr->second - 5 > vecOnlineUsers[last_begin_index].second && last_begin_index < mark)
		{
			last_begin_index++;
		}
		int i = 0;
		for(onlineIndex = last_begin_index; (onlineIndex < max && i <= MAX_REC_COUNT);onlineIndex++)
		{
				rec_list.append(Convert::UIntToString(vecOnlineUsers[onlineIndex].first)).append("|");
				i++;
		}
		// 从推荐里列表中去掉自己
		string temp("");
		temp.append(Convert::UIntToString(itr->first)).append("|");
		CBasic::StringReplace(rec_list, temp, "");
		vector<string> friend_list;
		CBasic::StringSplitTrim(userNomenateDB.black_list, "|", friend_list);
		for(vector<string>::iterator itr = friend_list.begin(); itr != friend_list.end();itr++)
		{
			temp.clear();
			temp.append(*itr).append("|");
			CBasic::StringReplace(rec_list, temp, "");
		}

		//add by aaron 20121129
		userNomenateDB.rec_list = rec_list;
		userNomenateDB.update_time = now;
		ret = logicNomenate.SetUserNomenate(itr->first, userNomenateDB);
		if(ret != 0)
		{
			cout << "SetUserNomenate fail!,ret = " << ret << endl;
			break;
		}
		rec_list.clear();
		userIndex++;
	}

	return 0;
}
