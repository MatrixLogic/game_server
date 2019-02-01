/*
 * FetionPlatform.cpp
 *
 *  Created on: 2012-2-9
 *      Author: dada
 */

#include "FetionPlatform.h"

#define FETION_API_URL "http://app.api.fetion.com.cn/service"
#define FETION_GET_USER_INFO_BASE_STRING "api_key=%scall_id=%lluformat=JSONmethod=users.getInfosession_key=%suids=%sv=1%s"
#define FETION_GET_USER_INFO_PARAMS "api_key=%s&call_id=%llu&format=JSON&method=users.getInfo&session_key=%s&sig=%s&uids=%s&v=1"
#define FETION_GET_APP_FRIENDS_BASE_STRING "api_key=%scall_id=%lluformat=JSONmethod=friends.getAppUserssession_key=%sv=1%s"
#define FETION_GET_APP_FRIENDS_PARAMS "api_key=%s&call_id=%llu&format=JSON&method=friends.getAppUsers&session_key=%s&sig=%s&v=1"

static uint64_t GetFetionTimestamp()
{
	uint64_t s;
	s = time(NULL);
	unsigned ms;
	mtime(ms);
	return s * 1000 + ms % 1000;
}

int CFetionPlatform::GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey)
{
	string baseString;
	uint64_t ts = GetFetionTimestamp();
	String::Format(baseString, FETION_GET_USER_INFO_BASE_STRING,
			m_appId.c_str(), ts, openkey.c_str(), openid.c_str(), m_appKey.c_str());
	string sig = Crypt::Md5Encode(baseString);
	string params;
	String::Format(params, FETION_GET_USER_INFO_PARAMS,
			m_appId.c_str(), ts, Crypt::UrlEncode(openkey).c_str(), sig.c_str(), openid.c_str());
	string response;
	if(!Network::HttpPostRequest(response, FETION_API_URL, params))
	{
		m_errMsg = response;
		error_log("[http request fail][reason=HttpGetRequest,openid=%s,errmsg=%s,url=%s,data=%s]",
				openid.c_str(), m_errMsg.c_str(), FETION_API_URL, params.c_str());
		return -1;
	}

	userInfo.Clear();
	Json::Value jsonUsers;
	if(!Json::FromString(jsonUsers, response))
	{
		m_errMsg = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,openid=%s,errmsg=%s,basestring=%s,params=%s,response=%s]",
				openid.c_str(), m_errMsg.c_str(), baseString.c_str(), params.c_str(), response.c_str());
		return -1;
	}
	if(!jsonUsers.isArray() || jsonUsers.size() != 1)
	{
		m_errMsg = "json_data_error";
		error_log("[data error][reason=QueryFail,openid=%s,data=%s]",
				openid.c_str(), response.c_str());
		return -1;
	}
	Json::Value &jsonUserInfo = jsonUsers[0u];
	Json::GetString(jsonUserInfo, "uid", userInfo.OpenId);
	if(userInfo.OpenId != openid)
	{
		m_errMsg = "json_data_error";
		error_log("[data error][reason=QueryFail,openid=%s,data=%s]",
				openid.c_str(), response.c_str());
		return -1;
	}
	Json::GetString(jsonUserInfo, "nickname", userInfo.Name);
	Json::GetInt(jsonUserInfo, "gender", userInfo.Gender);
	Json::GetString(jsonUserInfo, "headurl", userInfo.FigureUrl);
	if(Json::IsObject(jsonUserInfo, "hometown"))
	{
		Json::GetString(jsonUserInfo["hometown"], "province", userInfo.Province);
		Json::GetString(jsonUserInfo["hometown"], "city", userInfo.City);
	}
	return 0;
}

int CFetionPlatform::GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey)
{
	string baseString;
	uint64_t ts = GetFetionTimestamp();
	String::Format(baseString, FETION_GET_APP_FRIENDS_BASE_STRING, m_appId.c_str(), ts, openkey.c_str(), m_appKey.c_str());
	string sig = Crypt::Md5Encode(baseString);
	string params;
	String::Format(params, FETION_GET_APP_FRIENDS_PARAMS, m_appId.c_str(), ts, Crypt::UrlEncode(openkey).c_str(), sig.c_str());
	string response;
	if(!Network::HttpPostRequest(response, FETION_API_URL, params))
	{
		m_errMsg = response;
		error_log("[http request fail][reason=HttpGetRequest,openid=%s,errmsg=%s]",
				openid.c_str(), m_errMsg.c_str());
		return -1;
	}

	friendList.clear();
	Json::Value jsonFriends;
	if(!Json::FromString(jsonFriends, response))
	{
		m_errMsg = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,openkid=%s,errmsg=%s,basestring=%s,params=%s,response=%s]",
				openid.c_str(), m_errMsg.c_str(), baseString.c_str(), params.c_str(), response.c_str());
		return -1;
	}
	if(!jsonFriends.isArray())
	{
		m_errMsg = "json_data_fail";
		error_log("[data error][reason=QueryFail,openid=%s,data=%s]",
				openid.c_str(), response.c_str());
		return -1;
	}
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

