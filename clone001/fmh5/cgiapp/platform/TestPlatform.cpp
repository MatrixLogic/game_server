/*
 * TestPlatfrom.cpp
 *
 *  Created on: 2011-5-16
 *      Author: dada
 */

#include "TestPlatform.h"
#include "ErrorUtil.h"

#define KEY_USER_DATA "user_data"

int CTestPlatform::Initialize(const string &appid, const string &appkey, map<string, string> &params)
{
	IOpenPlatform::Initialize(appid, appkey, params);
	m_userData = params["user_data"];
	if(!File::IsExist(m_userData))
	{
		info_log("[user data no exist, create][file=%s]", m_userData.c_str());
		int result = File::Write(m_userData, "\n");
		if(result != 0)
		{
			error_log("[create user data fail][result=%d]", result);
			return result;
		}
	}
	return 0;
}

int CTestPlatform::GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey)
{
	userInfo.OpenId = openid;
	userInfo.Gender = GD_UNKNOW;

	string content;
	int result = File::Read(m_userData, content);
	if(result != 0)
	{
		error_log("[read user data fail][result=%d]", result);
		return result;
	}
	Json::Reader reader;
	Json::Value userlist;
	if (!reader.parse(content, userlist))
	{
		error_log("[parse user data fail]");
		return R_ERR_DB;
	}

	if(!openkey.empty())
	{
		if(!StringFilter::Check(openkey))
		{
			error_log("[StringFilter fail]");
			return R_ERR_DATA_LIMIT;
		}
		userInfo.Name = openkey;
		userlist[userInfo.OpenId] = openkey;
		result = File::Write(m_userData, Json::ToString(userlist));
		if (0 != result)
			error_log("[write user data fail]");
	}
	else if(userlist.isMember(openid))
		userInfo.Name = userlist[openid].asString();
	else
		userInfo.Name = openid;

	return 0;
}

int CTestPlatform::GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey)
{
	string content;
	int result = File::Read(m_userData, content);
	if(result != 0)
	{
		error_log("[read user data fail][result=%d]", result);
		return result;
	}
	OPFriendList allUserList;
	CBasic::StringSplitTrim(content, "\n", allUserList);
	int index = -1;
	for(unsigned i = 0; i < allUserList.size(); i++)
	{
		if(allUserList[i] == openid)
		{
			index = i;
			break;
		}
	}
	if(index < 0)
	{
		//error_log("[user data find fail][openid=%s]", openid.c_str());
		return 0;
	}
	int startIndex = index - 5;
	if(startIndex < 0)
	{
		startIndex = 0;
	}
	int endIndex = index + 5;
	if(endIndex > (int)allUserList.size())
	{
		endIndex = allUserList.size();
	}
	friendList.clear();
	for(int i = startIndex; i < endIndex; i++)
	{
		friendList.push_back(allUserList[i]);
	}
	return 0;
}

int CTestPlatform::Is_Login(const string &openid, const string &openkey)
{
	m_errMsg.clear();
	string url = "http://openapi.tencentyun.com" + m_config["v3domain"] + "/v3/user/is_login?";
	string osig = "GET&" + Crypt::UrlEncodeForTX("/v3/user/is_login") + "&";
//	string qsig = "appid=" + Crypt::UrlEncodeForTX(m_appId) + "&format=json&openid="
//			+ Crypt::UrlEncodeForTX(openid) + "&openkey=" + Crypt::UrlEncodeForTX(openkey)
//			+ "&pf=qzone&userip=" + Crypt::UrlEncodeForTX("218.17.160.248");
//	string qstr = "appid=" + m_appId + "&format=json&openid=" + openid + "&openkey=" + openkey
//			+ "&pf=qzone&userip=218.17.160.248";
	//string qstr =""
	string qsig = "appid=" + m_appId + "&format=json&openid=" + openid + "&openkey=" + openkey
				+ "&pf=qzone&userip=218.17.160.248";
	string qstr = "appid=" + Crypt::UrlEncodeForTX(m_appId) + "&format=json&openid="
				+ Crypt::UrlEncodeForTX(openid) + "&openkey=" + Crypt::UrlEncodeForTX(openkey)
				+ "&pf=" + Crypt::UrlEncodeForTX("qzone") + "&userip=" + Crypt::UrlEncodeForTX("218.17.160.248");

	osig += Crypt::UrlEncodeForTX(qsig);
	string key = "649b6c40d2f5eeb0d43d7657daa68c98&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);
	error_log("openid=%s,urlopenid=%s",openid.c_str(),Crypt::UrlEncodeForTX(openid).c_str());
	url += qstr + "&sig=" + Crypt::UrlEncodeForTX(sig);
	error_log("url = %s, qsig=%s",url.c_str(),qsig.c_str());
	string response;
	if (!Network::HttpGetRequest(response, url) || response.empty())
	{
		m_errMsg = response;
		error_log("[http request fail][openid=%s,openkey=%s,errmsg=%s]",
				openid.c_str(), openkey.c_str(), m_errMsg.c_str());
		error_log("responce = %s",response.c_str());
		return -1;
	}
	Json::Value value;
	if(!Json::Reader().parse(response, value))
	{
		m_errMsg = "json_parse_fail";
		error_log("[json parse fail][openid=%s,openkey=%s,response=%s]",
				openid.c_str(), openkey.c_str(), response.c_str());
		error_log("responce = %s",response.c_str());
		return -1;
	}
	int ret;
	if (!Json::GetInt(value, "ret", ret) || ret != 0)
	{
		Json::GetString(value, "msg", m_errMsg);
		error_log("[get_info fail][openid=%s,openkey=%s,msg=%s]",
				openid.c_str(), openkey.c_str(), m_errMsg.c_str());
		error_log("responce = %s",response.c_str());
		return -1;
	}
	error_log("responce = %s",response.c_str());
	return 0;
}

void CTestPlatform::ReplyCharge()
{
	CgiUtil::PrintText("{\"ret\":0,\"msg\":\"OK\"}");
}
