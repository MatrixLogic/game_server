
#include "LogicInc.h"

class CCgiLogin : public CCgiBase
{
public:
	CCgiLogin() : CCgiBase("login")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
		SetFeature(CF_CHECK_VERSION);
	}

	CGI_DIRECT_PROCESS(Login)

	int Login()
	{
		//解析参数
		string openid = CCGIIn::GetCGIStr("openid");
		string openkey = CCGIIn::GetCGIStr("openkey");
		string iopenid = CCGIIn::GetCGIStr("iopenid");
		string pf = CCGIIn::GetCGIStr("pf");
		unsigned app_appbitmap = CCGIIn::GetCGIInt("app_appbitmap");
		string via = CCGIIn::GetCGIStr("via");
		string time = CCGIIn::GetCGIStr("time");
		string cm = CCGIIn::GetCGIStr("cm");
		int isWhite = openkey == "white" ? 1 : 0;
		if (openid.empty())
		{
			error_log("[openid is empty][platform=%d,openid=%s,openkey=%s]",
					OpenPlatform::GetType(), openid.c_str(),openkey.c_str());
			PARAM_ERROR_RETURN_MSG("check_openid_fail");
		}
		if(OpenPlatform::GetType() == PT_360)
		{
			openid = Crypt::UrlDecode(openid);
		}

		//GM flag
		CLogicAdmin logicAdmin;
		unsigned gmFlag = gm_none;
		logicAdmin.CheckGM(openid, gmFlag);
		if(gmFlag == gm_forbid)
		{
			REFUSE_RETURN_MSG("being_blocked");
		}
		m_jsonResult["flag"] = gmFlag;

		//  获取UID
		OPUserInfo userinfo;
		unsigned uid = 0;
		bool isNewUser = false;
		if (isWhite == 1)
		{
			if(gmFlag == gm_white)
			{
				CDataUserMapping dbUserMapping;
				int ret = dbUserMapping.GetMapping(openid, PT_TEST, uid);
				if(ret)
				{
					error_log("[GetMapping fail][ret=%d,openid=%s]", ret, openid.c_str());
					DB_ERROR_RETURN_MSG("db_get_user_mapping_fail");
				}
			}
			else
			{
				error_log("[invalid white user][openid=%s,openkey=%s]", openid.c_str(), openkey.c_str());
				REFUSE_RETURN_MSG("check_white_user_fail");
			}

			info_log("[white user login][uId=%d,openid=%s,openkey=%s]", uid, openid.c_str(), openkey.c_str());
		}
		else
		{
			// 非白名单
			int ret = 0;

			map<string, string> params;
			params["pf"] = pf;
			params["userip"] = m_ipstr;
			params["app_appbitmap"] = CTrans::ITOS(app_appbitmap);
			params["time"] = time;
			params["cm"] = cm;
			if(OpenPlatform::GetType() == PT_XUNLEI)
			{
				params["clienttype"] = CCGIIn::GetCGIStr("clienttype");
			}
			else if(OpenPlatform::GetType() == PT_7k7k)
			{
				params["vip_type"] = CCGIIn::GetCGIStr("vip_type");
				params["vip_expired"] = CCGIIn::GetCGIStr("vip_expired");
			}
			OpenPlatform::GetPlatform()->SetParameter(params);

			ret = OpenPlatform::GetPlatform()->GetUserInfo(userinfo, openid, openkey);
			if (ret != 0)
			{
				if(OpenPlatform::GetType() == PT_TEST && ret == R_ERR_DATA_LIMIT)
				{
					ERROR_RETURN_MSG(R_ERR_DATA_LIMIT, "名字中包含非法字符");
				}

				error_log("[GetUserInfo fail][ret=%d,openid=%s,openkey=%s]",
						ret,openid.c_str(),openkey.c_str());
				PT_ERROR_RETURN_MSG("get_user_info_fail");
			}

			CLogicUserMapping LogicUserMapping;
			ret = LogicUserMapping.LoginPlatformUser(OpenPlatform::GetPlatform()->GetPlatformByPF(), userinfo, iopenid, uid,isNewUser,via);
			if(ret != 0)
				return ret;
		}

		userinfo.msg.set_uid(uid);
		userinfo.msg.set_openkey(openkey);
		userinfo.msg.set_ts(Time::GetGlobalTime());
		userinfo.msg.set_fig(userinfo.FigureUrl);
		userinfo.msg.set_isnew(isNewUser);
		userinfo.msg.set_platform((unsigned)OpenPlatform::GetPlatform()->GetPlatformByPF());
		userinfo.msg.set_openid(openid);

		string suid = CTrans::UTOS(uid);
		string sts = CTrans::UTOS(Time::GetGlobalTime());
		string sak(DAWX_RALF_KEY);
		string sig = Crypt::Md5Encode(suid + openkey + sts + sak);
		userinfo.msg.set_sig(sig);

		//string out;
		//userinfo.msg.SerializeToString(&out);
		Proto2Json p2j;
		p2j._p2j(&userinfo.msg, m_jsonResult["msg"]);

		m_jsonResult["open_ts"] = Config::GetIntValue(CONFIG_OPEN_TS);
		m_jsonResult["cur_pt"] = (int)OpenPlatform::GetType();
		m_jsonResult["currenttime"] = Time::GetGlobalTime();
		m_jsonResult["tcp_server"] = Config::GetValue(CONFIG_TCP_SERVER);
		m_jsonResult["access_server"] = Config::GetValue(CONFIG_ACCESS_SERVER);
		int serveridb;
		Config::GetDomain(serveridb);
		m_jsonResult["serverid"] = serveridb;
		set<int> db;
		MainConfig::GetIncludeDomains(serveridb, db);
		m_jsonResult["domains"].resize(0);
		for(set<int>::iterator it=db.begin();it!=db.end();++it)
			m_jsonResult["domains"].append(*it);

		CGI_SEND_LOG("uid=%u&platform=%d&openid=%s&openkey=%s&iopenid=%s&isWhite=%d",
				uid, OpenPlatform::GetPlatform()->GetPlatformByPF(), openid.c_str(), openkey.c_str(), iopenid.c_str(), isWhite);
		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiLogin)
