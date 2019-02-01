#include "VNPlatform.h"
#include <iostream>

CVNPlatform::CVNPlatform()
{
}

int CVNPlatform::Initialize(const string &appid, const string &appkey, map<string, string> &params)
{
	IOpenPlatform::Initialize(appid, appkey, params);
	info_log("[init ok][appid=%s,appkey=%s,v3domain=%s]",
			appid.c_str(), appkey.c_str(), params["v3domain"].c_str());
	return 0;
}

void CVNPlatform::SetParameter(const map<string, string> &params)
{
	m_pf.clear();
	m_userip.clear();
	map<string, string>::const_iterator it = params.find("pf");
	if (it != params.end())
		m_pf = it->second;
	it = params.find("userip");
	if (it != params.end())
		m_userip = it->second;
}

int CVNPlatform::GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey)
{
	m_errorMessage.clear();

	string url = "http://" + m_config["v3domain"] + "/user/get_info?";
	string osig = "GET&" + Crypt::UrlEncodeForTX("/user/get_info") + "&";


	string qsig = "openid=" + openid + "&openkey=" + openkey + "&userip=" + m_userip;
	string qstr = "openid="	+ Crypt::UrlEncodeForTX(openid) + "&openkey=" + Crypt::UrlEncodeForTX(openkey)
			 + "&userip=" + Crypt::UrlEncodeForTX(m_userip);
	osig += Crypt::UrlEncodeForTX(qsig);
	string key = m_appKey + "&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);
	url += qstr + "&sig=" + Crypt::UrlEncodeForTX(sig);
	string response;

	//error_log("[get_user_info]%s",url.c_str());
	if (!Network::HttpGetRequest(response, url) || response.empty())
	{
		m_errorMessage = response;
		error_log("[http request fail][openid=%s,openkey=%s,errmsg=%s]",
				openid.c_str(), openkey.c_str(), m_errorMessage.c_str());
		return -1;
	}
	//error_log("[get_user_info_response]%s",response.c_str());

	Json::Value value;
	if(!Json::Reader().parse(response, value))
	{
		m_errorMessage = "json_parse_fail";
		error_log("[json parse fail][openid=%s,openkey=%s,response=%s]",
				openid.c_str(), openkey.c_str(), response.c_str());
		return -1;
	}
	int ret;
	if (!Json::GetInt(value, "ret", ret) || ret != 0)
	{
		Json::GetString(value, "msg", m_errorMessage);
		error_log("[get_info fail][openid=%s,openkey=%s,msg=%s]",
				openid.c_str(), openkey.c_str(), m_errorMessage.c_str());
		return -1;
	}
	userInfo.Clear();
	userInfo.OpenId = openid;
	Json::GetString(value, "nickname", userInfo.Name);
	Json::GetString(value, "figureurl", userInfo.FigureUrl);
	if(userInfo.FigureUrl.find("http://") != 0)
		userInfo.FigureUrl = "http://" + userInfo.FigureUrl;
	string temp;
	Json::GetString(value, "gender", temp);
	if (temp == "male")
		userInfo.Gender = GD_MALE;
	else if (temp == "female")
		userInfo.Gender = GD_FEMALE;
	else
		userInfo.Gender = GD_UNKNOW;
	Json::GetString(value, "country", userInfo.Nation);
	Json::GetString(value, "province", userInfo.Province);
	Json::GetString(value, "city", userInfo.City);

	return 0;
}

int CVNPlatform::GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey)
{
	m_errorMessage.clear();

	string url = "http://" + m_config["v3domain"] + "/relation/get_app_friends?";
	string osig = "GET&" + Crypt::UrlEncodeForTX("/relation/get_app_friends") + "&";
	string qsig = "openid="	+ Crypt::UrlEncodeForTX(openid) + "&openkey=" + Crypt::UrlEncodeForTX(openkey)
			 + "&userip=" + Crypt::UrlEncodeForTX(m_userip);
	string qstr = "openid=" + openid + "&openkey=" + openkey + "&userip=" + m_userip;
	osig += Crypt::UrlEncodeForTX(qsig);
	string key = m_appKey + "&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);
	url += qstr + "&sig=" + Crypt::UrlEncodeForTX(sig);
	string response;

	//error_log("[get_friend_list]%s",url.c_str());
	if (!Network::HttpGetRequest(response, url) || response.empty())
	{
		m_errorMessage = response;
		error_log("[http request fail][openid=%s,openkey=%s,errmsg=%s]",
				openid.c_str(), openkey.c_str(), m_errorMessage.c_str());
		return -1;
	}
	//error_log("[get_friend_list_response]%s",response.c_str());

	Json::Value value;
	if(!Json::Reader().parse(response, value))
	{
		m_errorMessage = "json_parse_fail";
		error_log("[json parse fail][openid=%s,openkey=%s,response=%s]",
				openid.c_str(), openkey.c_str(), response.c_str());
		return -1;
	}
	int ret;
	if (!Json::GetInt(value, "ret", ret) || ret != 0)
	{
		Json::GetString(value, "msg", m_errorMessage);
		error_log("[get_info fail][openid=%s,openkey=%s,msg=%s]",
				openid.c_str(), openkey.c_str(), m_errorMessage.c_str());
		return -1;
	}
	if (Json::IsArray(value, "items"))
	{
		for (unsigned i = 0; i < value["items"].size(); i++)
		{
			string temp;
			if (Json::GetString(value["items"][i], "openid", temp))
				friendList.push_back(temp);
		}
	}

	return 0;
}
int CVNPlatform::Is_Login(const string &openid, const string &openkey,const string &pf)
{
	m_errorMessage.clear();
	string url = "http://" + m_config["v3domain"] + "/user/is_login?";
	string osig = "GET&" + Crypt::UrlEncodeForTX("/user/is_login") + "&";
	string qsig = "openid=" + openid + "&openkey=" + openkey + "&userip=" + m_userip;
	string qstr = "openid="	+ Crypt::UrlEncodeForTX(openid) + "&openkey=" + Crypt::UrlEncodeForTX(openkey)
			 + "&userip=" + Crypt::UrlEncodeForTX(m_userip);
	osig += Crypt::UrlEncodeForTX(qsig);
	string key = m_appKey + "&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);
	url += qstr + "&sig=" + Crypt::UrlEncodeForTX(sig);
	string response;

	//error_log("[is_login]%s",url.c_str());
	if (!Network::HttpGetRequest(response, url) || response.empty())
	{
		m_errorMessage = response;
		error_log("[http request fail][openid=%s,openkey=%s,errmsg=%s]",
				openid.c_str(), openkey.c_str(), m_errorMessage.c_str());
		error_log("responce = %s",response.c_str());
		error_log("url = %s, qsig=%s",url.c_str(),qsig.c_str());
		return -1;
	}
	//error_log("[is_login_response]%s",response.c_str());

	Json::Value value;
	if(!Json::Reader().parse(response, value))
	{
		m_errorMessage = "json_parse_fail";
		error_log("[json parse fail][openid=%s,openkey=%s,response=%s]",
				openid.c_str(), openkey.c_str(), response.c_str());
		error_log("responce = %s",response.c_str());
		error_log("url = %s, qsig=%s",url.c_str(),qsig.c_str());
		return -1;
	}
	int ret;
	if (!Json::GetInt(value, "ret", ret) || ret != 0)
	{
		Json::GetString(value, "msg", m_errorMessage);
		error_log("[get_info fail][openid=%s,openkey=%s,msg=%s]",
				openid.c_str(), openkey.c_str(), m_errorMessage.c_str());
		error_log("responce = %s",response.c_str());
		error_log("url = %s, qsig=%s",url.c_str(),qsig.c_str());
		return -1;
	}
	error_log("responce = %s",response.c_str());
	return 0;
}

string CVNPlatform::GetErrorMessage()
{
	return m_errorMessage;
}
