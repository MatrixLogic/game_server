#include "TencentPlatform.h"
#include <iostream>

CTencentPlatform::CTencentPlatform()
{
}

int CTencentPlatform::Initialize(const string &appid, const string &appkey, map<string, string> &params)
{
	IOpenPlatform::Initialize(appid, appkey, params);
	if(params["platform"] == "4")
	{
		SetPlatformType(PT_QZONE);
	}
	else if(params["platform"] == "11")
	{
		SetPlatformType(PT_TXWEIBO);
	}
	else if(params["platform"] == "15")
	{
		SetPlatformType(PT_3366);
	}
	else if(params["platform"] == "16")
	{
		SetPlatformType(PT_qqgame);
	}

	info_log("[init ok][appid=%s,appkey=%s,v3domain=%s]",
			appid.c_str(), appkey.c_str(), params["v3domain"].c_str());
	return 0;
}

void CTencentPlatform::SetParameter(const map<string, string> &params)
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

unsigned CTencentPlatform::GetPlatformByPF()
{
	if (m_pf == "qzone")
		return PT_QZONE;
	else if (m_pf == "weibo")
		return PT_TXWEIBO;
	else if (m_pf == "3366")
		return PT_3366;
	else if (m_pf == "qqgame")
		return PT_qqgame;
	else if (m_pf == "pengyou")
		return PT_PENGYOU;
	else
		return PT_UNKNOW;
}

int CTencentPlatform::GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey)
{
	m_errorMessage.clear();

	string url = "http://" + m_config["v3domain"] + "/v3/user/get_info?";
	string osig = "GET&" + Crypt::UrlEncodeForTX("/v3/user/get_info") + "&";


	string qsig = "appid=" + m_appId + "&format=json&openid=" + openid + "&openkey=" + openkey
			+ "&pf=" + m_pf + "&userip=" + m_userip;
	string qstr = "appid=" + Crypt::UrlEncodeForTX(m_appId) + "&format=json&openid="
			+ Crypt::UrlEncodeForTX(openid) + "&openkey=" + Crypt::UrlEncodeForTX(openkey)
			+ "&pf=" + Crypt::UrlEncodeForTX(m_pf) + "&userip=" + Crypt::UrlEncodeForTX(m_userip);
	osig += Crypt::UrlEncodeForTX(qsig);
	string key = m_appKey + "&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);
	url += qstr + "&sig=" + Crypt::UrlEncodeForTX(sig);
	string response;
	if (!Network::HttpGetRequest(response, url) || response.empty())
	{
		m_errorMessage = response;
		error_log("[http request fail][openid=%s,openkey=%s,errmsg=%s]",
				openid.c_str(), openkey.c_str(), m_errorMessage.c_str());
		return -1;
	}
	//info_log("[tx user info][url=%s][response=%s]", url.c_str(), response.c_str());
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
	if (temp == "男")
		userInfo.Gender = GD_MALE;
	else if (temp == "女")
		userInfo.Gender = GD_FEMALE;
	else
		userInfo.Gender = GD_UNKNOW;
	Json::GetString(value, "country", userInfo.Nation);
	Json::GetString(value, "province", userInfo.Province);
	Json::GetString(value, "city", userInfo.City);

	int is_yellow_vip = 0;
	int is_yellow_year_vip = 0;
	int yellow_vip_level = 0;

	Json::GetInt(value, "is_yellow_vip", is_yellow_vip);
	Json::GetInt(value, "is_yellow_year_vip", is_yellow_year_vip);
	Json::GetInt(value, "yellow_vip_level", yellow_vip_level);

	userInfo.ExtraIntInfo["bIsVip"] = is_yellow_vip == 1?1:0;
	userInfo.ExtraIntInfo["bIsYearVip"] = is_yellow_year_vip == 1?1:0;
	userInfo.ExtraIntInfo["nVipLevel"] = yellow_vip_level;

	int level_3366 = 1;
	string level_name_3366;
	int grow_level_3366 = 1;
	int grow_value_3366 = 1;

	Json::GetInt(value, "3366_level", level_3366);
	Json::GetString(value, "3366_level_name", level_name_3366);
	Json::GetInt(value, "3366_grow_level", grow_level_3366);
	Json::GetInt(value, "3366_grow_value", grow_value_3366);

	userInfo.ExtraIntInfo["3366_level"] = level_3366;
	userInfo.ExtraStrInfo["3366_level_name"] = level_name_3366;
	userInfo.ExtraIntInfo["3366_grow_level"] = grow_level_3366;
	userInfo.ExtraIntInfo["3366_grow_value"] = grow_value_3366;

	int blue_vip_level = 0;
	Json::GetInt(value, "blue_vip_level", blue_vip_level);
	userInfo.ExtraIntInfo["blue_vip_level"] = blue_vip_level;

	if(value.isMember("is_blue_vip"))
		userInfo.ExtraIntInfo["is_blue_vip"] = value["is_blue_vip"].asUInt() == 1?1:0;
	else
		userInfo.ExtraIntInfo["is_blue_vip"] = 0;

	if(value.isMember("is_blue_year_vip"))
		userInfo.ExtraIntInfo["is_blue_year_vip"] = value["is_blue_year_vip"].asUInt() == 1?1:0;
	else
		userInfo.ExtraIntInfo["is_blue_year_vip"] = 0;

	if(value.isMember("is_super_blue_vip"))
		userInfo.ExtraIntInfo["is_super_blue_vip"] = value["is_super_blue_vip"].asUInt() == 1?1:0;
	else
		userInfo.ExtraIntInfo["is_super_blue_vip"] = 0;

	return 0;
}

int CTencentPlatform::GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey)
{
	m_errorMessage.clear();

	string url = "http://" + m_config["v3domain"] + "/v3/relation/get_app_friends?";
	string osig = "GET&" + Crypt::UrlEncodeForTX("/v3/relation/get_app_friends") + "&";
	string qsig = "appid=" + Crypt::UrlEncodeForTX(m_appId) + "&format=json&openid="
			+ Crypt::UrlEncodeForTX(openid) + "&openkey=" + Crypt::UrlEncodeForTX(openkey)
			+ "&pf=" + Crypt::UrlEncodeForTX(m_pf) + "&userip=" + Crypt::UrlEncodeForTX(m_userip);
	string qstr = "appid=" + m_appId + "&format=json&openid=" + openid + "&openkey=" + openkey
			+ "&pf=" + m_pf + "&userip=" + m_userip;
	osig += Crypt::UrlEncodeForTX(qsig);
	string key = m_appKey + "&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);
	url += qstr + "&sig=" + Crypt::UrlEncodeForTX(sig);

	string response;
	if (!Network::HttpGetRequest(response, url) || response.empty())
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
int CTencentPlatform::Is_Login(const string &openid, const string &openkey,const string &pf)
{
	m_errorMessage.clear();
	string url = "http://" + m_config["v3domain"] + "/v3/user/is_login?";
	string osig = "GET&" + Crypt::UrlEncodeForTX("/v3/user/is_login") + "&";
	string qsig = "appid=" + m_appId + "&format=json&openid=" + openid + "&openkey=" + openkey
			+ "&pf=" + pf + "&userip=" + m_userip;
	string qstr = "appid=" + Crypt::UrlEncodeForTX(m_appId) + "&format=json&openid="
			+ Crypt::UrlEncodeForTX(openid) + "&openkey=" + Crypt::UrlEncodeForTX(openkey)
			+ "&pf=" + Crypt::UrlEncodeForTX(pf) + "&userip=" + Crypt::UrlEncodeForTX(m_userip);
	osig += Crypt::UrlEncodeForTX(qsig);
	string key = m_appKey + "&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);
	url += qstr + "&sig=" + Crypt::UrlEncodeForTX(sig);


	string response;
	if (!Network::HttpGetRequest(response, url) || response.empty())
	{
		m_errorMessage = response;
		error_log("[http request fail][openid=%s,openkey=%s,errmsg=%s]",
				openid.c_str(), openkey.c_str(), m_errorMessage.c_str());
		error_log("responce = %s",response.c_str());
		error_log("url = %s, qsig=%s",url.c_str(),qsig.c_str());
		return -1;
	}
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
	//error_log("responce = %s",response.c_str());
	return 0;
}

int CTencentPlatform::Is_area_Login(const string &openid, const string &openkey,const string &seqid)
{
	m_errorMessage.clear();

	string url = "http://" + m_config["v3domain"] + "/v3/user/is_area_login ?";
	string osig = "GET&" + Crypt::UrlEncodeForTX("/v3/user/is_area_login ") + "&";
	string qsig = "appid=" + Crypt::UrlEncodeForTX(m_appId) + "&format=json&openid="
			+ Crypt::UrlEncodeForTX(openid) + "&openkey=" + Crypt::UrlEncodeForTX(openkey)
			+ "&pf=" + Crypt::UrlEncodeForTX(m_pf) + "&userip=" + Crypt::UrlEncodeForTX(m_userip);
	string qstr = "appid=" + m_appId + "&format=json&openid=" + openid + "&openkey=" + openkey
			+ "&pf=" + m_pf + "&userip=" + m_userip + "&seqid=" + seqid;
	//string qstr =""
	osig += Crypt::UrlEncodeForTX(qsig);
	string key = m_appKey + "&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);
	url += qstr + "&sig=" + Crypt::UrlEncodeForTX(sig);

	string response;
	if (!Network::HttpGetRequest(response, url) || response.empty())
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
	int ret;
	if (!Json::GetInt(value, "ret", ret) || ret != 0)
	{
		Json::GetString(value, "msg", m_errorMessage);
		error_log("[get_info fail][openid=%s,openkey=%s,msg=%s]",
				openid.c_str(), openkey.c_str(), m_errorMessage.c_str());
		return -1;
	}
	return 0;
}

int CTencentPlatform::Set_Feed(const string &openid, const string &openkey)
{
	m_errorMessage.clear();

	string url = "http://" + m_config["v3domain"] + "/v3/spread/set_feed";
//	string url = "http://" + "119.147.19.43" + "/v3/spread/set_feed";   //test

	string osig = "POST&" + Crypt::UrlEncodeForTX("/v3/spread/set_feed") + "&";

	string qsig = "appid=" + Crypt::UrlEncodeForTX(m_appId) + "&flag=28&openid="
			+ Crypt::UrlEncodeForTX(openid) + "&openkey=" + Crypt::UrlEncodeForTX(openkey)
			+ "&pf=" + Crypt::UrlEncodeForTX(m_pf) + "&txt=welcome to this game&urlcmd_0=2&urlnum=1"
			+ "&urlparam_0=http://www.qq.com" + "&urltxt_0=click me to open a new world";

	osig += Crypt::UrlEncodeForTX(qsig);
	string key = m_appKey + "&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);

	string data;
	data.append("appid=").append(m_appId);
	data.append("&flag=28");
	data.append("&openid=").append(openid);
	data.append("&openkey=").append(openkey);
	data.append("&pf=").append(m_pf);
	data.append("&txt=welcome to this game");
	data.append("&urlcmd_0=2");
	data.append("&urlnum=1");
	data.append("&urlparam_0=http://www.qq.com");
	data.append("&urltxt_0=click me to open a new world");
	data.append("&sig=").append(sig);

	string response;
	if (!Network::HttpPostRequest(response, url, data) || response.empty())
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
	int ret;
	if (!Json::GetInt(value, "ret", ret) || ret != 0)
	{
		Json::GetString(value, "msg", m_errorMessage);
		error_log("[get_info fail][openid=%s,openkey=%s,msg=%s]",
				openid.c_str(), openkey.c_str(), m_errorMessage.c_str());
		return -1;
	}
	return 0;
}

int CTencentPlatform::Set_Request(const string &openid, const string &openkey,const string &fopenid)
{
	m_errorMessage.clear();

	string url = "http://" + m_config["v3domain"] + "/v3/spread/set_request";

	string osig = "POST&" + Crypt::UrlEncodeForTX("/v3/spread/set_request") + "&";

	string qsig = "appid=" + Crypt::UrlEncodeForTX(m_appId) + "&flag=7&fopenid="
			+ Crypt::UrlEncodeForTX(fopenid) + "&openid=" + Crypt::UrlEncodeForTX(openid)
			+ "&openkey=" + Crypt::UrlEncodeForTX(openkey)
			+ "&pf=" + Crypt::UrlEncodeForTX(m_pf) + "&txt=welcome to this game&urlcmd_0=2&urlnum=1"
			+ "&urlparam_0=http://www.qq.com" + "&urltxt_0=click me to open a new world";

	osig += Crypt::UrlEncodeForTX(qsig);
	string key = m_appKey + "&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);

	string data;
	data.append("appid=").append(m_appId);
	data.append("&flag=7");
	data.append("&fopenid=").append(fopenid);
	data.append("&openid=").append(openid);
	data.append("&openkey=").append(openkey);
	data.append("&pf=").append(m_pf);
	data.append("&txt=welcome to this game");
	data.append("&urlcmd_0=2");
	data.append("&urlnum=1");
	data.append("&urlparam_0=http://www.qq.com");
	data.append("&urltxt_0=click me to open a new world");
	data.append(sig);

	string response;
	if (!Network::HttpPostRequest(response, url, data) || response.empty())
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
	int ret;
	if (!Json::GetInt(value, "ret", ret) || ret != 0)
	{
		Json::GetString(value, "msg", m_errorMessage);
		error_log("[get_info fail][openid=%s,openkey=%s,msg=%s]",
				openid.c_str(), openkey.c_str(), m_errorMessage.c_str());
		return -1;
	}
	return 0;
}

int CTencentPlatform::Send_Notification(const string &openid, const string &openkey)
{
	m_errorMessage.clear();

	string url = "http://" + m_config["v3domain"] + "/v3/message/send_notification ?";

	string osig = "GET&" + Crypt::UrlEncodeForTX("/v3/message/send_notification ") + "&";

	string qsig = "appid=" + Crypt::UrlEncodeForTX(m_appId) + "&Ctxcmd=http://qq.com&Imgurl=http://minigameimg.qq.com/hello.gif&openid="
			+ Crypt::UrlEncodeForTX(openid) + "&openkey=" + Crypt::UrlEncodeForTX(openkey)
			+ "&pf=" + Crypt::UrlEncodeForTX(m_pf) + "&text=this is notice&title=notice_title&Viewcmd=http://qq.com" ;

	string qstr;
	qstr.append("appid=").append(m_appId);
	qstr.append("&Ctxcmd=http://qq.com");
	qstr.append("&Imgurl=http://minigameimg.qq.com/hello.gif");
	qstr.append("&openid=").append(openid);
	qstr.append("&openkey=").append(openkey);
	qstr.append("&pf=").append(m_pf);
	qstr.append("&text=this is notice");
	qstr.append("&title=notice_title");
	qstr.append("&Viewcmd=http://qq.com");

	osig += Crypt::UrlEncodeForTX(qsig);
	string key = m_appKey + "&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);
	url += qstr + "&sig=" + Crypt::UrlEncodeForTX(sig);

	string response;
	if (!Network::HttpGetRequest(response, url) || response.empty())
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
	int ret;
	if (!Json::GetInt(value, "ret", ret) || ret != 0)
	{
		Json::GetString(value, "msg", m_errorMessage);
		error_log("[get_info fail][openid=%s,openkey=%s,msg=%s]",
				openid.c_str(), openkey.c_str(), m_errorMessage.c_str());
		return -1;
	}
	return 0;
}

int CTencentPlatform::Get_Figure(OPUserInfo &userInfo,const string &openid, const string &openkey)
{
	m_errorMessage.clear();

	string url = "http://" + m_config["v3domain"] + "/v3/user/get_figure ?";

	string osig = "GET&" + Crypt::UrlEncodeForTX("/v3/user/get_figure ") + "&";

	string qsig = "appid=" + Crypt::UrlEncodeForTX(m_appId) + "&flag=1&openid="
			+ Crypt::UrlEncodeForTX(openid) + "&openkey=" + Crypt::UrlEncodeForTX(openkey)
			+ "&pf=" + Crypt::UrlEncodeForTX(m_pf);

	string qstr;
	qstr.append("appid=").append(m_appId);
	qstr.append("&flag=1");
	qstr.append("&openid=").append(openid);
	qstr.append("&openkey=").append(openkey);
	qstr.append("&pf=").append(m_pf);

	osig += Crypt::UrlEncodeForTX(qsig);
	string key = m_appKey + "&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);
	url += qstr + "&sig=" + Crypt::UrlEncodeForTX(sig);

	string response;
	if (!Network::HttpGetRequest(response, url) || response.empty())
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
	int ret;
	if (!Json::GetInt(value, "ret", ret) || ret != 0)
	{
		Json::GetString(value, "msg", m_errorMessage);
		error_log("[get_info fail][openid=%s,openkey=%s,msg=%s]",
				openid.c_str(), openkey.c_str(), m_errorMessage.c_str());
		return -1;
	}
	string figure_url;
	if (!Json::GetString(value, "figure_url", figure_url) || !figure_url.empty())
	{
		m_errorMessage = "figure_url is empty";
		error_log("[get_info fail][openid=%s,openkey=%s,msg=%s]",
				openid.c_str(), openkey.c_str(), m_errorMessage.c_str());
		return -1;
	}
	userInfo.FigureUrl = figure_url;
	return 0;
}

string CTencentPlatform::GetErrorMessage()
{
	return m_errorMessage;
}
