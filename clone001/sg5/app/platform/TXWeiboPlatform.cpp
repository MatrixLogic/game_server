/*
 * TXWeiboPlatform.cpp
 *
 *  Created on: 2011-10-14
 *      Author: dada
 */

#include "TXWeiboPlatform.h"
#include "OAuthUtil.h"

#define TXWB_REQUEST_TOKEN_URL "https://open.t.qq.com/cgi-bin/request_token"
#define TXWB_AUTHORIZE_URL "https://open.t.qq.com/cgi-bin/authorize"
#define TXWB_ACCESS_TOKEN_URL "https://open.t.qq.com/cgi-bin/access_token"
#define TXWB_USER_INFO_URL "http://open.t.qq.com/api/user/info"
#define TXWB_APP_FRIEND_URL "http://open.t.qq.com/api/friends/mutual_list"

int CTXWeiboPlatform::GetAuthorizeUrl(string &url)
{
	//request
	map<string, string> params;
	params["oauth_callback"] = OAuth::Encode(GetConfig("oauth_callback"));
	OAuth::OAuthParam oauthParam;
	oauthParam.AppKey = GetAppId();
	oauthParam.AppSecret = GetAppKey();
	oauthParam.SignMethod = OAuth::HMAC_SHA1;
	oauthParam.ParamTrans = OAuth::RequestURIQuery;
	string response;
	if(!OAuth::HttpRequest(response, OAuth::GET, TXWB_REQUEST_TOKEN_URL, params, oauthParam))
	{
		m_errMsg = "query_fail";
		error_log("[request_token_fail][platform=TXWEIBO,errmsg=%s,response=%s]", m_errMsg.c_str(), response.c_str());
		return -1;
	}

	//parse response
	string token;
	string tokenSecret;
	String::FindMatch(token, response, "oauth_token=", "&", true);
	String::FindMatch(tokenSecret, response, "oauth_token_secret=", "&", true);
	if(token.empty() || tokenSecret.empty())
	{
		m_errMsg = "response_error";
		error_log("[response_token_error][platform=TXWEIBO,errmsg=%s,response=%s]", m_errMsg.c_str(), response.c_str());
		return -1;
	}
	token = Crypt::UrlDecode(token);
	tokenSecret = Crypt::UrlEncode(tokenSecret);

	//save token
	if(!OAuth::SaveToken(token, tokenSecret))
	{
		m_errMsg = "save_token_fail";
		error_log("[save_token_fail][platform=TXWEIBO,errmsg=%s,token=%s,token_secret=%s]",
				m_errMsg.c_str(), token.c_str(), tokenSecret.c_str());
		return -1;
	}

	//get url
	String::Format(url, "%s?oauth_token=%s", TXWB_AUTHORIZE_URL, token.c_str());
	return  0;
}

int CTXWeiboPlatform::GetOpenKey(const string &token, const string &verifier, string &openid, string &openkey)
{
	//get token secret
	string tokenSecret;
	if(!OAuth::GetTokenSecret(token, tokenSecret))
	{
		m_errMsg = "get_token_fail";
		error_log("[get_token_fail][platform=TXWEIBO,errmsg=%s,token=%s]", m_errMsg.c_str(), token.c_str());
		return -1;
	}

	//request
	map<string, string> params;
	params["oauth_verifier"] = OAuth::Encode(verifier);
	OAuth::OAuthParam oauthParam;
	oauthParam.AppKey = GetAppId();
	oauthParam.AppSecret = GetAppKey();
	oauthParam.Token = token;
	oauthParam.TokenSecret = tokenSecret;
	oauthParam.SignMethod = OAuth::HMAC_SHA1;
	oauthParam.ParamTrans = OAuth::RequestURIQuery;
	string response;
	if(!OAuth::HttpRequest(response, OAuth::GET, TXWB_ACCESS_TOKEN_URL, params, oauthParam))
	{
		m_errMsg = "query_fail";
		error_log("[request_access_token_fail][platform=TXWEIBO,errmsg=%s,response=%s]",
				m_errMsg.c_str(), response.c_str());
		return -1;
	}

	//parse response
	string accessToken;
	string accessTokenSecret;
	String::FindMatch(accessToken, response, "oauth_token=", "&", true);
	String::FindMatch(accessTokenSecret, response, "oauth_token_secret=", "&", true);
	String::FindMatch(openid, response, "name=", "&", true);
	if(accessToken.empty() || accessTokenSecret.empty() || openid.empty())
	{
		m_errMsg = "response_error";
		error_log("[response_access_token_error][platform=TXWEIBO,errmsg=%s,response=%s]", m_errMsg.c_str(), response.c_str());
		return -1;
	}
	accessToken = Crypt::UrlDecode(accessToken);
	accessTokenSecret = Crypt::UrlEncode(accessTokenSecret);
	openid = Crypt::UrlEncode(openid);

	//gen openkey
	MakeOpenKey(openkey, openid, accessToken, accessTokenSecret);
	return 0;
}

int CTXWeiboPlatform::GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey)
{
	//get token secret
	string token;
	string tokenSecret;
	if(!ParseOpenKey(openkey, openid, token, tokenSecret))
	{
		return -1;
	}

	//request
	map<string, string> params;
	params["format"] = "json";
	OAuth::OAuthParam oauthParam;
	oauthParam.AppKey = GetAppId();
	oauthParam.AppSecret = GetAppKey();
	oauthParam.Token = token;
	oauthParam.TokenSecret = tokenSecret;
	oauthParam.SignMethod = OAuth::HMAC_SHA1;
	oauthParam.ParamTrans = OAuth::RequestURIQuery;
	string response;
	if(!OAuth::HttpRequest(response, OAuth::GET, TXWB_USER_INFO_URL, params, oauthParam))
	{
		m_errMsg = "query_fail";
		error_log("[get_user_info_fail][platform=TXWEIBO,openid=%s,openkey=%s,errmsg=%s,response=%s]",
				openid.c_str(), openkey.c_str(), m_errMsg.c_str(), response.c_str());
		return -1;
	}

	//parse response
	Json::Value jsonUserInfo;
	if(!Json::FromString(jsonUserInfo, response))
	{
		m_errMsg = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,openid=%s,openkey=%s,errmsg=%s,response=%s]",
				openid.c_str(), openkey.c_str(), m_errMsg.c_str(), response.c_str());
		return -1;
	}
	int ret = 0;
	Json::GetInt(jsonUserInfo, "ret", ret);
	if(ret != 0)
	{
		Json::GetString(jsonUserInfo, "msg", m_errMsg);
		error_log("[response error][reason=QueryFail,openid=%s,openkey=%s,ret=%d,errmsg=%s]",
				openid.c_str(), openkey.c_str(), ret, m_errMsg.c_str());
		return -1;
	}
	if(!Json::IsObject(jsonUserInfo, "data"))
	{
		m_errMsg = "data_error";
		error_log("[data error][reason=QueryFail,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errMsg.c_str(), response.c_str());
		return -1;
	}
	Json::Value &data = jsonUserInfo["data"];
	string name;
	Json::GetString(data, "name", name);
	if(name.empty() || name != openid)
	{
		m_errMsg = "name_error";
		error_log("[data error][reason=QueryFail,openid=%s,name=%s,errmsg=%s]",
				openid.c_str(), name.c_str(), m_errMsg.c_str());
		return -1;
	}
	userInfo.OpenId = name;
	Json::GetString(data, "nick", userInfo.Name);
	if(Json::GetInt(data, "sex", userInfo.Gender))
	{
		if(userInfo.Gender == 1)
		{
			userInfo.Gender = GD_MALE;
		}
		else if(userInfo.Gender == 2)
		{
			userInfo.Gender = GD_FEMALE;
		}
		else
		{
			userInfo.Gender = GD_UNKNOW;
		}
	}
	Json::GetString(data, "head", userInfo.FigureUrl);
	Json::GetString(data, "country_code", userInfo.Nation);
	Json::GetString(data, "province_code", userInfo.Province);
	Json::GetString(data, "city_code", userInfo.City);

	return 0;
}

int CTXWeiboPlatform::GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey)
{
	return 0;
}

bool CTXWeiboPlatform::MakeOpenKey(string &openkey, const string &openid, const string &token, const string &tokenSecret)
{
	openkey = token + "|" + tokenSecret;
	return true;
}

bool CTXWeiboPlatform::ParseOpenKey(const string &openkey, const string &openid, string &token, string &tokenSecret)
{
	string::size_type mid;
	mid = openkey.find("|");
	if(mid == string::npos || mid <= 0 || mid >= openkey.size() - 1)
	{
		m_errMsg = "openkey_error";
		error_log("[openkey_error][platform=TXWEIBO,errmsg=%s,openid=%s,openkey=%s]",
				m_errMsg.c_str(), openid.c_str(), openkey.c_str());
		return false;
	}
	token = openkey.substr(0, mid);
	tokenSecret = openkey.substr(mid + 1);
	return true;
}

