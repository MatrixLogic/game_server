#include "LogicInc.h"
#include "LogicVNPay.h"

class CCgiVNDeliver : public CCgiBase
{
public:
	CCgiVNDeliver() : CCgiBase("vndeliver")
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
		const string ts = CCGIIn::GetCGIStr("ts");
		if (ts.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（ts）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（ts）");
		}
		const string serverid = CCGIIn::GetCGIStr("serverid");
		if (serverid.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（payitem）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（payitem）");
		}
		const string num = CCGIIn::GetCGIStr("num");
		if (num.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（appid）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（appid）");
		}
		const string token = CCGIIn::GetCGIStr("token");
		if (token.empty())
		{
			CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（token）\"}");
			ERROR_RETURN_MSG(4, "请求参数错误：（token）");
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
			string osig = "GET&%2Fcgi%2Fvndeliver&" + Crypt::UrlEncodeForTX(qsig);
			string sha1Key = appkey + "&";
			string bnsig = Crypt::HmacSha1(osig, sha1Key);
			string nsig;
			Crypt::Base64Encode(nsig, bnsig);
			//info_log("[vndeliver sig][dsig=%s,nsig=%s,osig=%s,key=%s]",dsig.c_str(),nsig.c_str(),osig.c_str(),sha1Key.c_str());
			if (dsig != nsig)
			{
				error_log("[vndeliver sig error][dsig=%s,nsig=%s,osig=%s]",dsig.c_str(),nsig.c_str(),osig.c_str());
				CgiUtil::PrintText("{\"ret\":4,\"msg\":\"请求参数错误：（sig）\"}");
				ERROR_RETURN_MSG(4, "请求参数错误：（sig）");
			}
		}

		CLogicVNPay logicPay;
		int ret = logicPay.deliver(openid, CTrans::STOI(serverid),CTrans::STOI(num), token);
		if (ret != 0)
		{
			CgiUtil::PrintText("{\"ret\":"+CTrans::ITOS(ret)+",\"msg\":\"ChangePay failed\"}");
			return ret;
		}

		CgiUtil::PrintText("{\"ret\":0,\"msg\":\"OK\"}");
		CGI_SEND_LOG("openid=%s&billno=%s&serverid=%s&num=%s", openid.c_str(),token.c_str(),serverid.c_str(),num.c_str());
		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiVNDeliver)
