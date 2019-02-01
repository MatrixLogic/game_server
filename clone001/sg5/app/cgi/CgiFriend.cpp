
#include "LogicInc.h"

class CCgiFriend : public CCgiBase
{

public:

	CCgiFriend() : CCgiBase("friend")
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
	CGI_SET_ACTION_MAP("getlist", GetFriend)
	CGI_SET_ACTION_MAP("removeblack", DelFromBalck)
	CGI_SET_ACTION_MAP("revokeblack", RegainFriend)
	CGI_SET_ACTION_MAP("getally", GetAlly)
	CGI_ACTION_MAP_END

	int GetFriend()
	{
		int encode = CCGIIn::GetCGIInt("encode");

		CLogicUserBasic logicUserBasic;
		DataUser user;
		CLogicUser logicUser;

		Json::Value friends, blacklist;
		int ret = logicUserBasic.GetFriendsJson(m_uid, OpenPlatform::GetType(), friends, (encode==1?true:false));
		if(ret != 0)
			return ret;

		ret = logicUser.GetUserLimit(m_uid, user);
		if(ret != 0)
			return ret;

		ret = logicUserBasic.GetBlacksJson(m_uid, OpenPlatform::GetType(), blacklist);
		if(ret != 0)
			return ret;

		m_jsonResult["pt"] = OpenPlatform::GetType();
		m_jsonResult["currenttime"] = Time::GetGlobalTime();
		m_jsonResult["friends"] = friends;
		m_jsonResult["black"] = blacklist;
		CGI_SEND_LOG("uid=%u&platform=%d&friends=%d", m_uid, OpenPlatform::GetType(), friends.size());
		return R_SUCCESS;
	}

	int DelFromBalck()
	{
		int ret, count;
		string black_list, rep("|");
		Json::Value blackFriend;
		CLogicUserNomenate logicUserNomenate;
		ret = logicUserNomenate.getFriendBlackList(m_uid, black_list);
		if(ret != 0)
			return ret;
		Json::GetArray(m_data, "list", blackFriend);
		count = blackFriend.size();
		for(int i=0; i<count; i++)
		{
			string delFriend("|"), data("");
			Json::GetString(blackFriend, i, data);
			delFriend.append(data).append("|");
			CBasic::StringReplace(black_list, delFriend, rep);

		}
		ret = logicUserNomenate.setFriendBlackList(m_uid, black_list);
		if(ret != 0)
			return ret;

		return 0;
	}

	int RegainFriend()
	{
		int ret, count;
		string  rep("|");
		Json::Value blackFriend;
		DataUserNomenate userNomenateDB;
		CLogicUserNomenate logicUserNomenate;
		ret = logicUserNomenate.GetUserNomenate(m_uid, userNomenateDB);
		if(ret != 0)
			return ret;
		Json::GetArray(m_data, "list", blackFriend);
		count = blackFriend.size();
		for(int i=0; i< count; i++)
		{
			string delFriend("|"), data("");
			Json::GetString(blackFriend, i, data);
			delFriend.append(data).append("|");
			CBasic::StringReplace(userNomenateDB.black_list, delFriend, rep);
			userNomenateDB.friend_list.append(data).append("|");
		}
		ret = logicUserNomenate.SetUserNomenate(m_uid, userNomenateDB);
		if(ret != 0)
			return ret;

		return 0;
	}
	//联盟好友
	int GetAlly()
	{
		unsigned alliance_id;
		CLogicUser data;
		DataUser userDB;
		CLogicUserBasic userBasic;
		DataUserBasic userBasicDB;
		Json::Value allys, allydata;
		CLogicAlliance logicAlliance;
		vector<DataAllianceMember> members;

		Json::GetUInt(m_data, "allianceid", alliance_id);
		int ret = logicAlliance.GetMembers(alliance_id, members);
		if(ret!=0)
			return ret;
		for(vector<DataAllianceMember>::iterator itr=members.begin(); itr!=members.end(); itr++)
		{
			unsigned allyId;
			allyId = (*itr).uid;
			ret = data.GetUserLimit(allyId, userDB);
			if(ret!=0)
				return ret;
			ret = userBasic.GetUserBasicLimit(allyId, OpenPlatform::GetType(), userBasicDB);
			if(ret!=0)
				return ret;

			allydata["userid"] = allyId;
			allydata["name"] = userBasicDB.name;
			allydata["level"] = userDB.level;
			allydata["gender"] = userBasicDB.gender;
			allydata["sstate"] = userDB.sstate;
			allys.append(allydata);
		}
		m_jsonResult["allys"] = allys;

		return 0;
	}

};

CGI_MAIN(CCgiFriend)
