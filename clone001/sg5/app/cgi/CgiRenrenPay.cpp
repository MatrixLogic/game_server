#include "LogicInc.h"
#include "LogicRenrenPay.h"

class CCgiRenrenPay : public CCgiBase
{
public:

	CCgiRenrenPay() : CCgiBase("renrenpay")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
	}

	CGI_DIRECT_PROCESS(Buy)

	int Buy()
	{
		string openid = CCGIIn::GetCGIStr("openid");
		string openkey = CCGIIn::GetCGIStr("openkey");
		int type = CCGIIn::GetCGIInt("type");
		if (openid.empty() || type == CCGIIn::CGI_INT_ERR)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		string orderId;
		string token;
		CLogicRenrenPay logic;
		int ret = logic.PrePay(openid, openkey, type, orderId, token);
		if (ret != 0)
		{
			return ret;
		}

		m_jsonResult["order_id"] = orderId;
		m_jsonResult["token"] = token;

		CGI_SEND_LOG("openid=%s&type=%d&order_id=%s", openid.c_str(), type, orderId.c_str());
		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiRenrenPay)
