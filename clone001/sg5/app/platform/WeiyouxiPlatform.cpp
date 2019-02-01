/*
 * WeiyouxiPlatform.cpp
 *
 *  Created on: 2011-5-16
 *      Author: dada
 */

#include "WeiyouxiPlatform.h"
#include "Common.h"

#define WEIYOUXI_GET_USER_INFO_URL "http://api.weibo.com/game/1/user/show.json?%s&signature=%s"
#define WEIYOUXI_GET_USER_INFO_PARAM "session_key=%s&source=%s&timestamp=%.3lf"
#define WEIYOUXI_GET_APP_FRIEND_URL "http://api.weibo.com/game/1/user/app_friend_ids.json?%s&signature=%s"
#define WEIYOUXI_GET_APP_FRIEND_PARAM "session_key=%s&source=%s&timestamp=%.3lf"

static double GetWyxTimestamp()
{
	unsigned s;
	s = time(NULL);
	unsigned ms;
	mtime(ms);
	return (double)s + (double)(ms % 1000) / 1000.0;
}

int CWeiyouxiPlatform::GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey)
{
	//gen url
	string param;
	String::Format(param, WEIYOUXI_GET_USER_INFO_PARAM, openkey.c_str(), GetAppId().c_str(), GetWyxTimestamp());
	string key = Crypt::Sha1(param + GetAppKey());
	string url;
	String::Format(url, WEIYOUXI_GET_USER_INFO_URL, param.c_str(), key.c_str());

	//query
	string response;
	if(!Network::HttpGetRequest(response, url))
	{
		m_errorMessage = response;
		error_log("[http request fail][reason=HttpGetRequest,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}

	//check return
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

	//set user info
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
	Json::GetString(jsonUserInfo, "avatar_large", userInfo.FigureUrl);
	Json::GetString(jsonUserInfo, "province", userInfo.Province);
	Json::GetString(jsonUserInfo, "city", userInfo.City);

	//vip_level: 1-个人认证 5-机构认证 7-达人
	userInfo.ExtraIntInfo["type"] = 0;
	if(jsonUserInfo.isMember("verified"))
	{
		if(jsonUserInfo["verified"].isBool())
		{
			if(jsonUserInfo["verified"].asBool())
			{
				userInfo.ExtraIntInfo["type"] = 1;
			}
		}
	}
	int verifiedType = 0;
	Json::GetInt(jsonUserInfo, "verified_type", verifiedType);
	if(verifiedType > 0 && verifiedType < 10)
	{
		userInfo.ExtraIntInfo["type"] = 5;
	}
	else if(verifiedType == 200)
	{
		userInfo.ExtraIntInfo["type"] = 7;
	}
	return 0;
}

int CWeiyouxiPlatform::GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey)
{
	//gen url
	string param;
	String::Format(param, WEIYOUXI_GET_APP_FRIEND_PARAM, openkey.c_str(), GetAppId().c_str(), GetWyxTimestamp());
	string key = Crypt::Sha1(param + GetAppKey());
	string url;
	String::Format(url, WEIYOUXI_GET_APP_FRIEND_URL, param.c_str(), key.c_str());

	//query
	string response;
	if(!Network::HttpGetRequest(response, url))
	{
		m_errorMessage = response;
		error_log("[http request fail][reason=HttpGetRequest,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}

	//check return
	Json::Value jsonFriends;
	if(!Json::FromString(jsonFriends, response))
	{
		m_errorMessage = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}
	if(!jsonFriends.isArray())
	{
		if(jsonFriends.isObject())
		{
			if(Json::GetString(jsonFriends, "error", m_errorMessage))
			{
				error_log("[data error][reason=QueryFail,openid=%s,errmsg=%s]",
						openid.c_str(), m_errorMessage.c_str());
				return -1;
			}
		}
		m_errorMessage = "data_error";
		error_log("[data error][reason=DataFormatError,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}

	//set friends
	friendList.clear();
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

#define WEIYOUXI_IS_FUN_URL "http://api.weibo.com/game/1/application/is_fan.json?%s&signature=%s"
#define WEIYOUXI_IS_FUN_PARAM "session_key=%s&source=%s&timestamp=%.3lf"
#define WEIYOUXI_SCORED_URL "http://api.weibo.com/game/1/application/scored.json?%s&signature=%s"
#define WEIYOUXI_SCORED_PARAM "session_key=%s&source=%s&timestamp=%.3lf"
#define WEIYOUXI_IGNORE_INVITE_URL "http://api.weibo.com/game/1/invite/ignore_game_all.json?%s&signature=%s"
#define WEIYOUXI_IGNORE_INVITE_PARAM "session_key=%s&source=%s&timestamp=%.3lf"
#define WEIYOUXI_SET_ACHIEVEMENT_URL "http://api.weibo.com/game/1/achievements/set.json"
#define WEIYOUXI_SET_ACHIEVEMENT_PARAM "achv_id=%u&session_key=%s&source=%s&timestamp=%.3lf"
#define WEIYOUXI_SET_LEADERBOARD_URL "http://api.weibo.com/game/1/leaderboards/set.json"
#define WEIYOUXI_SET_LEADERBOARD_PARAM "rank_id=%u&session_key=%s&source=%s&timestamp=%.3lf&value=%u"

int CWeiyouxiPlatform::IsFan(const string &openid, const string &openkey, int &result)
{
	//gen url
	string param;
	String::Format(param, WEIYOUXI_IS_FUN_PARAM, openkey.c_str(), GetAppId().c_str(), GetWyxTimestamp());
	string key = Crypt::Sha1(param + GetAppKey());
	string url;
	String::Format(url, WEIYOUXI_IS_FUN_URL, param.c_str(), key.c_str());

	//query
	string response;
	if(!Network::HttpGetRequest(response, url))
	{
		m_errorMessage = response;
		error_log("[http request fail][reason=HttpGetRequest,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}

	//check return
	Json::Value jsonResult;
	if(!Json::FromString(jsonResult, response))
	{
		m_errorMessage = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}
	if(Json::GetString(jsonResult, "error", m_errorMessage))
	{
		error_log("[data error][reason=QueryFail,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}
	if(!Json::GetInt(jsonResult, "flag", result))
	{
		m_errorMessage = "query_fail";
		error_log("[data error][reason=QueryFail,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}
	return 0;
}

int CWeiyouxiPlatform::Scored(const string &openid, const string &openkey, int &result)
{
	//gen url
	string param;
	String::Format(param, WEIYOUXI_SCORED_PARAM, openkey.c_str(), GetAppId().c_str(), GetWyxTimestamp());
	string key = Crypt::Sha1(param + GetAppKey());
	string url;
	String::Format(url, WEIYOUXI_SCORED_URL, param.c_str(), key.c_str());

	//query
	string response;
	if(!Network::HttpGetRequest(response, url))
	{
		m_errorMessage = response;
		error_log("[http request fail][reason=HttpGetRequest,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}

	//check return
	Json::Value jsonResult;
	if(!Json::FromString(jsonResult, response))
	{
		m_errorMessage = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}
	if(Json::GetString(jsonResult, "error", m_errorMessage))
	{
		error_log("[data error][reason=QueryFail,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}
	if(!Json::GetInt(jsonResult, "flag", result))
	{
		m_errorMessage = "query_fail";
		error_log("[data error][reason=QueryFail,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}
	if(result == 0)
	{
		result = -1;
	}
	else
	{
		if(!Json::GetInt(jsonResult, "rate", result))
		{
			result = 5;
		}
	}
	return 0;
}

int CWeiyouxiPlatform::IgnoreInvite(const string &openid, const string &openkey)
{
	//gen url
	string param;
	String::Format(param, WEIYOUXI_IGNORE_INVITE_PARAM, openkey.c_str(), GetAppId().c_str(), GetWyxTimestamp());
	string key = Crypt::Sha1(param + GetAppKey());
	string url;
	String::Format(url, WEIYOUXI_IGNORE_INVITE_URL, param.c_str(), key.c_str());

	//query
	string response;
	if(!Network::HttpGetRequest(response, url))
	{
		m_errorMessage = response;
		error_log("[http request fail][reason=HttpGetRequest,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}

	//check return
	if(response.find("true") == string::npos)
	{
		m_errorMessage = "return fail";
		error_log("[json parse fail][reason=chcek_result,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}

	return 0;
}

int CWeiyouxiPlatform::SetAchievement(const string &openid, const string &openkey, unsigned id)
{
	//gen url
	string param;
	String::Format(param, WEIYOUXI_SET_ACHIEVEMENT_PARAM, id, openkey.c_str(), GetAppId().c_str(), GetWyxTimestamp());
	string key = Crypt::Sha1(param + GetAppKey());
	param = param + "&signature=" + key;

	//query
	string response;
	if(!Network::HttpPostRequest(response, WEIYOUXI_SET_ACHIEVEMENT_URL, param))
	{
		m_errorMessage = response;
		error_log("[http request fail][reason=HttpGetRequest,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}

	//check return
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

	return 0;
}

int CWeiyouxiPlatform::SetLeaderboard(const string &openid, const string &openkey, unsigned id, unsigned value)
{
	//gen url
	string param;
	String::Format(param, WEIYOUXI_SET_LEADERBOARD_PARAM, id, openkey.c_str(), GetAppId().c_str(), GetWyxTimestamp(), value);
	string key = Crypt::Sha1(param + GetAppKey());
	param = param + "&signature=" + key;

	//query
	string response;
	if(!Network::HttpPostRequest(response, WEIYOUXI_SET_LEADERBOARD_URL, param))
	{
		m_errorMessage = response;
		error_log("[http request fail][reason=HttpGetRequest,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}

	//check return
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

	return 0;
}

#define WEIYOUXI_GET_FRIEND_URL "http://api.weibo.com/game/1/user/friends.json?%s&signature=%s"
#define WEIYOUXI_GET_FRIEND_PARAM "count=200&session_key=%s&source=%s&timestamp=%.3lf&trim=1"

int CWeiyouxiPlatform::GetFriendList(vector<OPUserInfo> &friendList, const string &openid, const string &openkey)
{
	//gen url
	string param;
	String::Format(param, WEIYOUXI_GET_FRIEND_PARAM, openkey.c_str(), GetAppId().c_str(), GetWyxTimestamp());
	string key = Crypt::Sha1(param + GetAppKey());
	string url;
	String::Format(url, WEIYOUXI_GET_FRIEND_URL, param.c_str(), key.c_str());

	//query
	string response;
	if(!Network::HttpGetRequest(response, url))
	{
		m_errorMessage = response;
		error_log("[http request fail][reason=HttpGetRequest,openid=%s,errmsg=%s]",
				openid.c_str(), m_errorMessage.c_str());
		return -1;
	}

	//check return
	Json::Value data;
	if(!Json::FromString(data, response))
	{
		m_errorMessage = "json_parse_fail";
		error_log("[json parse fail][reason=JsonParse,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}
	if(!Json::IsArray(data, "users"))
	{
		if(Json::GetString(data, "error", m_errorMessage))
		{
			error_log("[data error][reason=QueryFail,openid=%s,errmsg=%s]",
					openid.c_str(), m_errorMessage.c_str());
			return -1;
		}
		m_errorMessage = "data_error";
		error_log("[data error][reason=DataFormatError,openid=%s,errmsg=%s,response=%s]",
				openid.c_str(), m_errorMessage.c_str(), response.c_str());
		return -1;
	}

	//set friends
	Json::Value &jsonFriends = data["users"];
	friendList.clear();
	unsigned friendsCount = jsonFriends.size();
	for(unsigned i = 0; i < friendsCount; i++)
	{
		if(jsonFriends[i].isObject())
		{
			Json::Value &friendData = jsonFriends[i];
			OPUserInfo user;
			if(Json::GetString(friendData, "id", user.OpenId) &&
				Json::GetString(friendData, "name", user.Name))
			{
				Json::GetString(friendData, "head_image_url", user.FigureUrl);
				//Json::GetString(friendData, "province", user.Province);
				//Json::GetString(friendData, "city", user.City);
				friendList.push_back(user);
			}
		}
	}

	return 0;
}

