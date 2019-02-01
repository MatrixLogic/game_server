/*
 * SinaWbPlatform.cpp
 *
 *  Created on: 2011-5-16
 *      Author: dada
 */

#include "SinaWbPlatform.h"
#include "Common.h"

#define URL_SINAWB_GET_USER_INFO "https://api.t.sina.com.cn/users/show/%s.json?access_token=%s"
#define URL_SINAWB_GET_FRIENDS "https://api.t.sina.com.cn/friends/ids/%s.json?access_token=%s"
#define URL_SINAWB_GET_FOLLOWERS "https://api.t.sina.com.cn/followers/ids/%s.json?access_token=%s"

CSinaWbPlatform::CSinaWbPlatform()
{
}

CSinaWbPlatform::~CSinaWbPlatform()
{
}

int CSinaWbPlatform::Initialize(const string &appid, const string &appkey, map<string, string> &params)
{
	IOpenPlatform::Initialize(appid, appkey, params);
	return 0;
}

int CSinaWbPlatform::GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey)
{
	string url;
	string response;
	String::Format(url, URL_SINAWB_GET_USER_INFO, openid.c_str(), openkey.c_str());
	if(!Network::HttpGetRequest(response, url))
	{
		m_errorMessage = response;
		error_log("[http request fail][reason=HttpGetRequest,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}
	Json::Value jsonUserInfo;
	if(!Json::FromString(jsonUserInfo, response))
	{
		m_errorMessage = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}
	if(Json::GetString(jsonUserInfo, "error", m_errorMessage))
	{
		error_log("[data error][reason=QueryFail,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}
	if(!jsonUserInfo.isMember("id"))
	{
		m_errorMessage = "query_fail";
		error_log("[data error][reason=QueryFail,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}

	userInfo.Clear();
	Json::GetString(jsonUserInfo, "id", userInfo.OpenId);
	Json::GetString(jsonUserInfo, "name", userInfo.Name);
	string sGender;
	Json::GetString(jsonUserInfo, "gender", sGender);
	if(sGender == "m")
	{
		userInfo.Gender = GD_MALE;
	}
	else if(sGender == "f")
	{
		userInfo.Gender = GD_FEMALE;
	}
	else
	{
		userInfo.Gender = GD_UNKNOW;
	}
	Json::GetString(jsonUserInfo, "profile_image_url", userInfo.FigureUrl);
	Json::GetString(jsonUserInfo, "province", userInfo.Province);
	Json::GetString(jsonUserInfo, "city", userInfo.City);

	return 0;
}

int CSinaWbPlatform::GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey)
{
	//get friends
	string url;
	string response;
	String::Format(url, URL_SINAWB_GET_FRIENDS, openid.c_str(), openkey.c_str());
	if(!Network::HttpGetRequest(response, url))
	{
		m_errorMessage = response;
		error_log("[http request fail][reason=HttpGetRequest,url=getfriends,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}
	Json::Value jsonFriends;
	if(!Json::FromString(jsonFriends, response))
	{
		m_errorMessage = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,url=getfriends,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}
	if(Json::GetString(jsonFriends, "error", m_errorMessage))
	{
		error_log("[data error][reason=QueryFail,url=getfriends,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}
	if(!Json::IsArray(jsonFriends, "ids"))
	{
		m_errorMessage = "query_fail";
		error_log("[data error][reason=QueryFail,url=getfriends,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}

	//get followers
	String::Format(url, URL_SINAWB_GET_FOLLOWERS, openid.c_str(), openkey.c_str());
	if(!Network::HttpGetRequest(response, url))
	{
		m_errorMessage = response;
		error_log("[http request fail][reason=HttpGetRequest,url=getfollowers,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}
	Json::Value jsonFollowers;
	if(!Json::FromString(jsonFollowers, response))
	{
		m_errorMessage = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,url=getfollowers,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}
	if(Json::GetString(jsonFollowers, "error", m_errorMessage))
	{
		error_log("[data error][reason=QueryFail,url=getfollowers,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}
	if(!Json::IsArray(jsonFollowers, "ids"))
	{
		m_errorMessage = "query_fail";
		error_log("[data error][reason=QueryFail,url=getfollowers,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}

	//merge friends
	friendList.clear();
	Json::Value &friends = jsonFriends["ids"];
	unsigned friendsCount = friends.size();
	set<string> idSet;
	for(unsigned i = 0; i < friendsCount; i++)
	{
		string id;
		if(Json::GetString(friends, i, id))
		{
			idSet.insert(id);
		}
	}
	Json::Value &followers = jsonFollowers["ids"];
	unsigned followersCount = followers.size();
	for(unsigned i = 0; i< followersCount; i++)
	{
		string id;
		if(Json::GetString(followers, i, id))
		{
			if(idSet.find(id) != idSet.end())
			{
				friendList.push_back(id);
			}
		}
	}

	return 0;
}

string CSinaWbPlatform::GetErrorMessage()
{
	return m_errorMessage;
}

