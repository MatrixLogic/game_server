#include "LogicInc.h"

class CCgiNonLogin : public CCgiBase
{
public:
	CCgiNonLogin() : CCgiBase("nonlogin")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
		SetFeature(CF_CHECK_HASH);
		SetFeature(CF_CHECK_VERSION);
		SetFeature(CF_APPEND_HASH);
		SetFeature(CF_PARSE_DATA);
		SetFeature(CF_CRYPT_DATA);
		SetFeature(CF_DECRYPT_DATA);
	}

	CGI_ACTION_MAP_BEGIN
	CGI_SET_ACTION_MAP("getvedio", GetVedio)
	CGI_ACTION_MAP_END

	int GetVedio()
	{
		uint64_t attackId;
		if(!Json::GetUInt64(m_data, "attackid", attackId))
		{
			PARAM_ERROR_RETURN_MSG("check_attackid_fail");
		}

		string vedioStr;
		CLogicAttack logicAttack;
		int ret = logicAttack.GetVedio(attackId, vedioStr);
		if (ret != 0)
			return ret;
		m_jsonResult["vedio"] = vedioStr;

		CGI_SEND_LOG("action=getvedio&attack_id=%llu", attackId);
		return R_SUCCESS;
	}
};

CGI_MAIN(CCgiNonLogin)
