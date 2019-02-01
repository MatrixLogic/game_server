#include "LogicInc.h"

class CCgiGate : public CCgiBase
{

public:

	CCgiGate() : CCgiBase("gate")
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
	}

	CGI_ACTION_MAP_BEGIN
//	CGI_SET_ACTION_MAP("getrank", GetRank)
	CGI_SET_ACTION_MAP("load", Load)
	CGI_SET_ACTION_MAP("getarena", GetArena)
	CGI_SET_ACTION_MAP("getinstance", GetInstance)
	CGI_SET_ACTION_MAP("getfight", GetFightArena)
	CGI_SET_ACTION_MAP("updatefight", UpdateFightArena)
	CGI_ACTION_MAP_END
/*
	int GetRank()
	{
		unsigned uid = 0;
		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}
		int gateid = 0;
		if (!Json::GetInt(m_data, "gateid", gateid))
		{
			PARAM_ERROR_RETURN_MSG("check_gateid_fail");
		}
		CLogicGate logicGate;
		int ret = logicGate.GetGateRank(uid, gateid, m_jsonResult);
		if (ret != 0)
			return ret;

		m_jsonResult["currenttime"] = Time::GetGlobalTime();

		CGI_SEND_LOG("action=getrank&uid=%u&gateid=%u&userid=%u", m_uid, gateid, uid);
		return R_SUCCESS;
	}
*/
	int Load()
	{
		unsigned uid = 0;
		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}
		unsigned start = 0;
		unsigned end = 0;
		if (!Json::GetUInt(m_data, "start", start) || !Json::GetUInt(m_data, "end", end))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicGate logicGate;
		int ret = logicGate.Load(uid, start, end, m_jsonResult);
		if (ret != 0)
			return ret;

		m_jsonResult["currenttime"] = Time::GetGlobalTime();

		CGI_SEND_LOG("action=load&uid=%u&userid=%u&start=%u&end=%u", m_uid, uid, start, end);
		return R_SUCCESS;
	}

	int GetArena()
	{
		CLogicArena logicArena;
		int ret = logicArena.GetArenaJson(m_jsonResult["arena"]);
		if (ret != 0)
			return ret;

		m_jsonResult["currenttime"] = Time::GetGlobalTime();

		CGI_SEND_LOG("action=getarena&uid=%u", m_uid);
		return R_SUCCESS;
	}

	int GetInstance()
	{
		CLogicInstance logicInstance;
		int ret = logicInstance.GetInstanceInfoJson(m_uid, m_jsonResult["instance"]);
		if (ret != 0)
			return ret;

		m_jsonResult["currenttime"] = Time::GetGlobalTime();

		CGI_SEND_LOG("action=getinstance&uid=%u", m_uid);
		return R_SUCCESS;
	}

	int GetFightArena()
	{
		CLogicFightArena logicArena;
		int ret = logicArena.GetFightArenaJson(m_jsonResult["arena"]);
		if (ret != 0)
			return ret;

		m_jsonResult["currenttime"] = Time::GetGlobalTime();

		CGI_SEND_LOG("action=getfight&uid=%u", m_uid);
		return R_SUCCESS;
	}
	int UpdateFightArena()
	{
		unsigned userid = 0;
		int win = 0;
		if (!Json::GetUInt(m_data, "userid", userid) || !Json::GetInt(m_data, "win", win))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicFightArena logicArena;
		int ret = logicArena.FightOver(userid, m_uid, (win==1?true:false), m_jsonResult);
		if (ret != 0)
			return ret;
		m_jsonResult["currenttime"] = Time::GetGlobalTime();
		CGI_SEND_LOG("action=updatefight&uid=%u&userid=%u&win=%d",m_uid,userid,win);
		return R_SUCCESS;
	}
};

CGI_MAIN(CCgiGate)
