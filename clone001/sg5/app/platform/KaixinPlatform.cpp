/*
 * KaixinPlatform.cpp
 *
 *  Created on: 2012-2-2
 *      Author: dada
 */

#include "KaixinPlatform.h"

#define KAIXIN_GET_USER_INFO_URL "https://api.kaixin001.com/users/me.json?session_key=%s"
#define KAIXIN_GET_APP_FRIENDS_URL "https://api.kaixin001.com/app/friends.json?num=50&session_key=%s"

int CKaixinPlatform::GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey)
{
	string url;
	String::Format(url, KAIXIN_GET_USER_INFO_URL, openkey.c_str());
	string response;
	if(!Network::HttpGetRequest(response, url))
	{
		m_errMsg = response;
		error_log("[http request fail][reason=HttpGetRequest,openkey=%s,errmsg=%s]",
				openkey.c_str(), m_errMsg.c_str());
		return -1;
	}

	userInfo.Clear();
	Json::Value jsonUserInfo;
	if(!Json::FromString(jsonUserInfo, response))
	{
		m_errMsg = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,openkey=%s,errmsg=%s,response=%s]",
				openkey.c_str(), m_errMsg.c_str(), response.c_str());
		return -1;
	}
	if(!Json::GetString(jsonUserInfo, "uid", userInfo.OpenId))
	{
		m_errMsg = "json_data_fail";
		error_log("[data error][reason=QueryFail,openkey=%s,data=%s]",
				openkey.c_str(), response.c_str());
		return -1;
	}
	Json::GetString(jsonUserInfo, "name", userInfo.Name);
	string sGender;
	Json::GetString(jsonUserInfo, "gender", sGender);
	if(sGender == "0")
	{
		userInfo.Gender = GD_MALE;
	}
	else if(sGender == "1")
	{
		userInfo.Gender = GD_FEMALE;
	}
	else
	{
		userInfo.Gender = GD_UNKNOW;
	}
	Json::GetString(jsonUserInfo, "logo50", userInfo.FigureUrl);
	return 0;
}

int CKaixinPlatform::GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey)
{
	string url;
	String::Format(url, KAIXIN_GET_APP_FRIENDS_URL, openkey.c_str());
	string response;
	if(!Network::HttpGetRequest(response, url))
	{
		m_errMsg = response;
		error_log("[http request fail][reason=HttpGetRequest,openid=%s,errmsg=%s]",
				openid.c_str(), m_errMsg.c_str());
		return -1;
	}

	friendList.clear();
	if(response.empty() || response == "[]")
	{
		return 0;
	}
	Json::Value data;
	if(!Json::FromString(data, response))
	{
		m_errMsg = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,openkid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errMsg.c_str(), response.c_str());
		return -1;
	}
	if(!Json::IsArray(data, "uids"))
	{
		m_errMsg = "json_data_fail";
		error_log("[data error][reason=QueryFail,openid=%s,data=%s]",
				openid.c_str(), response.c_str());
		return -1;
	}
	Json::Value &jsonFriends = data["uids"];
	unsigned friendsCount = jsonFriends.size();
	for(unsigned i = 0; i < friendsCount; i++)
	{
		string id;
		if(Json::GetString(jsonFriends, i, id))
		{
			friendList.push_back(id);
		}
	}
	return 0;
}

