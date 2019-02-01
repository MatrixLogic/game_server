#include "LogicInc.h"

//参数
//openid	用户ID
//openkey	MD5(openid + appkey)
//name	用户昵称(UTF-8编码)
//photo	用户头像URL地址(URL编码)
//
//返回
//成功	HTTP 302跳转到游戏页面
//失败	JSON格式，如{"error":"param_error"}
//error	错误字符串

#define CONFIG_GAME_HOME	"game_home"

class CCgiKwEntry : public CCgiBase
{
public:

	CCgiKwEntry() : CCgiBase("kwentry")
	{
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
	}

	CGI_DIRECT_PROCESS(KwEntry)

	int KwEntry()
	{
		//check platform
		if(OpenPlatform::GetType() != PT_KUAIWAN)
		{
			error_log("[platform_error][cgi=KwEntry,platform=%d]", OpenPlatform::GetType());
			CgiUtil::PrintText("{\"error\":\"server_error\"}");
			return R_ERR_REFUSE;
		}

		//check param
		string openid = CCGIIn::GetCGIStr("openid");
		string openkey = CCGIIn::GetCGIStr("openkey");
		string name = CCGIIn::GetCGIStr("name");
		string photo = CCGIIn::GetCGIStr("photo");
		if(openid.empty() || openkey.empty())
		{
			error_log("[param_error][cgi=KwEntry,openid=%s,openkey=%s]", openid.c_str(), openkey.c_str());
			CgiUtil::PrintText("{\"error\":\"param_error\"}");
			return R_ERR_PARAM;
		}

		//check key
		string check = openid + OpenPlatform::GetPlatform()->GetAppKey();
		string key = Crypt::Md5Encode(check);
		if(!String::EqualNoCase(key, openkey))
		{
			error_log("[openkey_error][cgi=KwEntry,openid=%s,openkey=%s,realkey=%s]",
					openid.c_str(), openkey.c_str(), key.c_str());
			CgiUtil::PrintText("{\"error\":\"param_error\"}");
			return R_ERR_PARAM;
		}

		Json::Value extraInfo;
		extraInfo["name"] = name;
		extraInfo["photo"] = photo;
		openkey = Crypt::UrlEncode(openkey + "|" + Json::ToString(extraInfo));

		//redirect
		string redirectUrl;
		String::Format(redirectUrl, "%s?openid=%s&openkey=%s&ts=%u",
				OpenPlatform::GetPlatform()->GetConfig(CONFIG_GAME_HOME).c_str(),
				openid.c_str(), openkey.c_str(), Time::GetGlobalTime());
		CgiUtil::HttpRedirect(redirectUrl.c_str());
		CGI_SEND_LOG("entry=ok&openid=%s", openid.c_str());
		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiKwEntry)

