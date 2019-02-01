
#include "LogicInc.h"

class CCgiUpdates : public CCgiBase
{

public:

	CCgiUpdates() : CCgiBase("updates")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_GET_UID);
		SetFeature(CF_CHECK_SESSION);
		SetFeature(CF_CHECK_TIME);
		SetFeature(CF_CHECK_PLATFORM);
		SetFeature(CF_CHECK_HASH);
		SetFeature(CF_CHECK_VERSION);
		SetFeature(CF_APPEND_HASH);
		SetFeature(CF_PARSE_DATA);
		SetFeature(CF_CRYPT_DATA);
		SetFeature(CF_DECRYPT_DATA);
	}

	CGI_DIRECT_PROCESS(Updates)

	int Updates()
	{
		unsigned uid = 0;
		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}

		CLogicArchive logicArchive;
		int result = logicArchive.Updates(uid, m_uid, m_data, m_jsonResult);
		if(result != 0)
		{
			return result;
		}

		m_jsonResult["currenttime"] = Time::GetGlobalTime();

		CGI_SEND_LOG("uid=%u&userid=%u", m_uid, uid);
		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiUpdates)
