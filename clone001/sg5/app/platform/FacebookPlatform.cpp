/*
 * CFacebookPlatform.cpp
 *
 *  Created on: 2011-5-16
 *      Author: dada
 */

#include "FacebookPlatform.h"
#include "Common.h"

#define URL_FB_FIGURE "http://graph.facebook.com/%s/picture"
#define URL_FB_GET_USER_INFO "https://graph.facebook.com/me?access_token=%s"
#define URL_FB_GET_OTHER_USER_INFO "https://graph.facebook.com/%s?access_token=%s"
#define URL_FB_FQL_QUERY "https://api.facebook.com/method/fql.query?query=%s&access_token=%s"
#define FQL_GET_APP_FRIENDS "SELECT+uid+FROM+user+WHERE+uid+IN+(SELECT+uid2+FROM+friend+WHERE+uid1=me())+AND+is_app_user"


CFacebookPlatform::CFacebookPlatform()
{
}

CFacebookPlatform::~CFacebookPlatform()
{
}

int CFacebookPlatform::Initialize(const string &appid, const string &appkey, map<string, string> &params)
{
	IOpenPlatform::Initialize(appid, appkey, params);
	return 0;
}

int CFacebookPlatform::GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey)
{
	string url;
	string response;
	String::Format(url, URL_FB_GET_USER_INFO, openkey.c_str());
	if(!Network::HttpGetRequest(response, url))
	{
		m_errorMessage = response;
		error_log("[http request fail][reason=HttpGetRequest,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}
	Json::Value jsonUserInfo;
	Json::Reader reader;
	if(!reader.parse(response, jsonUserInfo))
	{
		m_errorMessage = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
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
	userInfo.OpenId = jsonUserInfo["id"].asString();
	userInfo.Name = jsonUserInfo["name"].asString();
	string sGender = jsonUserInfo["gender"].asString();
	if(sGender == "male")
	{
		userInfo.Gender = GD_MALE;
	}
	else if(sGender == "female")
	{
		userInfo.Gender = GD_FEMALE;
	}
	else
	{
		userInfo.Gender = GD_UNKNOW;
	}
	String::Format(userInfo.FigureUrl, URL_FB_FIGURE, userInfo.OpenId.c_str());
	string locale = jsonUserInfo["locale"].asString();
	userInfo.Nation = locale;
	if(locale.size() == 5)
	{
		if(locale[2] == '_')
		{
			userInfo.Nation = locale.substr(3, 2);
		}
	}

	return 0;
}

int CFacebookPlatform::GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey)
{
	//string fql;
	//String::Format(fql, FQL_GET_APP_FRIENDS, openid.c_str());
	string url;
	String::Format(url, URL_FB_FQL_QUERY, FQL_GET_APP_FRIENDS, openkey.c_str());
	string response;
	if(!Network::HttpGetRequest(response, url))
	{
		m_errorMessage = response;
		error_log("[http request fail][reason=HttpGetRequest,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}

	bool parseSuccess = false;
	CMarkupSTL xmlResult;
	if(xmlResult.SetDoc(response.c_str()))
	{
		if(xmlResult.FindElem("fql_query_response"))
		{
			xmlResult.IntoElem();
			parseSuccess = true;
		}
	}
	if(!parseSuccess)
	{
		m_errorMessage = "json_parse_fail";
		error_log("[json parse fail][reason=XmlParse,openid=%s,response=%s]",
				openid.c_str(), response.c_str());
		return -1;
	}

	friendList.clear();
	while(xmlResult.FindElem("user"))
	{
		xmlResult.IntoElem();
		string uid = xmlResult.FindGetData("uid");
		if(!uid.empty())
		{
			friendList.push_back(uid);
		}
		xmlResult.OutOfElem();
	}

	return 0;
}

int CFacebookPlatform::GetOtherUserInfo(OPUserInfo userInfo, const string &queryOpenId, const string &openid, const string &openkey)
{
	string url;
	string response;
	String::Format(url, URL_FB_GET_OTHER_USER_INFO, queryOpenId.c_str(), openkey.c_str());
	if(!Network::HttpGetRequest(response, url))
	{
		m_errorMessage = response;
		error_log("[http request fail][reason=HttpGetRequest,openid=%s,errmsg=%s]",
				queryOpenId.c_str(), m_errorMessage.c_str());
		return -1;
	}
	Json::Value jsonUserInfo;
	Json::Reader reader;
	if(!reader.parse(response, jsonUserInfo))
	{
		m_errorMessage = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,openid=%s,errmsg=%s,response=%s]",
				queryOpenId.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}
	if(!jsonUserInfo.isMember("id"))
	{
		m_errorMessage = "query_fail";
		error_log("[data error][reason=QueryFail,openid=%s,errmsg=%s,response=%s]",
				queryOpenId.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}

	userInfo.Clear();
	userInfo.OpenId = jsonUserInfo["id"].asString();
	userInfo.Name = jsonUserInfo["name"].asString();
	string sGender = jsonUserInfo["gender"].asString();
	if(sGender == "male")
	{
		userInfo.Gender = GD_MALE;
	}
	else if(sGender == "female")
	{
		userInfo.Gender = GD_FEMALE;
	}
	else
	{
		userInfo.Gender = GD_UNKNOW;
	}
	String::Format(userInfo.FigureUrl, URL_FB_FIGURE, userInfo.OpenId.c_str());
	string locale = jsonUserInfo["locale"].asString();
	userInfo.Nation = locale;
	if(locale.size() == 5)
	{
		if(locale[2] == '_')
		{
			userInfo.Nation = locale.substr(3, 2);
		}
	}

	return 0;
}

int CFacebookPlatform::GetLikes(vector<string> &appIds, const string &openid, const string &openkey)
{
	string url;
	string response;
	String::Format(url, "https://graph.facebook.com/me/likes?access_token=%s", openkey.c_str());
	if(!Network::HttpGetRequest(response, url))
	{
		m_errorMessage = response;
		error_log("[http request fail][reason=HttpGetRequest,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}
	Json::Value likes;
	Json::Reader reader;
	if(!reader.parse(response, likes))
	{
		m_errorMessage = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}

	appIds.clear();
	size_t size = likes["data"].size();
	for (size_t i = 0; i < size; i++)
	{
		appIds.push_back(likes["data"][i]["id"].asString());
	}

	return 0;
}

int CFacebookPlatform::SendFeed(const string &openid, const string &openkey, const FacebookFeed &feed)
{
	string url;
	string response;
	String::Format(url, "https://graph.facebook.com/%s/feed?access_token=%s",
			feed.to.empty() ? "me" : feed.to.c_str(), openkey.c_str());
	string params;
	String::Format(
		params,
		"message=%s&picture=%s&link=%s&name=%s&caption=%s&description=%s&source=%s&actions=%s",
		Crypt::UrlEncode(feed.message).c_str(),
		Crypt::UrlEncode(feed.picture).c_str(),
		Crypt::UrlEncode(feed.link).c_str(),
		Crypt::UrlEncode(feed.name).c_str(),
		Crypt::UrlEncode(feed.caption).c_str(),
		Crypt::UrlEncode(feed.description).c_str(),
		Crypt::UrlEncode(feed.source).c_str(),
		Crypt::UrlEncode(feed.actions).c_str()
		);
	if(!Network::HttpPostRequest(response, url, params))
	{
		m_errorMessage = response;
		error_log("[http request fail][reason=HttpPostRequest,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}

	Json::Value value;
	if(!Json::FromString(value, response))
	{
		m_errorMessage = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}
	string id;
	if(!Json::GetString(value, "id", id))
	{
		m_errorMessage = response;
		error_log("[data error][reason=QueryFail,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}
	return 0;
}

string CFacebookPlatform::GetErrorMessage()
{
	return m_errorMessage;
}

