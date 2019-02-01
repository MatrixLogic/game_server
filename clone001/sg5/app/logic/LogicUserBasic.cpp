/*
 * LogicUserBasicBasic.cpp
 *
 *  Created on: 2011-7-11
 *      Author: dada
 */

#include "LogicUserBasic.h"

int CLogicUserBasic::GetUid(unsigned &uid, PlatformType platform, const string &openid, bool autoAlt)
{
	int ret = 0;
	CDataUserMapping dbUserMapping;
	ret = dbUserMapping.GetMapping(openid, platform, uid);
	if (ret == 0)
		return 0;
	if (ret != R_ERR_NO_DATA)
	{
		error_log("[GetMapping fail][ret=%d,openid=%s,platform=%d]", ret, openid.c_str(), platform);
		DB_ERROR_RETURN_MSG("db_get_user_mapping_fail");
	}
	/******close when add 3366  it's useless
	if (autoAlt && (platform == PT_PENGYOU || platform == PT_QZONE))
	{
		PlatformType platformAlt;
		if(platform == PT_PENGYOU)
		{
			platformAlt = PT_QZONE;
		}
		else
		{
			platformAlt = PT_PENGYOU;
		}
		ret = dbUserMapping.GetMapping(openid, platformAlt, uid);
		if(ret != 0 && ret != R_ERR_NO_DATA)
		{
			error_log("[GetMapping fail][ret=%d,openid=%s,platform=%d]", ret, openid.c_str(), platform);
			DB_ERROR_RETURN_MSG("db_get_user_mapping_fail");
		}
	}
	********/
	if (ret == R_ERR_NO_DATA)
	{
		::SetError(R_ERR_NO_DATA, "db_get_user_mapping_fail");
	}

	return ret;
}

int CLogicUserBasic::LoginPlatformUser(
		PlatformType platform,
		OPUserInfo &userInfo,
		const OPFriendList &friendList,
		const string &iopenid,
		unsigned &uid,
		bool &isNewUser)
{
	int ret = 0;
	bool isBlackUser = false;
	CheckBlackUser(userInfo.OpenId,isBlackUser);
	if (isBlackUser)
	{
		REFUSE_RETURN_MSG("being_blocked");
	}
	CLogicUser logicUser;
	ret = GetUid(uid, platform, userInfo.OpenId);
	if (ret == 0)
	{
		ret = SetUserBasic(uid, isNewUser, platform, userInfo, friendList);
		if (ret != 0)
		{
			return ret;
		}
		ret = logicUser.UpdateLogin(uid, platform);
		if (ret != 0)
		{
			return ret;
		}
		return 0;
	}
	if(ret != R_ERR_NO_DATA)
	{
		return ret;
	}
	// 新用户处理
	int full = 0;
	if (Config::GetIntValue(full, CONFIG_FULL))
	{
		if(full)
			return full;
	}
	isNewUser = true;
	unsigned inviteUid = 0;
	if (!iopenid.empty())
	{
		ret = GetUid(inviteUid, platform, iopenid);
		if (ret != 0)
		{
			inviteUid = 0;
			error_log("[iopenid invaild][platform=%d,iopenid=%s,openid=%s]",
					platform, iopenid.c_str(),userInfo.OpenId.c_str());
		}
	}
	//生成uid
	uint64_t nextUid = 0;
	CLogicIdCtrl logicIdCtrl;
	ret = logicIdCtrl.GetNextId(KEY_UID_CTRL, nextUid);
	if (ret != 0)
		return ret;
	if (!IsValidUid(nextUid))
	{
		fatal_log("[get uid error][uid=%llu]", nextUid);
		REFUSE_RETURN_MSG("sys_uid_use_up");
	}
/************允许跨服，暂时屏蔽**************************
	//检查UID是否超上限
	string strMaxUid;
	unsigned uidMax = UID_MAX;
	if (Config::GetValue(strMaxUid, CONFIG_UID_MAX))
	{
		if(Convert::StringToUInt(uidMax, strMaxUid))
		{
			if (nextUid > uidMax)
			{
				fatal_log("[get uid error][uid=%llu]", nextUid);
				REFUSE_RETURN_MSG("sys_uid_use_up");
			}
		}
	}
**************************************************/
	uid = (unsigned)nextUid;
	ret = logicUser.AddUser(uid, platform, inviteUid);
	if (ret != 0)
	{
		fatal_log("[AddUser fail][ret=%d,platform=%d,openid=%s]", ret, platform, userInfo.OpenId.c_str());
		return ret;
	}
	ret = SetUserBasic(uid, isNewUser, platform, userInfo, friendList);
	if (ret != 0)
	{
		fatal_log("[SetUserBasic fail][ret=%d,uid=%u,platform=%d,openid=%s]",
				ret, uid, platform, userInfo.OpenId.c_str());
		int result = logicUser.RemoveUser(uid);
		if (result != 0)
		{
			fatal_log("[RemoveUser fail][ret=%d,uid=%u,platform=%d,openid=%s]",
				ret, uid, platform, userInfo.OpenId.c_str());
		}
		return ret;
	}
/****************************************
	ret = OldServerToNewServerReward(uid, userInfo.OpenId);
	if (0 != ret)
	{
		error_log("[OldServerToNewServerReward fail][ret=%d,uid=%u]", ret, uid);
	}
****************************************/
	//邀请好友的处理
	logicUser.ProcessInvite(uid, inviteUid);

	if (platform == PT_FACEBOOK)
	{
		CLogicPlatformUser logicPlatformUser;
		logicPlatformUser.OnRegisterUser(userInfo.OpenId, platform, uid);
	}
	DATA_INFO_LOG("new_user", "uid=%u&platform=%d&openid=%s&invite_uid=%u",
				uid, platform, userInfo.OpenId.c_str(), inviteUid);

	//推荐好友的处理
	CLogicUserNomenate logicNomenate;
	DataUserNomenate userNomenateDB;
	userNomenateDB.uid = uid;
	ret = logicNomenate.AddUserNomenate(userNomenateDB);
	if(ret != 0)
	{
		fatal_log("add usernomenate fail,ret=%d", ret);
	}

	if((platform == PT_PENGYOU || platform == PT_3366 || platform == PT_QZONE || platform == PT_qqgame || platform == PT_FACEBOOK ) && inviteUid != 0)
	{
		CDataInviteMapping dbInviteMapping;
		ret = dbInviteMapping.AddMapping(inviteUid, uid);
		if (0 != ret)
		{
			error_log("[add invite mapping fail][uid=%u,inviteUid=%u,ret=%d]", uid, inviteUid, ret);
		}
	}
	// add by arron for tw new activity
	// add world user

	// add world user
	DATA_INFO_LOG("new_user", "uid=%u&platform=%d&openid=%s&invite_uid=%u",
			uid, platform, userInfo.OpenId.c_str(), inviteUid);

	return 0;
}

int CLogicUserBasic::GetFriendsJson(unsigned uid, PlatformType platform, Json::Value &friends, bool encode)
{
	int ret;
	UidList friendList;
	ret = GetFriends(uid, platform, friendList);
	if(ret != 0)
	{
		return ret;
	}

	//friendList.push_back(ADMIN_UID);

	map<unsigned, int> interacts;
	CLogicUserInteract logicInteract;
	ret = logicInteract.GetFriendInteracts(uid, interacts);

	DataUserBasic userBasic;
	DataUser user;
	CLogicUser logicUser;
	friends.resize(0);
	for(UidList::iterator itr = friendList.begin(); itr != friendList.end(); itr++)
	{
		if (uid == *itr)
			continue;

		//ret = GetUserBasicLimit(*itr, platform, userBasic);
		ret = GetUserBasicLimitSmart(*itr, platform, userBasic);
		if(ret != 0)
		{
			continue;
		}

		ret = logicUser.GetUserLimit(*itr, user);
		if(ret != 0)
		{
			continue;
		}
		if(user.status != US_ACTIVE)
		{
			continue;
		}

		Json::Value userData;
		userData["userid"] = *itr;
		if (encode)
		{
			String::Utf8Regularize(userBasic.name);
			userData["name"] = userBasic.name;
		}
		else
		{
			userData["name"] = userBasic.name;
		}

		userData["gender"] = userBasic.gender;
		userData["pic"] = userBasic.figure_url;
		userData["level"] = user.level;
		userData["sstate"] = user.sstate;
		userData["vt"] = userBasic.vip_type;
		userData["vl"] = userBasic.vip_level;
		userData["openid"] = userBasic.open_id;
		userData["online"] = IsOnlineUser(user.last_active_time)?1:0;
		userData["game_vip_level"] = user.viplevel;
		map<unsigned, int>::iterator itrInteract = interacts.find(*itr);
		if(itrInteract != interacts.end())
		{
			userData["ic"] = itrInteract->second;
		}
		friends.append(userData);
	}
	return 0;
}

int CLogicUserBasic::GetUserBasic(unsigned uid, PlatformType platform, DataUserBasic &user)
{
	int ret = 0;
	CDataUserBasic dbUserBasic;

	ret = dbUserBasic.GetUserBasic(uid, platform, user);
	if(ret != 0)
	{
		error_log("[GetUserBasic fail][ret=%d,uid=%u,platform=%d]", ret, uid, platform);
		DB_ERROR_RETURN_MSG("db_get_uesr_basic_fail");
	}
	return 0;
}

int CLogicUserBasic::GetUserBasicLimit(unsigned uid, PlatformType platform, DataUserBasic &user)
{
	CDataUserBasic dbUserBasic;
	int ret = dbUserBasic.GetUserBasicLimit(uid, platform, user);
	if (ret != 0)
	{
		error_log("[GetUserBasicLimit fail][ret=%d,uid=%u,platform=%d]", ret, uid, platform);
		DB_ERROR_RETURN_MSG("db_get_uesr_basic_fail");
	}
	return 0;
}

int CLogicUserBasic::GetUserBasicLimitSmart(unsigned uid, PlatformType platform, DataUserBasic &user)
{
	int ret = 0;
	CDataUserBasic dbUserBasic;
	if(platform == PT_PENGYOU || platform == PT_QZONE || platform == PT_3366 || platform == PT_qqgame)
	{
		ret = dbUserBasic.GetUserBasicLimit(uid, platform, user);
		if(ret == 0)
		{
			return 0;
		}
		if(ret != R_ERR_NO_DATA)
		{
			error_log("[GetUserBasicLimit fail][ret=%d,uid=%u,platform=%d]", ret, uid, platform);
			DB_ERROR_RETURN_MSG("db_get_uesr_basic_fail");
		}
	}
	ret = dbUserBasic.GetUserBasicRegisterLimit(uid, user);
	if(ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[GetUserBasicRegisterLimit fail][ret=%d,uid=%u,platform=%d]", ret, uid, platform);
		DB_ERROR_RETURN_MSG("db_get_uesr_basic_fail");
	}
	return 0;
}

int CLogicUserBasic::GetFriends(unsigned uid, PlatformType platform, UidList &friends)
{
	int ret = 0,ret2=0;
	string black_list("");
	CDataUserBasic dbUserBasic;
	CDataUserNomenate userNomenate;
	CLogicUserNomenate logicUserNomenate;
	DataUserNomenate userNomenateDB;

	string sFriends(""),sFriends1(""), sFriends2("");
	vector<string>vecfriendlist;

	ret = dbUserBasic.GetFriends(uid, platform, sFriends1);
	if(ret != 0)
	{
		error_log("[GetFriends fail][ret=%d,uid=%u,platform=%d]", ret, uid, platform);
		DB_ERROR_RETURN_MSG("db_get_uesr_basic_fail");
	}
	ret2 = userNomenate.getFriendList(uid, sFriends2);
	if(ret2 != 0)
	{
		error_log("[GetFriends fail][ret=%d,uid=%u]", ret2, uid);
		DB_ERROR_RETURN_MSG("db_uesr_nomanate_fail");
	}
	if(sFriends2.empty())
	{
		sFriends = sFriends1;
	}else{
		CBasic::StringSplitTrim(sFriends1, "|", vecfriendlist);
		for(vector<string>::iterator itr = vecfriendlist.begin(); itr != vecfriendlist.end(); itr++)
		{
			string tempFriend("");
			tempFriend.append(*itr).append("|");
			if(sFriends2.find(tempFriend)==string::npos)
			{
				sFriends2.append(tempFriend);
			}
		}
		sFriends = sFriends2;
	}
	friends.clear();
	vector<string> vecFriends, vecFriends1;
	ret = logicUserNomenate.getFriendBlackList(uid, black_list);
	CBasic::StringSplitTrim(black_list, "|", vecFriends1);
	CBasic::StringSplitTrim(sFriends, "|", vecFriends);

	for(vector<string>::iterator itr = vecFriends.begin(); itr != vecFriends.end(); itr++)
	{
		unsigned friendId;
		bool flag = true;

		Convert::StringToUInt(friendId, *itr);

		for(vector<string>::iterator itr1 = vecFriends1.begin(); itr1 != vecFriends1.end(); itr1++)
		{
			unsigned fid;
			Convert::StringToUInt(fid, *itr1);
			if(fid == friendId)
			{
				flag = false;
				break;
			}
		}
		if(flag == true)
			friends.push_back(friendId);
	}

	return 0;
}

int CLogicUserBasic::GetOidFriends(unsigned uid, PlatformType platform, string &oid_friends)
{
	int ret;
	CDataUserBasic userBasic;
	//DataUserBasic userBasicDB;

	ret = userBasic.GetOidFriends(uid, platform, oid_friends);
	if(ret != 0)
	{
		error_log("[GetOidFriends fail][ret=%d,uid=%u,platform=%d]", ret, uid, platform);
		DB_ERROR_RETURN_MSG("db_get_uesr_basic_fail");
	}
	return 0;
}
int CLogicUserBasic::SetOidFriends(unsigned uid, PlatformType platform, const string &oid_friends)
{
	int ret;
	CDataUserBasic userBasic;
	//DataUserBasic userBasicDB;

	ret = userBasic.SetOidFriends(uid, platform, oid_friends);
	if(ret != 0)
	{
		error_log("[SetOidFriends fail][ret=%d,uid=%u,platform=%d]", ret, uid, platform);
		DB_ERROR_RETURN_MSG("db_get_uesr_basic_fail");
	}
	return 0;
}
int CLogicUserBasic::GetBlacksJson(unsigned uid, PlatformType platform, Json::Value &blacks)
{
	int ret;
	unsigned fid;
	string black_list("");
	vector <string> vecFriends;
	CLogicUserNomenate logicUserNomenate;
	CLogicUserBasic logicUserBasic;
	DataUserBasic userBasicDB;
	CLogicUser data;
	DataUser userDB;
	DataUserNomenate userNomenateDB;
	CLogicAlliance logicAlliance;
	DataAlliance allianceDB;

	ret = logicUserNomenate.getFriendBlackList(uid, black_list);
	if(ret != 0)
	{
		error_log("[GetFriendBlackList fail][ret=%d,uid=%u,platform=%d]", ret, uid, platform);
		DB_ERROR_RETURN_MSG("db_get_uesr_blacklist_fail");
	}
	if(!black_list.empty())
	{
		CBasic::StringSplitTrim(black_list, "|", vecFriends);
		for(vector<string>::iterator itr=vecFriends.begin(); itr!=vecFriends.end(); itr++)
		{
			Json::Value blackdata;
			Convert::StringToUInt(fid, *itr);
			ret = logicUserBasic.GetUserBasicLimit(fid, platform, userBasicDB);
			if(ret != 0)
			{
				error_log("[GetUserBasicLimit fail][ret=%d,uid=%u,platform=%d]", ret, fid, platform);
				continue;
				//DB_ERROR_RETURN_MSG("db_get_uesr_basic_limit_fail");
			}

			ret = data.GetUserLimit(fid, userDB);
			if(ret != 0)
			{
				//return ret;
				continue;
			}

			ret = logicAlliance.GetAllianceLimit(userDB.alliance_id, allianceDB);
			if(ret == 0)
				blackdata["alliance_name"] = allianceDB.name;
			else
				blackdata["alliance_name"] = "null";

			blackdata["userid"] = fid;
			blackdata["name"] = userBasicDB.name;
			blackdata["level"] = userDB.level;
			blackdata["gender"] = userBasicDB.gender;
			blackdata["alliance_id"] = userDB.alliance_id;

			blacks.append(blackdata);
		}
		vecFriends.clear();
	}
	return 0;
}
bool CLogicUserBasic::IsFriend(unsigned uid, PlatformType platform, unsigned fid)
{
	int ret = 0, i;
	string sFriends("|");
	UidList friendlist;
	ret = GetFriends(uid, platform, friendlist);
	if(ret != 0)
	{
		error_log("[GetFriends fail][ret=%d,uid=%u,platform=%d]", ret, uid, platform);
		return false;
	}
	for(UidList::const_iterator itr=friendlist.begin(); itr!=friendlist.end(); itr++)
	{
		string temp("");
		temp.append(Convert::UIntToString(*itr)).append("|");
		sFriends.append(temp);

	}
	string sFind;
	String::Format(sFind, "|%u|", fid);
	if(sFriends.find(sFind) == string::npos)
	{
		return false;
	}
	sFriends.clear();
	return true;
}

int CLogicUserBasic::RemoveUserMapping(unsigned uid)
{
	//只删除Mapping，不删除UserBasic，移除朋友
	int ret = 0;
	CDataUserMapping dbUserMapping;
	vector<DataOpenidPlatform> openids;
	CDataUserBasic dbUserBasic;
	ret = dbUserBasic.GetOpenidList(uid, openids);
	if(ret != 0)
	{
		error_log("[GetOpenidList fail][ret=%,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("db_get_uesr_basic_fail");
	}
	for(vector<DataOpenidPlatform>::iterator itr = openids.begin(); itr != openids.end(); itr++)
	{
		ret = dbUserMapping.RemoveMapping(itr->open_id, itr->platform);
		if(ret != 0)
		{
			error_log("[RemoveMapping fail][ret=%,uid=%u,openid=%s,platform=%d]",
					ret, uid, itr->open_id.c_str(), itr->platform);
			DB_ERROR_RETURN_MSG("db_rm_uesr_basic_fail");
		}
	}
	return 0;
}

int CLogicUserBasic::SetUserBasic(unsigned uid, bool newUser, PlatformType platform, OPUserInfo &userInfo, const OPFriendList &friendList)
{
	int ret = 0;
	DataUserBasic userOld;
	CDataUserBasic dbUser;
	bool add = true;
	if(!newUser)
	{
		ret = dbUser.GetUserBasicExt(uid, platform, userOld);
		if(ret != 0)
		{
			if(ret != R_ERR_NO_DATA)
			{
				error_log("[GetUserBasicExt fail][ret=%d,uid=%u,platform=%d,openid=%s]", ret, uid, platform, userInfo.OpenId.c_str());
				DB_ERROR_RETURN_MSG("db_get_user_basic_fail");
			}
		}
		else
		{
			add = false;
		}
	}

	DataUserBasic user;
	user.uid = uid;
	user.platform = platform;
	if(add)
	{
		user.is_register_platform = newUser ? True : False;
	}
	else
	{
		user.is_register_platform = userOld.is_register_platform;
	}
	user.open_id = userInfo.OpenId;
	user.name = userInfo.Name;
	user.gender = userInfo.Gender;
	user.figure_url = userInfo.FigureUrl;
	user.nation = userInfo.Nation;
	user.province = userInfo.Province;
	user.city = userInfo.City;
	user.extra = userOld.extra;

	bool b_3366_changed = false;
	if(platform == PT_PENGYOU || platform == PT_QZONE || platform == PT_3366 || platform == PT_qqgame)
	{
		user.vip_type = VT_NORMAL;
		if(userInfo.ExtraIntInfo["bIsYearVip"])
			user.vip_type |= VT_QQ_YELLOW_YEAR;
		if(userInfo.ExtraIntInfo["bIsVip"])
			user.vip_type |= VT_QQ_YELLOW;
		if(userInfo.ExtraIntInfo["is_blue_vip"])
			user.vip_type |= VT_QQ_BLUE;
		if(userInfo.ExtraIntInfo["is_blue_year_vip"])
			user.vip_type |= VT_QQ_BLUE_YEAR;
		if(userInfo.ExtraIntInfo["is_super_blue_vip"])
			user.vip_type |= VT_QQ_SUPER_BULE;

		user.vip_level = (userInfo.ExtraIntInfo["blue_vip_level"])*16 + userInfo.ExtraIntInfo["nVipLevel"];
		//error_log("[SetUserBasic]user.vip_level=%d,blue_vip_level=%d,blue_vip_level*16=%d,nVipLevel=%d",user.vip_level,userInfo.ExtraIntInfo["blue_vip_level"],userInfo.ExtraIntInfo["blue_vip_level"]*16,userInfo.ExtraIntInfo["nVipLevel"]);

		Json::FastWriter writer;
		Json::Value extraJson;
		if (!user.extra.empty())
		{
			Json::Reader reader;
			reader.parse(user.extra, extraJson);
		}

		if(!extraJson.isMember("3366_level") || !extraJson["3366_level"].isInt() || extraJson["3366_level"].asInt() != userInfo.ExtraIntInfo["3366_level"]){
			extraJson["3366_level"] = userInfo.ExtraIntInfo["3366_level"];
			b_3366_changed = true;
		}
		if(!extraJson.isMember("3366_level_name") || !extraJson["3366_level_name"].isString() || extraJson["3366_level_name"].asString() != userInfo.ExtraStrInfo["3366_level_name"]){
			extraJson["3366_level_name"] = userInfo.ExtraStrInfo["3366_level_name"];
			b_3366_changed = true;
		}
		if(!extraJson.isMember("3366_grow_level") || !extraJson["3366_grow_level"].isInt() || extraJson["3366_grow_level"].asInt() != userInfo.ExtraIntInfo["3366_grow_level"]){
			extraJson["3366_grow_level"] = userInfo.ExtraIntInfo["3366_grow_level"];
			b_3366_changed = true;
		}
		if(!extraJson.isMember("3366_grow_value") || !extraJson["3366_grow_value"].isInt() || extraJson["3366_grow_value"].asInt() != userInfo.ExtraIntInfo["3366_grow_value"]){
			extraJson["3366_grow_value"] = userInfo.ExtraIntInfo["3366_grow_value"];
			b_3366_changed = true;
		}
		if(b_3366_changed)
			user.extra = writer.write(extraJson);
	}
	else
	{
		if (add)
		{
			user.vip_type = VT_NORMAL;
			user.vip_level = 0;
		}
		else
		{
			user.vip_type = userOld.vip_type;
			user.vip_level = userOld.vip_level;
		}
	}

	//当存在旧数据，且新数据为空时，信任旧数据
	if(!add)
	{
		if(user.name.empty())
		{
			user.name = userOld.name;
		}
		if(user.gender == GD_UNKNOW)
		{
			user.gender = userOld.gender;
		}
		if(user.figure_url.empty())
		{
			user.figure_url = userOld.figure_url;
		}
		if(user.nation.empty())
		{
			user.nation = userOld.nation;
		}
		if(user.province.empty())
		{
			user.province = userOld.province;
		}
		if(user.city.empty())
		{
			user.city = userOld.city;
		}
	}
	if (user.name.length() > 31)
	{
		user.name = user.name.substr(0, 31);
		String::Utf8Regularize(user.name);
	}

	if (add)
	{
		user.fnum = 0;
		user.friends = "|";
		user.oid_friends = "|";
		for (OPFriendList::const_iterator it = friendList.begin(); it != friendList.end(); it++)
		{
			if (*it == userInfo.OpenId)
				continue;
			unsigned fid = 0;
			ret = GetUid(fid, platform, *it, false);
			if (ret != 0)
			{
				//error_log("[Get fid fail][ret=%d,platform=%d,openid=%s]",ret,platform,it->c_str());
				continue;
			}
			user.fnum++;
			user.friends.append(CTrans::UTOS(fid)).append("|");
			user.oid_friends.append(*it).append("|");
		}
	}
	else
	{
		user.fnum = userOld.fnum;
		user.friends = userOld.friends;
		user.oid_friends = userOld.oid_friends;
		DataUser userData;
		CLogicUser logicUser;
		ret = logicUser.GetUserLimit(uid, userData);
		if (ret == 0)
		{
			int inter = CTime::GetDayInterval(userData.last_login_time, Time::GetGlobalTime());
			if (inter > 0)	//每天第一次登录删除好友
			{
				OPFriendList foids;
				CBasic::StringSplitTrim(user.oid_friends, "|", foids);
				map<unsigned, bool> oidsMap;
				vector<string> fids;
				CBasic::StringSplitTrim(user.friends, "|", fids);
				for (vector<string>::const_iterator it = fids.begin(); it != fids.end(); it++)
				{
					unsigned fId = 0;
					if(Convert::StringToUInt(fId, *it))
					{
						oidsMap[fId] = true;
					}
				}
				user.oid_friends = "|";
				for (OPFriendList::const_iterator it = foids.begin(); it != foids.end(); it++)
				{
					OPFriendList::const_iterator it2 = friendList.begin();
					for (; it2 != friendList.end(); it2++)
					{
						if (*it2 == *it)
							break;
					}
					if (it2 == friendList.end())
					{
						unsigned fuidTmp = 0;
						ret = GetUid(fuidTmp, platform, *it, false);
						if (ret == 0)
						{
							oidsMap[fuidTmp] = false;
						}
					}
					else
					{
						user.oid_friends.append(*it).append("|");
					}
				}
				user.fnum = 0;
				user.friends = "|";
				for (map<unsigned, bool>::const_iterator it = oidsMap.begin(); it != oidsMap.end(); it++)
				{
					if (it->second)
					{
						user.friends.append(CTrans::UTOS(it->first)).append("|");
						user.fnum++;
					}
				}
			}
		}

		for (OPFriendList::const_iterator it = friendList.begin(); it != friendList.end(); it++)
		{
			if (*it == userInfo.OpenId)
				continue;
			string match = "|" + *it + "|";
			if (user.oid_friends.find(match) != string::npos)
				continue;
			unsigned fid = 0;
			ret = GetUid(fid, platform, *it, false);
			if (ret != 0)
			{
				//error_log("[Get fid fail][ret=%d,platform=%d,openid=%s]",ret,platform,it->c_str());
				continue;
			}
			match = "|" + CTrans::UTOS(fid) + "|";
			if (user.friends.find(match) == string::npos)
			{
				user.fnum++;
				user.friends.append(CTrans::UTOS(fid)).append("|");
			}
			user.oid_friends.append(*it).append("|");
		}
	}

	if(add)
	{
		ret = dbUser.AddUserBasicExt(user);
		if(ret != 0)
		{
			error_log("[AddUserBasicExt fail][ret=%d,uid=%u,platform=%d,openid=%s]", ret, uid, platform, userInfo.OpenId.c_str());
			DB_ERROR_RETURN_MSG("db_add_user_basic_fail");
		}
		//add mapping
		CDataUserMapping dbUserMapping;
		ret = dbUserMapping.AddMapping(user.open_id, platform, uid);
		if(ret != 0)
		{
			error_log("[AddMapping fail][ret=%d,uid=%u,openid=%s,platform=%d]", ret, uid, user.open_id.c_str(), platform);
			DB_ERROR_RETURN_MSG("db_add_user_mapping_fail");
		}
	}
	else
	{
		if(user.open_id != userOld.open_id ||
			user.name != userOld.name ||
			user.gender != userOld.gender ||
			user.figure_url != userOld.figure_url ||
			user.nation != userOld.nation ||
			user.province != userOld.province ||
			user.city != userOld.city ||
			user.vip_type != userOld.vip_type ||
			user.vip_level != userOld.vip_level ||
			user.friends != userOld.friends ||
			user.oid_friends != userOld.oid_friends ||
			b_3366_changed)
		{
			ret = dbUser.SetUserBasicExt(uid, platform, user);
			if(ret != 0)
			{
				error_log("[SetUser fail][ret=%d,uid=%u,platform=%d]", ret, uid, platform);
				DB_ERROR_RETURN_MSG("db_set_user_basic_fail");
			}
		}
	}

	return 0;
}

int CLogicUserBasic::GetOpenKey(unsigned uid, string &openid, string &openkey)
{
	string sessionData = Session::GetValue(uid, SESSION_DATA);
	Json::Value session;
	Json::Reader reader;
	if (sessionData.empty() || !reader.parse(sessionData, session))
	{
		error_log("[parse session fail][uid=%u]",uid);
		DB_ERROR_RETURN_MSG("session_error");
	}
	openid = session.get("oid", "").asString();
	openkey = session.get("okey", "").asString();
	return 0;
}

int CLogicUserBasic::GetWorldUserInfo(unsigned uid, int platform,string &signature,string &figure_url,string &name)
{
	CDataUserBasic userBasic;
	int ret = userBasic.GetWorldUserInfo(uid,platform,signature,figure_url,name);

	return ret;
}
int CLogicUserBasic::SetWorldUserInfo(unsigned uid, int platform,const string &signature)
{
	CDataUserBasic userBasic;
	int ret = userBasic.SetWorldUserInfo(uid,platform,signature);

	return ret;
}

int CLogicUserBasic::OldServerToNewServerReward(unsigned uid, string &openId)
{
	int ret = 0;
	CDataUserAccCharge userAcccharge;
	DataUserReward data;
	ret = userAcccharge.GetAccCharge(openId, data);
	if (0 != ret)
	{
		if (R_ERR_NO_DATA != ret)
		{
			error_log("[GetAccCharge fail][ret=%d,openId=%s]", ret, openId.c_str());
		}
		return ret;
	}

	const int levelRewardCash[6] = {0, 2000, 3000, 4000, 5000, 6000};
	const int levelRewardCoins[6] = {0, 2500, 4000, 6000, 8000, 10000};

	int level = (data.level - 1) / 10;
	if (level > 5) level = 5;

	DataPay pay;
	pay.cash = data.accCharge * 3 + levelRewardCash[level];
	pay.coins = levelRewardCoins[level];
	CLogicPay logicPay;
	ret = logicPay.ChangePay(uid, pay.cash, pay.coins, "OldServerToNewServerReward");
	if(0 != ret)
	{
		return ret;
	}

	CDataUser dbUser;
	ret = dbUser.AddAccCharge(uid, data.accCharge);
	if (0 != ret)
	{
		error_log("[AddAccCharge fail][ret=%d,uid=%u]",ret, uid);
		return ret;
	}

	ret = userAcccharge.RemoveAccCharge(openId);
	if (0 != ret)
	{
		error_log("[RemoveAccCharge fail][ret=%d,openId=%s]", ret, openId.c_str());
		return ret;
	}
	return R_SUCCESS;
}

int CLogicUserBasic::CheckBlackUser(const string &openid,bool &black)
{
	black = false;
	int ret = 0;
	CDataBlackUser dbBlackUser;
	DataBlackUser data;
	ret = dbBlackUser.GetBlackUser(openid, data);
	if (0 == ret && data.block_time > Time::GetGlobalTime())
	{
		black = true;
	}
	return 0;
}
