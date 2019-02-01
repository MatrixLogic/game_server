#include "LogicInc.h"
#include "LogicQQPay.h"

class CCgiQQPay : public CCgiBase
{
public:
	CCgiQQPay() : CCgiBase("qqpay")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
	}

	CGI_DIRECT_PROCESS(qqpay)

	int qqpay()
	{
		const string openid = CCGIIn::GetCGIStr("openid");
		const string openkey = CCGIIn::GetCGIStr("openkey");
		const string itemid = CCGIIn::GetCGIStr("itemid");
		const string pf = CCGIIn::GetCGIStr("pf");
		const string pfkey = CCGIIn::GetCGIStr("pfkey");
		if (openid.empty() || openkey.empty() || itemid.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		string appid;
		string url_params;

		CLogicQQPay logicPay;
		int ret = logicPay.v3_buy_goods(pf, pfkey, openid, openkey, m_ipstr, itemid, appid, url_params);
		if (ret != 0)
			return ret;
//		int ret = logicPay.qz_buy_goods(openid, openkey, m_ipstr, itemid, appid, url_params);
//		if (ret != 0)
//			return ret;

		m_jsonResult["appid"] = appid;
		m_jsonResult["url_params"] = url_params;

		CGI_SEND_LOG("openid=%s&itemid=%s", openid.c_str(),itemid.c_str());
		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiQQPay)
