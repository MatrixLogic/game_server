/*
 * MobagePlatform.cpp
 *
 *  Created on: 2011-11-22
 *      Author: dada
 */

#include "MobagePlatform.h"
#include "OAuthUtil.h"

#define MOBAGE_BASE_API "mobage_base_api"
//#define MOBAGE_BASE_URL "http://app.sb.mbga-platform.jp/social/api/restful/v2"

int CMobagePlatform::GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey)
{
	/*vector<string> items;
	CBasic::StringSplit(openkey, "|", items);
	if(items.size() != 3)
	{
		m_errMsg = "data_error";
		error_log("[data error][openid=%s,openkey=%s]", openid.c_str(), openkey.c_str());
		return -1;
	}
	userInfo.OpenId = openid;
	userInfo.Name = Crypt::UrlDecode(items[0]);
	userInfo.Gender = GD_UNKNOW;
	userInfo.FigureUrl = Crypt::UrlDecode(items[1]);*/
	string response;
	map<string, string> params;
	params["fields"] = "id,nickname,gender,thumbnailUrl";
	OAuth::OAuthParam oauthParam;
	oauthParam.AppKey = GetAppId();
	oauthParam.AppSecret = GetAppKey();
	oauthParam.ParamTrans = OAuth::AuthorizationHeader;
	oauthParam.Realm = "";
	oauthParam.SignMethod = OAuth::HMAC_SHA1;
	oauthParam.Token = "";
	oauthParam.TokenSecret = "";

	string url = GetConfig(MOBAGE_BASE_API);
	url.append("/people/").append(openid).append("/@self");
	if (!OAuth::HttpRequest(response, OAuth::GET, url, params, oauthParam))
	{
		error_log("[HttpRequest error][openid=%s]", openid.c_str());
		return -1;
	}
	Json::Value json;
	Json::Reader reader;
	if (!reader.parse(response, json) || json.isMember("Error"))
	{
		error_log("[HttpRequest error][openid=%s,res=%s]", openid.c_str(),response.c_str());
		return -1;
	}
	userInfo.OpenId = openid;
	userInfo.Name = json["nickname"].asString();
	string gender = json["gender"].asString();
	if (gender == "male")
		userInfo.Gender = GD_MALE;
	else if (gender == "female")
		userInfo.Gender = GD_FEMALE;
	else
		userInfo.Gender = GD_UNKNOW;
	userInfo.FigureUrl = json["thumbnailUrl"].asString();
	return 0;
}

int CMobagePlatform::GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey)
{
	/*vector<string> items;
	CBasic::StringSplit(openkey, "|", items);
	if(items.size() != 3)
	{
		m_errMsg = "data_error";
		error_log("[data error][openid=%s,openkey=%s]", openid.c_str(), openkey.c_str());
		return -1;
	}
	vector<string> friends;
	CBasic::StringSplitTrim(items[2], ",", friends);
	friendList.clear();
	for(vector<string>::iterator itr = friends.begin(); itr != friends.end(); itr++)
	{
		if(!itr->empty())
		{
			friendList.push_back(Crypt::UrlDecode(*itr));
		}
	}*/
	string response;
	map<string, string> params;
	params["fields"] = "id,hasApp";
	params["count"] = "500";
	OAuth::OAuthParam oauthParam;
	oauthParam.AppKey = GetAppId();
	oauthParam.AppSecret = GetAppKey();
	oauthParam.ParamTrans = OAuth::AuthorizationHeader;
	oauthParam.Realm = "";
	oauthParam.SignMethod = OAuth::HMAC_SHA1;
	oauthParam.Token = "";
	oauthParam.TokenSecret = "";

	string url = GetConfig(MOBAGE_BASE_API);
	url.append("/people/").append(openid).append("/@friends");
	//info_log("[mobage][openid=%s,url=%s]",openid.c_str(),url.c_str());
	if (!OAuth::HttpRequest(response, OAuth::GET, url, params, oauthParam))
	{
		error_log("[HttpRequest error][openid=%s]", openid.c_str());
		return -1;
	}
	Json::Value json;
	Json::Reader reader;
	if (!reader.parse(response, json) || json.isMember("Error"))
	{
		error_log("[HttpRequest error][openid=%s,res=%s]", openid.c_str(),response.c_str());
		return -1;
	}
	if (Json::IsArray(json, "entry"))
	{
		Json::Value &entry = json["entry"];
		for (unsigned i = 0; i < entry.size(); i++)
		{
			if (entry[i]["hasApp"].asBool())
			{
				string fopenid = entry[i]["id"].asString();
				size_t pos = fopenid.find(":");
				if (pos != string::npos)
					fopenid = fopenid.substr(pos+1);
				if (!fopenid.empty() && fopenid != openid)
					friendList.push_back(fopenid);
			}
		}
	}

	return 0;
}

int CMobagePlatform::GetBlackList(OPFriendList &blackList, const string &openid, const string &openkey)
{
	///TODO: 未完成，请不要使用
	return -1;
	const string rest_base_uri = "http://app.sb.mbga-platform.jp/social/api/restful/v2";
	string url = rest_base_uri + "/blacklist/" + openid + "/@all";

	string response;
	if(!Network::HttpGetRequest(response, url))
	{
		m_errMsg = "request fail";
		error_log("[http request fail][openid=%s]", openid.c_str());
		return -1;
	}
	Json::Value value;
	if(!Json::Reader().parse(response, value))
	{
		m_errMsg = "json_parse_fail";
		error_log("[json parse fail][openid=%s,response=%s]",
				openid.c_str(), response.c_str());
		return -1;
	}
	if (value.isObject() && Json::IsArray(value, "entry") && value["entry"].size() > 0)
	{
		for (unsigned i = 0; i < value["entry"].size(); i++)
		{
			string id;
			string targetId;
			Json::GetString(value["entry"][i], "id", id);
			Json::GetString(value["entry"][i], "targetId", targetId);
			size_t found1 = id.find(':');
			size_t found2 = targetId.find(':');
			if (found1 == string::npos || found2 == string::npos)
				continue;
			id = id.substr(found1 + 1);
			targetId = targetId.substr(found2 + 1);
			if (id != openid)
				continue;
			blackList.push_back(targetId);
		}
	}
	return 0;
}
