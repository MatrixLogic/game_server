#include "LogicInc.h"
#include "LogicRenrenPay.h"

class CCgiRenrenPay : public CCgiBase
{
public:

	CCgiRenrenPay() : CCgiBase("rrconfirm")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
	}

	CGI_DIRECT_PROCESS(Confirm)

	int Confirm()
	{
		string openid = CCGIIn::GetCGIStr("xn_sig_user");
		string sig = CCGIIn::GetCGIStr("xn_sig_skey");
		string orderId = CCGIIn::GetCGIStr("xn_sig_order_id");
		if (openid.empty() || sig.empty() || orderId.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		int amount = 0;
		CLogicRenrenPay logic;
		int ret = logic.DeliverGood(openid, sig, orderId, amount);
		if (ret != 0)
			return ret;

		m_jsonResult["app_res_user"] = openid;
		m_jsonResult["app_res_order_id"] = orderId;
		m_jsonResult["app_res_amount"] = amount;

		CGI_SEND_LOG("openid=%s&orderId=%s&amount=%d", openid.c_str(), orderId.c_str(), amount);
		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiRenrenPay)
