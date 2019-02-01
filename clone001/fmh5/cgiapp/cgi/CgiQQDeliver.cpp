#include "LogicInc.h"
#include "LogicQQPay.h"

class CCgiQQDeliver : public CCgiBase
{
public:
	CCgiQQDeliver() : CCgiBase("qqdeliver")
	{
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
	}

	CGI_DIRECT_PROCESS(deliver)

	int deliver()
	{
		//error_log("qqdeliever failed");
		const string openid = CCGIIn::GetCGIStr("openid");
		if (openid.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（openid）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（openid）");
		}
		//error_log("qqdeliever failed");
		const string appid = CCGIIn::GetCGIStr("appid");
		if (appid.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（appid）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（appid）");
		}
		//error_log("qqdeliever failed");
		const string ts = CCGIIn::GetCGIStr("ts");
		if (ts.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（ts）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（ts）");
		}
		const string payitem = CCGIIn::GetCGIStr("payitem");
		if (payitem.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（payitem）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（payitem）");
		}
		//error_log("qqdeliever failed");
		const string amt = CCGIIn::GetCGIStr("amt");
		if (amt.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（amt）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（amt）");
		}
		//error_log("qqdeliever failed");
		const string token = CCGIIn::GetCGIStr("token");
		if (token.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（token）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（token）");
		}
		const string billno = CCGIIn::GetCGIStr("billno");
		if (billno.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（billno）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（billno）");
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
			string osig = "GET&%2Fcgi%2Fqqdeliver&" + Crypt::UrlEncodeForTX(qsig);
			string sha1Key = appkey + "&";
			string bnsig = Crypt::HmacSha1(osig, sha1Key);
			string nsig;
			Crypt::Base64Encode(nsig, bnsig);
			//info_log("[qqdeliver sig][dsig=%s,nsig=%s,osig=%s,key=%s]",dsig.c_str(),nsig.c_str(),osig.c_str(),sha1Key.c_str());
			if (dsig != nsig)
			{
				error_log("[qqdeliver sig error][dsig=%s,nsig=%s,osig=%s]",dsig.c_str(),nsig.c_str(),osig.c_str());
				CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（sig）\"}");
				ERROR_RETURN_MSG(4, "请求参数错误：（sig）");
			}
		}
		//error_log("qqdeliever failed");
		CLogicQQPay logicPay;
		int ret = logicPay.deliver(appid, openid, payitem, CTrans::STOI(amt), billno);
		if (ret != 0)
		{
			CgiUtil::PrintText("{\"ret\":"+CTrans::ITOS(::GetError())+",\"msg\":\""+::GetErrorMessage()+"\"}");
			return ret;
		}
		CgiUtil::PrintText("{\"ret\":0,\"msg\":\"OK\"}");

		CGI_SEND_LOG("openid=%s&billno=%s&payitem=%s&amt=%s", openid.c_str(),billno.c_str(),payitem.c_str(),amt.c_str());
		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiQQDeliver)
