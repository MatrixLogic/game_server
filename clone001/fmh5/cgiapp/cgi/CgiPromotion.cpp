
#include "LogicInc.h"

class CCgiPromotion : public CCgiBase
{
public:
	CCgiPromotion() : CCgiBase("promotion")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
		//SetFeature(CF_COMPRESS_OUT);
	}

	CGI_SIMPLE_ACTION_MAP_BEGIN
	CGI_SET_ACTION_MAP("login", Login)
	CGI_ACTION_MAP_END

	int Login()
	{
		string openid = CCGIIn::GetCGIStr("openid");
		string openkey = CCGIIn::GetCGIStr("openkey");
		if (openid.empty() || openkey.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		string iopenid = CCGIIn::GetCGIStr("iopenid");
		string pf = CCGIIn::GetCGIStr("pf");

		OPUserInfo userinfo;
		unsigned uid = 0;
		int ret = 0;

		map<string, string> params;
		params["pf"] = pf;
		params["userip"] = m_ipstr;
		OpenPlatform::GetPlatform()->SetParameter(params);

		ret = OpenPlatform::GetPlatform()->GetUserInfo(userinfo, openid, openkey);
		if (ret != 0)
		{
			error_log("[GetUserInfo fail][ret=%d,openid=%s,openkey=%s]",
					ret,openid.c_str(),openkey.c_str());
			PT_ERROR_RETURN_MSG("get_user_info_fail");
		}

		CLogicUserMapping LogicUserMapping;
		bool isNewUser=false;
		string via;
		ret = LogicUserMapping.LoginPlatformUser(OpenPlatform::GetPlatform()->GetPlatformByPF(), userinfo, iopenid, uid, isNewUser,via);
		if(ret != 0)
			return ret;

		m_jsonResult["uid"] = uid;
		m_jsonResult["name"] = userinfo.Name;
		m_jsonResult["cur_pt"] = (int)OpenPlatform::GetType();
		m_jsonResult["currenttime"] = Time::GetGlobalTime();

		CGI_SEND_LOG("action=login&uid=%u&platform=%d&openid=%s&openkey=%s&iopenid=%s",
				uid, OpenPlatform::GetPlatform()->GetPlatformByPF(), openid.c_str(), openkey.c_str(), iopenid.c_str());
		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiPromotion)
