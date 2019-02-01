#include "LogicInc.h"
#include "LogicQQReport.h"

class CCgiQQReport : public CCgiBase
{
public:
	CCgiQQReport() : CCgiBase("qqreport")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
	}

	CGI_DIRECT_PROCESS(qqReport)

	int qqReport()
	{
		const string openid = CCGIIn::GetCGIStr("openid");
		if (openid.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
			error_log("[qq report][no openid]");
		}

		unsigned report = 255;
		const string action = CCGIIn::GetCGIStr("action");
		if(action == "feed")
		{
			const string openkey = CCGIIn::GetCGIStr("openkey");
			const string pf = CCGIIn::GetCGIStr("pf");
			const string imgurl = CCGIIn::GetCGIStr("imgurl");
			const string text = CCGIIn::GetCGIStr("text");
			if(openkey.empty() || pf.empty() || imgurl.empty() || text.empty())
			{
				error_log("[qq set feed][openkey=%s,pf=%s,imgurl=%s,text=%s]", openkey.c_str(),pf.c_str(),imgurl.c_str(),text.c_str());
				PARAM_ERROR_RETURN_MSG("param_error");
			}

			CLogicQQReport logicReport;
			int ret = logicReport.SetFeed(openid,openkey,pf,imgurl,text);
			if (ret != 0)
				return ret;
		}
		else if(action == "setachievement")
		{
			const string openkey = CCGIIn::GetCGIStr("openkey");
			const string pf = CCGIIn::GetCGIStr("pf");
			const string level = CCGIIn::GetCGIStr("level");
			const string serverid = CCGIIn::GetCGIStr("serverid");
			if(openkey.empty() || pf.empty() || level.empty() || serverid.empty())
			{
				error_log("[qq setachievement][openkey=%s,pf=%s,level=%s,serverid=%s]", openkey.c_str(),pf.c_str(),level.c_str(),serverid.c_str());
				PARAM_ERROR_RETURN_MSG("param_error");
			}

			CLogicQQReport logicReport;
			int ret = logicReport.SetAchievement(openid,openkey,pf,level,serverid);
			if (ret != 0)
				return ret;
		}
		else if (action == "alliance_registe")
		{
			//@add oulong 20160523
			//腾讯 游戏联盟额外增加一次上报（不影响原先的上报）：
			try
			{
				CLogicQQReport logicReport;
				TxAllianceRegisterApi api;
				int ret = logicReport.TxAlliance(api);
				if (ret != 0)
				{
					return ret;
				}
			}
			catch (const std::exception& e)
			{
				::SetError(R_ERROR, e.what());
				return R_ERROR;
			}
		}
		else
		{
			report = CCGIIn::GetCGIInt("report");
			unsigned uid = CCGIIn::GetCGIInt("uid");
			unsigned fee = CCGIIn::GetCGIInt("fee");
			const string userip = CCGIIn::GetCGIStr("userip");
			const string pf = CCGIIn::GetCGIStr("pf");
			const string zoneid = CCGIIn::GetCGIStr("zoneid");

			CLogicQQReport logicReport;
			int ret = logicReport.Report(report,openid,uid,fee,userip,pf,zoneid);
			if (ret != 0)
				return ret;
		}

		CGI_SEND_LOG("openid=%s&report=%u", openid.c_str(),report);
		return R_SUCCESS;
	}
};

CGI_MAIN(CCgiQQReport)
