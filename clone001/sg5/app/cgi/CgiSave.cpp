/*
 * CgiXSave.cpp
 *
 *  Created on: 2011-6-1
 *      Author: dada
 */

#include "LogicInc.h"

class CCgiSave : public CCgiBase
{

public:

	CCgiSave() : CCgiBase("save")
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

	CGI_DIRECT_PROCESS(Save)

	int Save()
	{
		//info_log("[save data][uid=%u&m_strData=<%s>]",m_uid,m_strData.c_str());

		unsigned uid = 0;
		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}
		string type;
		Json::GetString(m_data, "type", type);
		CLogicArchive logicArchive;
		int result = logicArchive.Save(uid, m_uid, type, m_data, m_jsonResult);
		if (result != 0)
		{
			return result;
		}

		m_jsonResult["currenttime"] = Time::GetGlobalTime();

		CGI_SEND_LOG("uid=%u&userid=%u&type=%s&lasttime=%u&lastseq=%u", m_uid, uid, type.c_str(), m_jsonResult.isMember("lasttime")?m_jsonResult["lasttime"].asUInt():0, m_jsonResult.isMember("lasttime")?m_jsonResult["lastseq"].asUInt():0);
		return R_SUCCESS;
	}
};

CGI_MAIN(CCgiSave)
