/*
 * LogicUserMappingBasic.cpp
 *
 *  Created on: 2011-7-11
 *      Author: dada
 */

#include "LogicUserMapping.h"

int CLogicUserMapping::GetUid(unsigned &uid, PlatformType platform, const string &openid, bool autoAlt)
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
	if (ret == R_ERR_NO_DATA)
	{
		::SetError(R_ERR_NO_DATA, "db_get_user_mapping_fail");
	}

	return ret;
}

int CLogicUserMapping::LoginPlatformUser(
		PlatformType platform,
		OPUserInfo &userInfo,
		const string &iopenid,
		unsigned &uid,
		bool &isNewUser,
		string& via)
{
	int ret = 0;
	ret = GetUid(uid, platform, userInfo.OpenId);
	if (ret == 0)
	{
		ret = SetUserMapping(uid, isNewUser, platform, userInfo, via);
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
	vector<unsigned> uids;
	unsigned inviteUid = 0;
	if (!iopenid.empty())
	{
		CDataUserMapping dbUserMapping;
		dbUserMapping.GetAllMapping(iopenid,uids);
		ret = GetUid(inviteUid, platform, iopenid);
		if (ret != 0)
		{
			if(uids.empty())
				inviteUid = ADMIN_UID;
			else
				inviteUid = uids[0];
			//error_log("[iopenid invaild][platform=%d,iopenid=%s,openid=%s]",platform, iopenid.c_str(),userInfo.OpenId.c_str());
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
	uid = (unsigned)nextUid;
	ret = SetUserMapping(uid, isNewUser, platform, userInfo, via);
	if (ret != 0)
	{
		fatal_log("[SetUserMapping fail][ret=%d,uid=%u,platform=%d,openid=%s]",
				ret, uid, platform, userInfo.OpenId.c_str());
		return ret;
	}

	DATA_INFO_LOG("new_user", "uid=%u&platform=%d&openid=%s&invite_uid=%u",	uid, platform, userInfo.OpenId.c_str(), inviteUid);
	return 0;
}

int CLogicUserMapping::SetUserMapping(unsigned uid, bool newUser, PlatformType platform, OPUserInfo &userInfo, string& via)
{
	int ret = 0;
	if(newUser)
	{
		//add mapping
		CDataUserMapping dbUserMapping;
		ret = dbUserMapping.AddMapping(userInfo.OpenId, platform, uid, via);
		if(ret != 0)
		{
			error_log("[AddMapping fail][ret=%d,uid=%u,openid=%s,platform=%d]", ret, uid, userInfo.OpenId.c_str(), platform);
			DB_ERROR_RETURN_MSG("db_add_user_mapping_fail");
		}
	}
	else
	{
		//login mapping
		CDataUserMapping dbUserMapping;
		ret = dbUserMapping.UpdateLastLogin(userInfo.OpenId, platform);
		if(ret != 0)
		{
			error_log("[login mapping fail][ret=%d,uid=%u,openid=%s,platform=%d]", ret, uid, userInfo.OpenId.c_str(), platform);
			//DB_ERROR_RETURN_MSG("db_add_user_mapping_fail");
		}
	}

	return 0;
}
