#include "LogicInc.h"

//参数
//openid	用户ID
//openkey	MD5(openid + appkey)
//
//返回
//JSON格式
//存在	{"error":0,"status":0}
//不存在	{"error":0,"status":1}
//错误	{"error":"param_error"}
//error	0表示成功，错误时返回错误字符串
//status	用户状态，成功时返回，1表示存在，0表示不存在

class CCgiKwCheckUser : public CCgiBase
{
public:

	CCgiKwCheckUser() : CCgiBase("kwcheckuser")
	{
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
		SetFeature(CF_PRINT_JSON);
	}

	CGI_DIRECT_PROCESS(KwCheckUser)

	int KwCheckUser()
	{
		//check platform
		if(OpenPlatform::GetType() != PT_KUAIWAN)
		{
			error_log("[platform_error][cgi=KwCheckUser,platform=%d]", OpenPlatform::GetType());
			SetError(R_ERR_REFUSE, "server_error");
			return R_ERR_REFUSE;
		}

		//check param
		string openid = CCGIIn::GetCGIStr("openid");
		string openkey = CCGIIn::GetCGIStr("openkey");
		if(openid.empty() || openkey.empty())
		{
			error_log("[param_error][cgi=KwCheckUser,openid=%s,openkey=%s]", openid.c_str(), openkey.c_str());
			return R_ERR_PARAM;
		}

		//check key
		string check = openid + OpenPlatform::GetPlatform()->GetAppKey();
		string key = Crypt::Md5Encode(check);
		if(!String::EqualNoCase(key, openkey))
		{
			error_log("[openkey_error][cgi=KwCheckUser,openid=%s,openkey=%s,realkey=%s]",
					openid.c_str(), openkey.c_str(), key.c_str());
			return R_ERR_PARAM;
		}

		//query
		int ret;
		CLogicUserBasic logicUser;
		unsigned uid;
		ret = logicUser.GetUid(uid, OpenPlatform::GetType(), openid);
		if(ret == 0)
		{
			m_jsonResult["status"] = 1;
		}
		else if(ret == R_ERR_NO_DATA)
		{
			m_jsonResult["status"] = 0;
		}
		else
		{
			return ret;
		}
		CGI_SEND_LOG("openid=%s&status=%d", openid.c_str(), (ret == 0 ? 1 : 0));
		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiKwCheckUser)

