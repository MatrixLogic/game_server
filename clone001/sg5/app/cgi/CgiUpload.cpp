#include "LogicInc.h"

class CCgiUpload : public CCgiBase
{
public:

	CCgiUpload() : CCgiBase("upload")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_GET_UID);
		SetFeature(CF_CHECK_SESSION);
		SetFeature(CF_CHECK_PLATFORM);
		SetFeature(CF_CHECK_HASH);
		SetFeature(CF_CHECK_VERSION);
		SetFeature(CF_APPEND_HASH);
		SetFeature(CF_CRYPT_DATA);
		SetFeature(CF_DECRYPT_DATA);
		//SetFeature(CF_COMPRESS_OUT);
	}

	CGI_SIMPLE_ACTION_MAP_BEGIN
	CGI_SET_ACTION_MAP("setvedio", SetVedio)
	CGI_ACTION_MAP_END

	int SetVedio()
	{
		string tmp = CCGIIn::GetCGIStr("attackid");
		uint64_t attackid = 0;
		if (!Convert::StringToUInt64(attackid, tmp))
		{
			PARAM_ERROR_RETURN_MSG("check_attackid_fail");
		}
		if (m_strData.empty())
		{
			PARAM_ERROR_RETURN_MSG("check_data_fail");
		}

		CLogicAttack logicAttack;
		int ret = logicAttack.SetVedio(attackid, m_strData);
		if (ret != 0)
			return ret;

		CGI_SEND_LOG("action=setvedio&uid=%u&attack_id=%llu", m_uid, attackid);
		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiUpload)
