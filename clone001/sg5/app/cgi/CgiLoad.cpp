/*
 * CgiLoad.cpp
 *
 *  Created on: 2011-6-1
 *      Author: dada
 */

#include "LogicInc.h"

class CCgiLoad : public CCgiBase
{

public:

	CCgiLoad() : CCgiBase("load")
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

	CGI_DIRECT_PROCESS(Load)

	int Load()
	{
		unsigned uid = 0;
		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}
/****************允许跨服*********************
		//防止跨服
		if (IsValidUid(uid) && uid != m_uid)
		{
			int serverid1 = (int)((m_uid - 10000000) / 500000);
			int serverid2 = (int)((uid - 10000000) / 500000);
			if (serverid1 != serverid2)
			{
				error_log("[not same server][uid=%u,userid=%u]",uid,m_uid);
				PARAM_ERROR_RETURN_MSG("not_same_server");
			}
		}
********************************************/
		/*if (IsValidUid(uid) && uid != m_uid)
		{
			int serverid1 = 0;
			Config::GetDomain(serverid1);
			int serverid2 = MainConfig::GetMergedDomain((int)((uid - 10000000) / 500000 + 1));
			if (serverid1 != serverid2)
			{
				error_log("[not same server][uid=%u,userid=%u]",uid,m_uid);
				PARAM_ERROR_RETURN_MSG("not_same_server");
			}
		}*/
		m_jsonResult["uid"] = uid;

		string type;
		Json::GetString(m_data, "type", type);
		CLogicArchive logicArchive;
		int ret = logicArchive.Load(uid, m_uid, type, m_data, m_jsonResult);
		if(ret != 0)
			return ret;

		m_jsonResult["currenttime"] = Time::GetGlobalTime();

		CGI_SEND_LOG("uid=%u&userid=%u&type=%s",m_uid,uid,type.c_str());
		return R_SUCCESS;
	}
};

CGI_MAIN(CCgiLoad)
