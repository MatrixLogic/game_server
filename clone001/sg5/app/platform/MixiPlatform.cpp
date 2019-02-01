/*
 * MixiPlatform.cpp
 *
 *  Created on: 2011-8-29
 *      Author: dada
 */

#include "MixiPlatform.h"

#define MIXI_TOKEN_URL "https://secure.mixi-platform.com/2/token?grant_type=authorization_code&client_id=%s&client_secret=%s&code=%s&redirect_uri=%s"
#define MIXI_USER_INFO "http://api.mixi-platform.com/2/people/@me/@self"
#define MIXI_FRIEND "http://api.mixi-platform.com/2/people/@me/@friends?format=json&filterBy=hasApp"

int CMixiPlatform::GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey)
{
	int ret = GetAccessToken(openid, openkey);
	if(ret != 0)
	{
		return ret;
	}

	string response;
	map<string, string> headers;
	headers["Authorization"] = string("OAuth ") + m_lastToken;
	if(!Network::HttpGetRequest(response, MIXI_USER_INFO, headers))
	{
		m_errMsg = response;
		error_log("[http request fail][reason=HttpGetRequest,url=userinfo,openid=%s,errmsg=%s]",
				openid.c_str(), m_errMsg.c_str());
		return -1;
	}
	Json::Value jsonUserInfo;
	if(!Json::FromString(jsonUserInfo, response))
	{
		m_errMsg = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,url=userinfo,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errMsg.c_str(), response.c_str());
		return -1;
	}
	if(!Json::IsObject(jsonUserInfo, "entry"))
	{
		error_log("[data error][reason=QueryFail,url=userinfo,openid=%s,errmsg=%s]",
				openid.c_str(), m_errMsg.c_str());
		return -1;
	}
	Json::Value &jsonUserEntry = jsonUserInfo["entry"];
	Json::GetString(jsonUserEntry, "id", userInfo.OpenId);
	Json::GetString(jsonUserEntry, "displayName", userInfo.Name);
	userInfo.Gender = GD_UNKNOW;
	Json::GetString(jsonUserEntry, "thumbnailUrl", userInfo.FigureUrl);
	return 0;
}

int CMixiPlatform::GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey)
{
	if(openid != m_lastOpenId)
	{
		int ret = GetAccessToken(openid, openkey);
		if(ret != 0)
		{
			return ret;
		}
	}

	string response;
	map<string, string> headers;
	headers["Authorization"] = string("OAuth ") + m_lastToken;
	if(!Network::HttpGetRequest(response, MIXI_FRIEND, headers))
	{
		m_errMsg = response;
		error_log("[http request fail][reason=HttpGetRequest,url=friend,openid=%s,errmsg=%s]",
				openid.c_str(), m_errMsg.c_str());
		return -1;
	}
	Json::Value jsonFriend;
	if(!Json::FromString(jsonFriend, response))
	{
		m_errMsg = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,url=friend,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errMsg.c_str(), response.c_str());
		return -1;
	}

	friendList.clear();

	return 0;
}

int CMixiPlatform::GetAccessToken(const string &openid, const string &openkey)
{
	//get token
	string tokenUrl;
	String::Format(tokenUrl, MIXI_TOKEN_URL, GetAppId().c_str(), GetAppKey().c_str(),
			openkey.c_str(), Crypt::UrlEncode(GetConfig("game_home")).c_str());
	string response;
	if(!Network::HttpGetRequest(response, tokenUrl))
	{
		m_errMsg = response;
		error_log("[http request fail][reason=HttpGetRequest,url=token,openid=%s,errmsg=%s]",
				openid.c_str(), m_errMsg.c_str());
		return -1;
	}
	Json::Value jsonToken;
	if(!Json::FromString(jsonToken, response))
	{
		m_errMsg = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,url=token,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errMsg.c_str(), response.c_str());
		return -1;
	}
	if(Json::GetString(jsonToken, "access_token", m_lastToken))
	{
		error_log("[data error][reason=QueryFail,url=token,openid=%s,errmsg=%s]",
				openid.c_str(), m_errMsg.c_str());
		return -1;
	}
	m_lastOpenId = openid;
	return 0;
}


