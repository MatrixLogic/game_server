/*
 * RenrenPlatform.cpp
 *
 *  Created on: 2011-7-4
 *      Author: dada
 */

#include "RenrenPlatform.h"

CRenrenPlatform::CRenrenPlatform()
{
}

CRenrenPlatform::~CRenrenPlatform()
{
}

int CRenrenPlatform::Initialize(const string &appid, const string &appkey, map<string, string> &params)
{
	IOpenPlatform::Initialize(appid, appkey, params);
	m_apiUrl = params["apiurl"];
	if(m_apiUrl.empty())
	{
		m_errorMessage = "config_error";
		error_log("[renren_config_error][apiurl=%s]", m_apiUrl.c_str());
		return -1;
	}
	return 0;
}

int CRenrenPlatform::GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey)
{
	//get url
	unsigned t;
	mtime(t);
	string params;
	String::Format(params, "api_key=%scall_id=%uformat=JSONmethod=users.getInfosession_key=%sv=1.0%s",
			m_appId.c_str(), t, openkey.c_str(), m_appKey.c_str());
	string sig = Crypt::Md5Encode(params);
	string data;
	String::Format(data, "api_key=%s&call_id=%u&format=JSON&method=users.getInfo&session_key=%s&sig=%s&v=1.0",
			m_appId.c_str(), t, openkey.c_str(), sig.c_str());

	//request
	string response;
	if(!Network::HttpPostRequest(response, m_apiUrl, data))
	{
		m_errorMessage = response;
		error_log("[http request fail][openid=%s,openkey=%s,errmsg=%s]",
				openid.c_str(), openkey.c_str(), m_errorMessage.c_str());
		return -1;
	}
	Json::Value value;
	if(!Json::Reader().parse(response, value))
	{
		m_errorMessage = "json_parse_fail";
		error_log("[json parse fail][openid=%s,openkey=%s,response=%s]",
				openid.c_str(), openkey.c_str(), response.c_str());
		return -1;
	}

	//get user info
	if(!value.isArray() || value.size() <= 0)
	{
		m_errorMessage = "json_data_error";
		error_log("[json data fail][openid=%s,openkey=%s,response=%s]",
				openid.c_str(), openkey.c_str(), response.c_str());
		return -1;
	}
	Json::Value &user = value[0u];
	if(!user["uid"].isIntegral())
	{
		m_errorMessage = "json_data_error";
		error_log("[json data fail][openid=%s,openkey=%s,response=%s]",
				openid.c_str(), openkey.c_str(), response.c_str());
	}
	userInfo.OpenId = Convert::UIntToString(user["uid"].asUInt());
	userInfo.Name = user["name"].asString();
	if(user["sex"].isIntegral())
	{
		userInfo.Gender = user["sex"].asInt();
	}
	else
	{
		userInfo.Gender = GD_UNKNOW;
	}
	userInfo.FigureUrl = user["tinyurl"].asString();
	if(user.isMember("hometown_location"))
	{
		Json::Value &hometown = user["hometown_location"];
		if(hometown.isObject())
		{
			userInfo.Nation = hometown["country"].asString();
			userInfo.Province = hometown["province"].asString();
			userInfo.City = hometown["city"].asString();
		}
	}
	return 0;
}

int CRenrenPlatform::GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey)
{
	//get url
	unsigned t;
	mtime(t);
	string params;
	String::Format(params, "api_key=%scall_id=%uformat=JSONmethod=friends.getAppFriendssession_key=%sv=1.0%s",
			m_appId.c_str(), t, openkey.c_str(), m_appKey.c_str());
	string sig = Crypt::Md5Encode(params);
	string data;
	String::Format(data, "api_key=%s&call_id=%u&format=JSON&method=friends.getAppFriends&session_key=%s&sig=%s&v=1.0",
			m_appId.c_str(), t, openkey.c_str(), sig.c_str());

	//request
	string response;
	if(!Network::HttpPostRequest(response, m_apiUrl, data))
	{
		m_errorMessage = response;
		error_log("[http request fail][openid=%s,openkey=%s,errmsg=%s]",
				openid.c_str(), openkey.c_str(), m_errorMessage.c_str());
		return -1;
	}
	Json::Value value;
	if(!Json::Reader().parse(response, value))
	{
		m_errorMessage = "json_parse_fail";
		error_log("[json parse fail][openid=%s,openkey=%s,response=%s]",
				openid.c_str(), openkey.c_str(), response.c_str());
		return -1;
	}

	//get friend
	if(!value.isArray())
	{
		m_errorMessage = "json_data_error";
		error_log("[json data fail][openid=%s,openkey=%s,response=%s]",
				openid.c_str(), openkey.c_str(), response.c_str());
		return -1;
	}
	friendList.clear();
	for(unsigned i = 0; i < value.size(); i++)
	{
		Json::Value &uid = value[i];
		if(!uid.isIntegral())
		{
			m_errorMessage = "json_data_error";
			error_log("[json data fail][openid=%s,openkey=%s,response=%s]",
					openid.c_str(), openkey.c_str(), response.c_str());
			return -1;
		}
		friendList.push_back(Convert::UIntToString(uid.asUInt()));
	}
	return 0;
}

string CRenrenPlatform::GetErrorMessage()
{
	return m_errorMessage;
}


