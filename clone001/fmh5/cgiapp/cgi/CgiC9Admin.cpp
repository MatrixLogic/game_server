/*
 * CgiC9Admin.cpp
 *
 *  Created on: 2015-8-26
 *      Author: Ralf
 */


#include "LogicInc.h"

class CCgiC9Admin : public CCgiBase
{
public:
	CCgiC9Admin() : CCgiBase("c9admin")
	{
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
	}

	CGI_DIRECT_PROCESS(QueryUser)

	int QueryUser()
	{
		const unsigned serverid = CCGIIn::GetCGIInt("serverid");
		const string serverid_s = CCGIIn::GetCGIStr("serverid");
		const string openid = CCGIIn::GetCGIStr("uid");
		const unsigned time = CCGIIn::GetCGIInt("time");
		const string time_s = CCGIIn::GetCGIStr("time");
		const string sign = CCGIIn::GetCGIStr("sign");

		Json::Value result;
		result["errno"] = 0;
		result["errmsg"] = "查询成功";
		result["data"]["uid"] = openid;
		result["data"]["nickname"] = "";
		result["data"]["sex"] = "m";
		result["data"]["last_login"] = 0;
		result["data"]["loginlong"] = 0;
		result["data"]["group"] = 0;
		result["data"]["occupation"] = 0;
		result["data"]["level"] = 0;
		result["data"]["is_valid"] = 1;
		result["data"]["exp"] = 0;
		result["data"]["createtime"] = 0;

		if(openid.empty() || sign.empty() || time == CCGIIn::CGI_INT_ERR)
		{
			result["errno"] = -2;
			result["errmsg"] = "参数错误";
			return PrintResult(result);
		}

		if (Math::Abs(Time::GetGlobalTime() - time) > 180)
		{
			result["errno"] = -2;
			result["errmsg"] = "参数错误";
			return PrintResult(result);
		}

		string osig  = serverid_s + time_s + openid + OpenPlatform::GetPlatform()->GetAppKey();
		string sig = Crypt::Md5Encode(osig);
		if(sig != sign)
		{
			result["errno"] = -2;
			result["errmsg"] = "参数错误";
			return PrintResult(result);
		}

		// Get uid
		CDataUserMapping userMapper;
		unsigned uid = 0, updatetime = 0, lastlogin = 0;
		int ret = userMapper.GetMapping(openid, serverid, uid, updatetime, lastlogin);
		if (ret != 0)
		{
			result["errno"] = -1;
			result["errmsg"] = "未创建角色";
			return PrintResult(result);
		}
		result["data"]["last_login"] = lastlogin;
		result["data"]["createtime"] = updatetime;

		return PrintResult(result);
	}

private:
	int PrintResult(Json::Value &result)
	{
		Json::FastWriter writer;
		string res = writer.write(result);
		CgiUtil::PrintText(res.c_str());
		unsigned ret = 0;
		Json::GetUInt(result,"errno",ret);
		return ret;
	}
};

CGI_MAIN(CCgiC9Admin)




