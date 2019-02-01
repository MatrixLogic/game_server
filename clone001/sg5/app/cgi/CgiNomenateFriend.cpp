#include "LogicInc.h"
#include <stdlib.h>
#include "DataNoise.h"

#define LOW_UID 10000001	//等级为4，新手的好友推荐同此号	朋友平台
#define TEST_LOW_UID 10002352	//测试平台
#define MAX_ADD_LIST_SIZE 255
#define MAX_FRIEND_LIST_SIZE 2047
#define MAX_BLACK_LIST_SIZE 511
class CCgiNomenatFriend : public CCgiBase
{

public:
	CCgiNomenatFriend() : CCgiBase("friend_push")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_GET_UID);
		SetFeature(CF_CHECK_SESSION);
		SetFeature(CF_CHECK_PLATFORM);
		SetFeature(CF_CHECK_HASH);
		SetFeature(CF_CHECK_VERSION);
		SetFeature(CF_APPEND_HASH);
		SetFeature(CF_PARSE_DATA);
		SetFeature(CF_CRYPT_DATA);
		SetFeature(CF_DECRYPT_DATA);
		SetFeature(CF_COMPRESS_OUT);
	}

	CGI_ACTION_MAP_BEGIN
	CGI_SET_ACTION_MAP("getlist", GetRecList)
	CGI_SET_ACTION_MAP("add", AddFriends)
	CGI_SET_ACTION_MAP("reject", RejectFriends)
	CGI_SET_ACTION_MAP("remove", DelFriend)
	CGI_SET_ACTION_MAP("agree", AgreeAdd)
	CGI_SET_ACTION_MAP("disagree", DisagreeAdd)
	CGI_SET_ACTION_MAP("blacklist", BlackList)
	CGI_ACTION_MAP_END

	int GetRecList()
	{
		int ret;
		unsigned uid;
		Json::Value friends;
		vector<string> vecFriends, friendList, blackList;
		string rep("|"), rec_list(""), low_level_rec(""), strUid("|") ;
		CLogicUser data;
		DataUser userDB;
		CLogicUserBasic userBasic;
		DataUserBasic userBasicDB;
		CLogicUserNomenate logicUserNomenate;
		DataUserNomenate userNomenateDB;
		CLogicAlliance logicAlliance;
		DataAlliance allianceDB;

		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}
		//新手的好友推荐
		ret = logicUserNomenate.getFriendRecList(uid, rec_list);
		if(ret != 0)
			return ret;

		if(rec_list.empty())
		{
			logicUserNomenate.GetNewUserRecList(low_level_rec);
		}

		ret = data.GetUserLimit(uid, userDB);
		if(ret != 0)
			return 0;
		strUid = Convert::UIntToString(uid) + "|";
		if(rec_list.empty())
		{
			rec_list = low_level_rec;
			if(rec_list.find(strUid)!=string::npos)
			{
				CBasic::StringReplace(rec_list, strUid, "|");
			}
			ret = logicUserNomenate.setFriendRecList(uid, rec_list);
			if(ret != 0)
				return ret;
		}

		ret = logicUserNomenate.GetUserNomenate(uid, userNomenateDB);
		if (ret != 0)
			return ret;

		CBasic::StringSplitTrim(userNomenateDB.friend_list, "|", friendList);

		if(!userNomenateDB.friend_list.empty())
		{
			for(vector<string>::iterator itr = friendList.begin(); itr != friendList.end(); itr++)
			{
				string tempFriend("|");
				tempFriend.append(*itr).append("|");
				if(userNomenateDB.rec_list.find(tempFriend) != string::npos)
				{
					CBasic::StringReplace(userNomenateDB.rec_list, tempFriend, rep);
				}
			}
			ret = logicUserNomenate.SetUserNomenate(uid, userNomenateDB);
			if(ret != 0)
				return ret;
		}

		ret = logicUserNomenate.GetUserNomenate(uid, userNomenateDB);
		if (ret != 0)
			return ret;
		CBasic::StringSplitTrim(userNomenateDB.black_list, "|", blackList);

		if(!userNomenateDB.black_list.empty())
		{
			for(vector<string>::iterator itr = blackList.begin(); itr != blackList.end(); itr++)
			{
				string tempFriend1("|");
				tempFriend1.append(*itr).append("|");
				if(userNomenateDB.rec_list.find(tempFriend1) != string::npos)
				{
					CBasic::StringReplace(userNomenateDB.rec_list, tempFriend1, rep);
				}
			}
			ret = logicUserNomenate.SetUserNomenate(uid, userNomenateDB);
			if(ret != 0)
				return ret;
		}
		ret = logicUserNomenate.GetUserNomenate(uid, userNomenateDB);
			if (ret != 0)
				return ret;

		CBasic::StringSplitTrim(userNomenateDB.rec_list, "|", vecFriends);
		for(vector<string>::iterator itr = vecFriends.begin(); itr != vecFriends.end(); itr++)
		{
			unsigned friendId;
			Json::Value userdata;
			if(Convert::StringToUInt(friendId, *itr))
			{
				ret = userBasic.GetUserBasicLimit(friendId, OpenPlatform::GetType(), userBasicDB);
				if(ret != 0)
					continue;

				ret = data.GetUserLimit(friendId, userDB);
				if(ret != 0)
					continue;

				ret = logicAlliance.GetAllianceLimit(userDB.alliance_id, allianceDB);
				if(ret == 0)
					userdata["alliance_name"] = allianceDB.name;
				else
					userdata["alliance_name"] = "null";

				userdata["userid"] = userDB.uid;
				userdata["name"] = userBasicDB.name;
				userdata["level"] = userDB.level;
				userdata["gender"] = userBasicDB.gender;
				userdata["alliance_id"] = userDB.alliance_id;
			}
			else
			{
				CGI_SEND_LOG("[parse getlist fail][uid=%u,platform=%d,friend=%s]", uid, OpenPlatform::GetType(), itr->c_str());
			}
			friends.append(userdata);
		}
		m_jsonResult["pt"] = OpenPlatform::GetType();
		m_jsonResult["currenttime"] = Time::GetGlobalTime();
		m_jsonResult["friend"] = friends;
		CGI_SEND_LOG("uid=%u&platform=%d&alliance_id=%d", uid, OpenPlatform::GetType(), userDB.alliance_id);

		return R_SUCCESS;
	}
	int AddFriends()
	{
		int ret, flag = 0;
		unsigned fid, uid, friendId;
		string reclist("|"), oldlist(""), templist(""), addlist(""), addtemp("");
		char temp[35][10];
		memset(temp, 0x00, sizeof(temp));
		UidList friendlist;
		list<unsigned> delist;
		vector<string>vecFriends;
		Json::Value revFriends, userdata, add_list;
		CLogicUpdates logicUpdates;
		Json::Value updates;

		CLogicUserNomenate logicUserNomenate;
		DataUserNomenate userNomenateDB;
		CLogicUser data;
		DataUser userDB;
		CLogicUserBasic userBasic;
		DataUserBasic userBasicDB;
		CLogicAlliance logicAlliance;
		DataAlliance allianceDB;
		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}
		ret = userBasic.GetUserBasicLimit(uid, OpenPlatform::GetType(), userBasicDB);
		if(ret != 0)
		{
			error_log("uid = %d",uid);
				//return ret;
		}
		ret = data.GetUserLimit(uid, userDB);
		if(ret != 0)
			return ret;

		ret = logicAlliance.GetAllianceLimit(userDB.alliance_id, allianceDB);
		if(ret == 0)
		{
			userdata["alliance_name"] = allianceDB.name;
		}
		else
		{
			userdata["alliance_name"] = "null";
		}
		userdata["userid"] = userDB.uid;
		userdata["name"] = userBasicDB.name;
		userdata["level"] = userDB.level;
		userdata["gender"] = userBasicDB.gender;
		userdata["alliance_id"] = userDB.alliance_id;
		add_list.append(userdata);
		updates.resize(1);
		updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
		updates[(unsigned)0]["s"] = "ADDFRIEND";
		updates[(unsigned)0]["add_list"] = add_list;

		addtemp = Convert::UIntToString(uid);
		templist.append(addtemp).append("|");
		ret = logicUserNomenate.GetUserNomenate(uid, userNomenateDB);
		if(ret != 0)
			return ret;

		CBasic::StringSplitTrim(userNomenateDB.rec_list, "|", vecFriends);
		for(vector<string>::iterator itr = vecFriends.begin(); itr != vecFriends.end(); itr++)
		{
			if(Convert::StringToUInt(friendId, *itr))
			{
				friendlist.push_back(friendId);
			}
		}
		vecFriends.clear();
		Json::GetArray(m_data, "list", revFriends);
		int count = revFriends.size();
		for(int i=0;i<count;i++)
		{
			Json::GetUInt(revFriends, i, fid);
			delist.push_back(fid);
			ret = logicUserNomenate.getFriendAddList(fid, oldlist);
			if(ret != 0)
				return ret;
			if(oldlist.empty())
			{
				error_log("friend_add list is empty!!\n");
				addlist.append("|").append(templist);
			}else{
				if(oldlist.find(templist)==string::npos)
				{
					if(addlist.size() + 10 > MAX_ADD_LIST_SIZE)
					{
						// 申请列表已满，将原来列表清空
						addlist.clear();
						//continue;
					}
					if(addlist.empty())
					{
						addlist.append("|").append(templist);
					}else if(oldlist[oldlist.size() -1] != '|')
					{
						addlist.append(oldlist).append("|").append(templist);
					}else{
						addlist.append(oldlist).append(templist);
					}

				}else{
					// 已在添加列表中，不需要重新添加
					addlist.append(oldlist);
					//addlist.clear();
					continue;
				}
			}
			ret = logicUserNomenate.setFriendAddList(fid, addlist);
			if(ret!=0)
				return ret;
			logicUpdates.AddUpdates(fid, updates);
			addlist.clear();
		}
		for(list<unsigned>::iterator itr = delist.begin(); itr != delist.end(); itr++)
		{
			for(vector<unsigned>::iterator itr1 = friendlist.begin(); itr1 != friendlist.end(); itr1++)
			{
				if(*itr == *itr1)
				{
					friendlist.erase(itr1);
					break;
				}
			}
		}
		for(vector<unsigned>::iterator itr1 = friendlist.begin(); itr1 != friendlist.end(); itr1++)
		{
			sprintf(temp[flag], "%d", *itr1);
			reclist.append(temp[flag]).append("|");
		}
		ret = logicUserNomenate.setFriendRecList(uid, reclist);
		if(ret!=0)
			return ret;

		m_jsonResult["pt"] = OpenPlatform::GetType();
		m_jsonResult["currenttime"] = Time::GetGlobalTime();
		return R_SUCCESS;
	}
	int RejectFriends()
	{
		int ret, flag = 0;
		unsigned uid,fid;
		string reclist("|");
		char temp[35][10];
		UidList friendlist;
		list<unsigned> delist;
		vector<string>vecFriends;
		Json::Value revFriends;

		CLogicUserNomenate logicUserNomenate;
		DataUserNomenate userNomenateDB;

		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}
		ret = logicUserNomenate.GetUserNomenate(uid, userNomenateDB);
		if(ret != 0)
			return ret;

		CBasic::StringSplitTrim(userNomenateDB.rec_list, "|", vecFriends);
		for(vector<string>::iterator itr = vecFriends.begin(); itr != vecFriends.end(); itr++)
		{
			unsigned friendId;
			if(Convert::StringToUInt(friendId, *itr))
			{
				friendlist.push_back(friendId);
			}
		}
		vecFriends.clear();
		Json::GetArray(m_data, "list", revFriends);
		int count = revFriends.size();
		for(int i=0;i<count;i++)
		{
			Json::GetUInt(revFriends, i, fid);
			delist.push_back(fid);
		}
		for(list<unsigned>::iterator itr = delist.begin(); itr != delist.end(); itr++)
		{
			for(vector<unsigned>::iterator itr1 = friendlist.begin(); itr1 != friendlist.end(); itr1++)
			{
				if(*itr == *itr1)
				{
					friendlist.erase(itr1);
					break;
				}
			}
		}
		for(vector<unsigned>::iterator itr1 = friendlist.begin(); itr1 != friendlist.end(); itr1++)
		{
			sprintf(temp[flag], "%d", *itr1);
			reclist.append(temp[flag]).append("|");
		}
		ret = logicUserNomenate.setFriendRecList(uid, reclist);
		if(ret!=0)
			return ret;

		m_jsonResult["pt"] = OpenPlatform::GetType();
		m_jsonResult["currenttime"] = Time::GetGlobalTime();

		return R_SUCCESS;
	}
	int DelFriend()
	{
		int ret, count;
		unsigned uid, delUid;
		string tempFriend("|"), rep("|"), delStr(""), strUid("|");
		Json::Value revFriend;
		CLogicUserNomenate logicUserNomenate;
		DataUserNomenate userNomenateDB;

		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}
		strUid.append(Convert::UIntToString(uid)).append("|");
		Json::GetArray(m_data, "list", revFriend);
		count = revFriend.size();
		for(int i=0;i<count;i++)
		{
			Json::GetString(revFriend, i, delStr);
			Json::GetUInt(revFriend, i, delUid);
			tempFriend.append(delStr).append("|");
			ret = logicUserNomenate.GetUserNomenate(uid, userNomenateDB);
			if(ret != 0)
				return ret;
			CBasic::StringReplace(userNomenateDB.friend_list, tempFriend, rep);
			ret = logicUserNomenate.SetUserNomenate(uid, userNomenateDB);
			if(ret != 0)
				return ret;

			ret = logicUserNomenate.GetUserNomenate(delUid, userNomenateDB);
			if(ret != 0)
				return ret;
			CBasic::StringReplace(userNomenateDB.friend_list, strUid, rep);
			ret = logicUserNomenate.SetUserNomenate(delUid, userNomenateDB);
			if(ret != 0)
				return ret;

			tempFriend.clear();
		}
		return R_SUCCESS;
	}

	int AgreeAdd()
	{
		int ret, count;
		unsigned uid, recFriend;
		Json::Value revData, updates, tempTips;
		string recString(""), rep("|"), strUid("");
		CLogicUserNomenate logicUserNomenate;
		DataUserNomenate userNomenateDB;
		DataUserNomenate userNomenateDB1;
		CLogicUpdates logicUpdates;
		CLogicUserBasic userBasic;
		DataUserBasic userBasicDB;
		bool isFull = false;
		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}
		strUid.append(Convert::UIntToString(uid)).append("|");
		Json::GetArray(m_data, "list", revData);
		count = revData.size();
		ret = logicUserNomenate.GetUserNomenate(uid, userNomenateDB);
		if(ret != 0)
			return ret;

		ret = userBasic.GetUserBasicLimit(uid, OpenPlatform::GetType(), userBasicDB);
		if(ret != 0)
			return ret;
		for(int i=0;i<count;i++)
		{
			Json::GetUInt(revData, i, recFriend);
			Json::GetString(revData, i, recString);
			string tempRec("");
			tempRec.append(recString).append("|");
			CBasic::StringReplace(userNomenateDB.add_list, tempRec, rep);
			if(userNomenateDB.rec_list.find(tempRec) == string::npos)
			{
			}else{
				CBasic::StringReplace(userNomenateDB.rec_list, tempRec, "");
			}

			if(userNomenateDB.friend_list.empty())
			{
				userNomenateDB.friend_list.append(tempRec);
			}else{
				if(userNomenateDB.friend_list.find(tempRec)==string::npos && userNomenateDB.friend_list.length()<=1014)
				{
					userNomenateDB.friend_list.append(recString).append("|");
				}
//				if(userNomenateDB.friend_list.size() > 1024)
//				{
//					isFull =true;
//					break;
//				}

			}

			ret = logicUserNomenate.GetUserNomenate(recFriend, userNomenateDB1);
			if(ret != 0)
				return ret;

			if(userNomenateDB1.friend_list.empty())
			{
				userNomenateDB1.friend_list.append("|").append(strUid);
			}else{
				if(userNomenateDB1.friend_list.find(strUid)==string::npos && userNomenateDB1.friend_list.length()<=1014)
				{
					userNomenateDB1.friend_list.append(strUid);
				}
//				if(userNomenateDB1.friend_list.size() > 1023)
//				{
//					isFull =true;
//					continue;
//				}
			}
			ret = logicUserNomenate.SetUserNomenate(recFriend, userNomenateDB1);
			if(ret != 0)
				return ret;

			updates[(unsigned)0]["s"] = "AGREEADD";
			updates[(unsigned)0]["isAgree"] = 1;
			updates[(unsigned)0]["name"] = userBasicDB.name;
			updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
			logicUpdates.AddUpdates(recFriend, updates);
			tempRec.clear();
		}
		ret = logicUserNomenate.SetUserNomenate(uid, userNomenateDB);
		if(ret != 0)
			return ret;
		if(isFull)
		{

		}
		return R_SUCCESS;
	}
	int DisagreeAdd()
	{
		int ret, count;
		unsigned uid, recFriend;
		Json::Value revData, updates, tips, tempTips;
		string recString(""), rejectFriend(""), rep("|");
		CLogicUserNomenate logicUserNomenate;
		DataUserNomenate userNomenateDB;
		CLogicUpdates logicUpdates;
		CLogicUserBasic userBasic;
		DataUserBasic userBasicDB;

		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}
		Json::GetArray(m_data, "list", revData);
		count = revData.size();
		for(int i=0;i<count;i++)
		{
			Json::GetUInt(revData, i, recFriend);
			Json::GetString(revData, i, recString);
			rejectFriend.append("|").append(recString).append("|");

			ret = logicUserNomenate.GetUserNomenate(uid, userNomenateDB);
			if(ret != 0)
				continue;
			ret = userBasic.GetUserBasicLimit(uid, OpenPlatform::GetType(), userBasicDB);
			if(ret != 0)
				continue;

			CBasic::StringReplace(userNomenateDB.add_list, rejectFriend, rep);

			ret = logicUserNomenate.SetUserNomenate(uid, userNomenateDB);
			if(ret != 0)
				continue;

			updates[(unsigned)0]["s"] = "DISAGREEADD";
			updates[(unsigned)0]["isAgree"] = 0;
			updates[(unsigned)0]["name"] = userBasicDB.name;
			updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
			logicUpdates.AddUpdates(recFriend, updates);

			rejectFriend.clear();
			tips.clear();
		}
		return R_SUCCESS;
	}

	int BlackList()
	{
		int ret, count;
		unsigned uid;
		Json::Value blklist;
		string delStr(""), rep("|");
		CLogicUserNomenate logicUserNomenate;
		DataUserNomenate userNomenateDB;

		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}
		Json::GetArray(m_data, "list", blklist);
		count = blklist.size();
		for(int i=0;i<count;i++)
		{
			CDataNoise *pData = GetDataNoise();
			if (pData)
			{
				unsigned uidforbid = 0;
				Json::GetUInt(blklist, i, uidforbid);
				ret = pData->Add(uidforbid, uid);
				if(ret == 0)
				{
					CLogicAdmin logicAdmin;
					logicAdmin.AddForbidUser(uidforbid,Time::GetGlobalTime()+NOISE_SHUT);
				}
			}
			else
				error_log("[get data noise fail]");

			string delTemp(""), delTemp1("|");
			Json::GetString(blklist, i, delStr);
			delTemp.append(delStr).append("|");
			delTemp1.append(delStr).append("|");
			ret = logicUserNomenate.GetUserNomenate(uid, userNomenateDB);
			if(ret != 0)
				return ret;
			if(userNomenateDB.black_list.length()>1014)
			{
				PARAM_ERROR_RETURN_MSG("black_limit");
				return 0;
			}
			CBasic::StringReplace(userNomenateDB.friend_list, delTemp1, rep);
			if(userNomenateDB.black_list.empty())
			{
				userNomenateDB.black_list.append(delTemp1);
			}else{
				if(userNomenateDB.black_list.find(delTemp)==string::npos)
				{
					userNomenateDB.black_list.append(delTemp);
				}
			}
			ret = logicUserNomenate.SetUserNomenate(uid, userNomenateDB);
			if(ret != 0)
				return ret;
		}
		return R_SUCCESS;
	}

	CDataNoise* GetDataNoise(void)
	{
		static CDataNoise* data;
		if (data)
		{
			return data;
		}
		data = new CDataNoise();

		int ret = data->Init(CONFIG_NOISE_SHM,sem_noise);
		if (0 != ret)
		{
			error_log("Init data fail!");
			delete data;
			return NULL;
		}
		//debug_log("Init data!");
		return data;
	}
};

CGI_MAIN(CCgiNomenatFriend)
