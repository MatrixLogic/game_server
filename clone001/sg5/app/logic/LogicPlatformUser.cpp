/*
 * LogicPlatformUser.cpp
 *
 *  Created on: 2011-12-14
 *      Author: dada
 */

#include "LogicPlatformUser.h"

int CLogicPlatformUser::SetUserData(const string &openid, PlatformType platform, const Json::Value &data)
{
	int ret;
	CDataPlatformUser dbPlatformUser;
	ret = dbPlatformUser.SetData(openid, platform, Json::ToString(data));
	if(ret != 0)
	{
		error_log("[SetData fail][ret=%d,openid=%s,platform=%d]", ret, openid.c_str(), platform);
		DB_ERROR_RETURN_MSG("set_platform_user_fail");
	}
	return 0;
}

int CLogicPlatformUser::GetUserData(const string &openid, PlatformType platform, Json::Value &data)
{
	int ret;
	CDataPlatformUser dbPlatformUser;
	string sData;
	ret = dbPlatformUser.GetData(openid, platform, sData);
	if(ret == 0)
	{
		Json::FromString(data, sData);
	}
	else if(ret == R_ERR_NO_DATA)
	{
		ret = dbPlatformUser.AddData(openid, platform,  "{}");
		if(ret != 0)
		{
			error_log("[AddData fail][ret=%d,openid=%s,platform=%d]", ret, openid.c_str(), platform);
			DB_ERROR_RETURN_MSG("add_platform_user_fail");
		}
		data = Json::Value(Json::objectValue);
	}
	else
	{
		error_log("[GetData fail][ret=%d,openid=%s,platform=%d]", ret, openid.c_str(), platform);
		DB_ERROR_RETURN_MSG("get_platform_user_fail");
	}
	return 0;
}

int CLogicPlatformUser::RemoveUserData(const string &openid, PlatformType platform)
{
	int ret;
	CDataPlatformUser dbPlatformUser;
	ret = dbPlatformUser.RemoveData(openid, platform);
	if(ret != 0)
	{
		error_log("[RemoveData fail][ret=%d,openid=%s,platform=%d]", ret, openid.c_str(), platform);
		DB_ERROR_RETURN_MSG("remove_platform_user_fail");
	}
	return 0;
}

int CLogicPlatformUser::AddPendingMessage(const string &openid, PlatformType platform, uint64_t messageId)
{
	int ret;
	Json::Value data;
	ret = GetUserData(openid, platform, data);
	if(ret != 0)
	{
		return ret;
	}
	if(!Json::IsArray(data, "PendingMessage"))
	{
		data["PendingMessage"] = Json::Value(Json::arrayValue);
	}
	Json::Value &pendingMessage = data["PendingMessage"];
	unsigned size = pendingMessage.size();
	for(unsigned i = 0; i < size; i++)
	{
		uint64_t pendingMessageId;
		if(Json::GetUInt64(pendingMessage, i, pendingMessageId))
		{
			if(pendingMessageId == messageId)
			{
				error_log("[exist_pending_messsage][openid=%s,platform=%d,messageId=%llu]",
						openid.c_str(), platform, messageId);
				return 0;
			}
		}
	}
	pendingMessage[size] = Convert::UInt64ToString(messageId);
	ret = SetUserData(openid, platform, data);
	if(ret != 0)
	{
		return ret;
	}
	return 0;
}

int CLogicPlatformUser::OnRegisterUser(const string &openid, PlatformType platform, unsigned uid)
{
	int ret;
	CDataPlatformUser dbPlatformUser;
	string sData;
	ret = dbPlatformUser.GetData(openid, platform, sData);
	if(ret != 0)
	{
		if(ret != R_ERR_NO_DATA)
		{
			error_log("[GetData fail][ret=%d,openid=%s,platform=%d]", ret, openid.c_str(), platform);
			DB_ERROR_RETURN_MSG("get_platform_user_fail");
		}
		return 0;
	}
	Json::Value data;
	Json::FromString(data, sData);
	if(Json::IsArray(data, "PendingMessage"))
	{
		Json::Value pendingMessage = data["PendingMessage"];
		vector<uint64_t> messageIds;
		unsigned size = pendingMessage.size();
		for(unsigned i = 0; i < size; i++)
		{
			uint64_t pendingMessageId;
			if(Json::GetUInt64(pendingMessage, i, pendingMessageId))
			{
				messageIds.push_back(pendingMessageId);
			}
		}
		CLogicMessage logicMessage;
		ret = logicMessage.SetMessagesTarget(messageIds, uid);
	}
	ret = RemoveUserData(openid, platform);
	return 0;
}
