#include "LogicInc.h"
#include "LogicQQPay.h"

class CCgiQQCheck : public CCgiBase
{
public:
	CCgiQQCheck() : CCgiBase("qqcheck")
	{
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
	}

	CGI_DIRECT_PROCESS(deliver)

	int deliver()
	{
		const string openid = CCGIIn::GetCGIStr("openid");
		if (openid.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（openid）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（openid）");
		}

		const string appid = CCGIIn::GetCGIStr("appid");
		if (appid.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（appid）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（appid）");
		}

		const string pf = CCGIIn::GetCGIStr("pf");
		if (pf.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（pf）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（pf）");
		}

		const string ts = CCGIIn::GetCGIStr("ts");
		if (ts.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（ts）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（ts）");
		}

		const string version = CCGIIn::GetCGIStr("version");
		if (version.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（version）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（version）");
		}

		const string contractid = CCGIIn::GetCGIStr("contractid");
		if (contractid.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（contractid）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（contractid）");
		}

		const string sig = CCGIIn::GetCGIStr("sig");
		if (sig.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（sig）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（sig）");
		}
		else
		{
			string appkey = OpenPlatform::GetPlatform()->GetAppKey();
			string qstr = getenv("QUERY_STRING");
			map<string, string> params;
			vector<string> reqs;
			CBasic::StringSplit(qstr, "&", reqs);
			for (unsigned i = 0; i < reqs.size(); i++)
			{
				string key;
				string value;
				size_t found;
				found = reqs[i].find('=');
				if (found != string::npos)
				{
					key = reqs[i].substr(0, found);
					value = reqs[i].substr(found + 1);
				}
				else
				{
					key = reqs[i];
					value = "";
				}
				params[key] = value;
			}
			string dsig;
			bool bfirst = true;
			string qsig;
			map<string, string>::const_iterator it = params.begin();
			for (; it != params.end(); it++)
			{
				if (it->first == "sig")
				{
					dsig = CBasic::unescape(it->second);
				}
				else
				{
					if (!bfirst) qsig += "&";
					qsig += it->first + "=" + Crypt::EncodeForTXSig(it->second);
					bfirst = false;
				}
			}
			string osig = "GET&%2Fcgi%2Fcheck_completion&" + Crypt::UrlEncodeForTX(qsig);
			string sha1Key = appkey + "&";
			string bnsig = Crypt::HmacSha1(osig, sha1Key);
			string nsig;
			Crypt::Base64Encode(nsig, bnsig);
			if (dsig != nsig)
			{
				error_log("[CheckTask sig error][dsig=%s,nsig=%s,osig=%s]",dsig.c_str(),nsig.c_str(),osig.c_str());
				CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（sig）\"}");
				ERROR_RETURN_MSG(4, "请求参数错误：（sig）");
			}

		}

		CLogicQQPay logicPay;
		int ret = logicPay.CheckTask(appid, openid, contractid);
		if (ret != 0)
		{
			CgiUtil::PrintText("{\"ret\":"+CTrans::ITOS(::GetError())+",\"msg\":\""+::GetErrorMessage()+"\"}");
			return ret;
		}
		CgiUtil::PrintText("{\"ret\":0,\"msg\":\"OK\"}");
		CGI_SEND_LOG("openid=%s,contractid=%s", openid.c_str(),contractid.c_str());
		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiQQCheck)
