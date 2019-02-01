#include "LogicQQReport.h"
#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>


TxAllianceApi::~TxAllianceApi()
{
}

std::string TxAllianceApi::MakeRequestData() const
{
	std::string data;
	for (std::map<string, string>::const_iterator it = m_params.begin();
			it != m_params.end();
			++it)
	{
		if (it != m_params.begin())
		{
			data += "&";
		}
		data +=  it->first + "=" + Crypt::UrlEncodeForTX(it->second);
	}

	return data;
}

const std::string& TxAllianceApi::GetValue(const std::string& key)
{
	if (m_params.count(key) == 0)
	{
		throw std::runtime_error("TxAllianceApi::GetValue_not_exist_key");
	}

	return m_params[key];
}

unsigned TxAllianceApi::GetUserIP() const
{
	const string userip = CCGIIn::GetCGIStr("userip");
	unsigned ip;
	if(userip.empty())
		ip = CCGIIn::GetRemoteIP();
	else
	{
		struct in_addr addr;
		inet_aton(userip.c_str(), &addr);
		ip = ntohl(addr.s_addr);
	}

	return ip;
}

void TxAllianceApi::SetServiceIp(unsigned svrip)
{
	m_params["svrip"] = CTrans::UTOS(svrip);
}

//--
TxAllianceRegisterApi::TxAllianceRegisterApi()
{
	if (NULL == OpenPlatform::GetPlatform())
	{
		error_log("[qq report][not_config_plathform]");
		throw std::logic_error("not_config_plathform");
	}

	m_params["version"] = "1";
	m_params["appid"] = OpenPlatform::GetPlatform()->GetAppId();
	m_params["userip"] = CTrans::UTOS(GetUserIP());
	//m_params["svrip"] = CTrans::UTOS(m_ip);
	m_params["time"] = CTrans::UTOS(Time::GetGlobalTime());

	std::string szPf = CCGIIn::GetCGIStr("pf");
	if(szPf.find("union") == string::npos)
	{
		error_log("[qq report][not_TxAlliance_plat]");
		throw std::logic_error("not_TxAlliance_plat");
	}

	m_params["worldid"] = CCGIIn::GetCGIStr("zoneid");
	m_params["opuid"] = CCGIIn::GetCGIStr("uid");
	m_params["opopenid"] = CCGIIn::GetCGIStr("openid");
	m_params["pf"] = szPf;
	m_params["openkey"] = CCGIIn::GetCGIStr("openkey");
	m_params["pfkey"] = CCGIIn::GetCGIStr("pfkey");
}


const char* TxAllianceRegisterApi::Name() const
{
	return  "Register.cgi";
}

//--------------------------------------------------------------------------------
unsigned  CLogicQQReport::m_ip = 0;


CLogicQQReport::CLogicQQReport()
{
	if(!m_ip)
	{
		struct ifaddrs * ifAddrStruct = NULL;

		getifaddrs(&ifAddrStruct);

		while (ifAddrStruct != NULL) {
			if (ifAddrStruct->ifa_addr->sa_family == AF_INET) { // check it is IP4
				// is a valid IP4 Address
				string eth = ifAddrStruct->ifa_name;
				if(eth == "eth1")
					m_ip = ntohl(((struct sockaddr_in *) ifAddrStruct->ifa_addr)->sin_addr.s_addr);
			}
			ifAddrStruct = ifAddrStruct->ifa_next;
		}

		//info_log("[qq report][m_ip=%u]", m_ip);
	}
}

QQPT CLogicQQReport::GetQQPT(string pt)
{
	if(pt.find("union") != string::npos)
		return QQPT_union;
	if(pt == "qzone")
		return QQPT_qzone;
	if(pt == "pengyou")
		return QQPT_pengyou;
	if(pt == "weibo")
		return QQPT_weibo;
	if(pt == "qqgame")
		return QQPT_qqgame;
	if(pt == "3366")
		return QQPT_3366;

	return QQPT_none;
}

int CLogicQQReport::Report(unsigned report, string openid, unsigned uid, unsigned fee, string userip, string pf, string zoneid)
{
	int ret = 0;

	if(report > QQREPORT_max)
		return 1;

	if(openid.empty())
		return 2;

	unsigned uidtemp;
	CDataUserMapping dbUserMapping;
	ret = dbUserMapping.GetMapping(openid, PT_PENGYOU, uidtemp);
	if (ret)
	{
		if(!uid)
			return 3;
	}
	else
		uid = uidtemp;

	unsigned ip;
	if(userip.empty())
		ip = CCGIIn::GetRemoteIP();
	else
	{
		struct in_addr addr;
		inet_aton(userip.c_str(), &addr);
		ip = ntohl(addr.s_addr);
	}

	unsigned pt;
	if(pf.empty())
		pf = CCGIIn::GetCGIStr("pf");
	pt = GetQQPT(pf);
	if(!pt)
		return 4;

	if(zoneid.empty())
	{
		string host = getenv("SERVER_NAME");
		vector<string> rlt;
		String::Split(host, '.', rlt);
		zoneid = rlt[0].substr(1,rlt[0].length()-1);
	}

	const string reportURL="http://tencentlog.com/stat/";
	const string reportAPI[QQREPORT_max]={
			"report_login.php",
			"report_register.php",
			"report_accept.php",
			"report_invite.php",
			"report_consume.php"
	};

	string url = reportURL + reportAPI[report] + "?version=1";
	url += "&appid=" + OpenPlatform::GetPlatform()->GetAppId();
	url += "&userip=" + CTrans::UTOS(ip);
	url += "&svrip=" + CTrans::UTOS(m_ip);
	url += "&time=" + CTrans::UTOS(Time::GetGlobalTime());
	url += "&domain=" + CTrans::UTOS(pt);
	url += "&worldid=" + zoneid;
	url += "&opuid=" + CTrans::UTOS(uid);
	url += "&opopenid=" + openid;
	if(report == QQREPORT_consume)
		url += "&modifyfee=" + CTrans::UTOS(fee);

	//对于联盟大区，修改原有向罗盘的基础指标的数据上报，加入reserve_5字段，字段值为整个PF
	if (QQPT_union == GetQQPT(pf))
	{
		url += "&reserve_5=" + pf;
	}


	string response;
	if (!Network::HttpGetRequest(response, url) || response.empty())
	{
		error_log("[report request fail][url=%s]", url.c_str());
		return 5;
	}
	Json::Value value;
	if(!Json::Reader().parse(response, value))
	{
		error_log("[report parse fail][url=%s]", url.c_str());
		return 6;
	}
	if (!Json::GetInt(value, "ret", ret) || ret != 0)
	{
		//error_log("[report fail][url=%s,response=%s]", url.c_str(),response.c_str());
		return 7;
	}
	//info_log("[qq report][url=%s, respons=%s]", url.c_str(), response.c_str());

	return 0;
}

int CLogicQQReport::SetFeed(const string &openid, const string &openkey, const string &pf, const string &imgurl, const string &text)
{
	string url = "http://" + OpenPlatform::GetPlatform()->GetConfig("v3domain") + "/v3/spread/set_feed";

	string osig = "POST&" + Crypt::UrlEncodeForTX("/v3/spread/set_feed") + "&";

	string qsig = "appid=" + OpenPlatform::GetPlatform()->GetAppId()
			+ "&charset=utf-8" + "&flag=28"
			+ "&imgurl=" + imgurl
			+ "&openid=" + openid
			+ "&openkey=" + openkey
			+ "&pf=" + pf
			+ "&txt=" + text
			+ "&urlcmd_0=1&urlnum=1"
			+ "&urlparam_0=" + "&urltxt_0=PLAY";

	osig += Crypt::UrlEncodeForTX(qsig);
	//error_log("[qq set feed sig][sig=%s]", osig.c_str());
	string key = OpenPlatform::GetPlatform()->GetAppKey() + "&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);

	string data;
	data.append("appid=").append(Crypt::UrlEncodeForTX(OpenPlatform::GetPlatform()->GetAppId()));
	data.append("&charset=").append(Crypt::UrlEncodeForTX("utf-8"));
	data.append("&flag=28");
	data.append("&imgurl=").append(Crypt::UrlEncodeForTX(imgurl));
	data.append("&openid=").append(Crypt::UrlEncodeForTX(openid));
	data.append("&openkey=").append(Crypt::UrlEncodeForTX(openkey));
	data.append("&pf=").append(Crypt::UrlEncodeForTX(pf));
	data.append("&txt=").append(Crypt::UrlEncodeForTX(text));
	data.append("&urlcmd_0=1");
	data.append("&urlnum=1");
	data.append("&urlparam_0=");
	data.append("&urltxt_0=PLAY");
	data.append("&sig=").append(Crypt::UrlEncodeForTX(sig));

	string response;
	if (!Network::HttpPostRequest(response, url, data) || response.empty())
	{
		error_log("[http request fail][openid=%s,url=%s,response=%s]",
				openid.c_str(), url.c_str(), response.c_str());
		return -1;
	}

	Json::Value value;
	if(!Json::Reader().parse(response, value))
	{
		error_log("[json parse fail][openid=%s,data=%s,response=%s]",
				openid.c_str(), data.c_str(), response.c_str());
		return -1;
	}
	int ret;
	if (!Json::GetInt(value, "ret", ret) || ret != 0)
	{
		//error_log("[json ret fail][openid=%s,data=%s,response=%s]",	openid.c_str(), data.c_str(), response.c_str());
		return -1;
	}
	//info_log("[qq set feed][data=%s, respons=%s]", data.c_str(), response.c_str());

	return 0;
}

int CLogicQQReport::SetAchievement(const string &openid, const string &openkey, const string &pf,string level, string zoneid)
{
	string attr = "{\"level\":" + level + ",\"area_name\":\"" + zoneid + "\"}";

	string url = "http://" + OpenPlatform::GetPlatform()->GetConfig("v3domain") + "/v3/user/set_achievement";

	string osig = "POST&" + Crypt::UrlEncodeForTX("/v3/user/set_achievement") + "&";

	string qsig = "appid=" + OpenPlatform::GetPlatform()->GetAppId()
			+ "&openid=" + openid
			+ "&openkey=" + openkey
			+ "&pf=" + pf
			+ "&user_attr=" + attr;

	osig += Crypt::UrlEncodeForTX(qsig);
	//error_log("[qq setachievement sig][sig=%s]", osig.c_str());
	string key = OpenPlatform::GetPlatform()->GetAppKey() + "&";
	string bsig = Crypt::HmacSha1(osig, key);
	string sig;
	Crypt::Base64Encode(sig, bsig);

	string data;
	data.append("appid=").append(Crypt::UrlEncodeForTX(OpenPlatform::GetPlatform()->GetAppId()));
	data.append("&openid=").append(Crypt::UrlEncodeForTX(openid));
	data.append("&openkey=").append(Crypt::UrlEncodeForTX(openkey));
	data.append("&pf=").append(Crypt::UrlEncodeForTX(pf));
	data.append("&user_attr=").append(Crypt::UrlEncodeForTX(attr));
	data.append("&sig=").append(Crypt::UrlEncodeForTX(sig));

	string response;
	if (!Network::HttpPostRequest(response, url, data) || response.empty())
	{
		error_log("[http request fail][openid=%s,url=%s,response=%s]",
				openid.c_str(), url.c_str(), response.c_str());
		return -1;
	}

	Json::Value value;
	if(!Json::Reader().parse(response, value))
	{
		error_log("[json parse fail][openid=%s,data=%s,response=%s]",
				openid.c_str(), data.c_str(), response.c_str());
		return -1;
	}
	int ret;
	if (!Json::GetInt(value, "ret", ret) || ret != 0)
	{
		//error_log("[json ret fail][openid=%s,data=%s,response=%s]",	openid.c_str(), data.c_str(), response.c_str());
		return -1;
	}
	//info_log("[qq setachievement][data=%s, respons=%s]", data.c_str(), response.c_str());

	return 0;
}

int CLogicQQReport::HallAction(QQHallApi& api)
{
	std::string szDomain;
	if (OpenPlatform::GetPlatform())
	{
		szDomain = OpenPlatform::GetPlatform()->GetConfig("v3domain");
	}

	//测试url
	if (szDomain.empty())
	{
		szDomain = "119.147.19.43";
	}

	std::string uri = "/v3/message/" + std::string(api.Name());

	std::string url = "http://" + szDomain + uri;

	int ret = 0;
	std::string data;
	if ((ret = api.MakeRequestData(uri, data)) != R_SUCCESS)
	{
		error_log("[MakeGetData][Openid=%s,ApiName:%s]",
				api.OpenId().c_str(), api.Name());
		return ret;
	}

	url += "?" + data;
	string response;
	if (!Network::HttpGetRequest(response, url) || response.empty())
	{
		error_log("[http request fail][openid=%s,url=%s,response=%s,ApiName:%s]",
				api.OpenId().c_str(), url.c_str(), response.c_str(),api.Name());
		return -1;
	}

	Json::Value value;
	if(!Json::Reader().parse(response, value))
	{
		error_log("[json parse fail][openid=%s,data=%s,response=%s,ApiName:%s]",
				api.OpenId().c_str(), data.c_str(), response.c_str(), api.Name());
		return -1;
	}

	if (!Json::GetInt(value, "ret", ret) || ret != 0)
	{
		error_log("[json ret fail][openid=%s,data=%s,response=%s,ApiName:%s]"
				,	api.OpenId().c_str(), data.c_str(), response.c_str(), api.Name());
		return -1;
	}
	//debug_log("url=%s,response=%s", url.c_str(), response.c_str());

	return R_SUCCESS;
}

int CLogicQQReport::TxAlliance(TxAllianceApi& api)
{
	/**
	 * 2.5 请务必在日志中记录失败用户的openid、pf、pfkey 、iRet，对于失败和链接错误的用户，请重新上报一次并一定进行记录。
	 */
	int cnt = 0;
	int ret = 0;
	try
	{
		api.SetServiceIp(m_ip);

		std::string szParams = api.MakeRequestData();

		std::string url = "http://union.tencentlog.com/cgi-bin/"
				+ std::string(api.Name()) + "?" + szParams;

		do
		{
			std::string response;
			if (!Network::HttpGetRequest(response, url) || response.empty())
			{
				error_log("[http_request_fail][openid=%s,pf=%s,url=%s, resp:%s,ApiName:%s]"
						, api.GetValue("opopenid").c_str(), api.GetValue("pf").c_str()
						, url.c_str(), response.c_str(),api.Name());
				return -1;
			}

			Json::Value jsonRetData;
			if(!Json::Reader().parse(response, jsonRetData))
			{
				error_log("[json_parse_fail][openid=%s,pf=%s,url=%s, resp:%s,ApiName:%s]"
								, api.GetValue("opopenid").c_str(), api.GetValue("pf").c_str()
								, url.c_str(), response.c_str(),api.Name());
				return -1;
			}

			int ret = 0;
			if (!Json::GetInt(jsonRetData, "iRet", ret) || ret != 0)
			{
				error_log("[report_fail][openid=%s,pf=%s,pfkey:%s,iRet=%d,cnt:%d,Msg:%s]"
								, api.GetValue("opopenid").c_str()
								, api.GetValue("pf").c_str()
								, api.GetValue("pfkey").c_str()
								, ret, cnt
								, jsonRetData["sMsg"].asString().c_str());
			}

			if (ret == 0 || cnt > 0) break;

			++ cnt;

		} while(1);
	}
	catch(const std::exception& e)
	{
		error_log("%s", e.what());
		return -1;
	}

	return ret;
}
