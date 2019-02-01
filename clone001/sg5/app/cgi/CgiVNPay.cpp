#include "LogicInc.h"
#include "LogicVNPay.h"

class CCgiVNPay : public CCgiBase
{
public:
	CCgiVNPay() : CCgiBase("vnpay")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_GET_UID);
		SetFeature(CF_CHECK_SESSION);
		SetFeature(CF_CHECK_PLATFORM);
		SetFeature(CF_CHECK_HASH);
		SetFeature(CF_CHECK_VERSION);
		SetFeature(CF_APPEND_HASH);
		SetFeature(CF_PARSE_DATA);
		SetFeature(CF_CRYPT_DATA);
		SetFeature(CF_DECRYPT_DATA);
		SetFeature(CF_COMPRESS_OUT);
	}

	CGI_DIRECT_PROCESS(vnpay)

	int vnpay()
	{
		string openid;
		if(!Json::GetString(m_data, "openid", openid))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		string openkey;
		if(!Json::GetString(m_data, "openkey", openkey))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		string itemid;
		if(!Json::GetString(m_data, "itemid", itemid))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		if (openid.empty() || openkey.empty() || itemid.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		string url_params;

		CLogicVNPay logicPay;
		int ret = logicPay.v3_buy_goods(m_uid, openid, openkey, m_ipstr, itemid, url_params);
		if (ret != 0)
			return ret;

		m_jsonResult["url_params"] = url_params;

		CGI_SEND_LOG("openid=%s&itemid=%s", openid.c_str(),itemid.c_str());
		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiVNPay)
