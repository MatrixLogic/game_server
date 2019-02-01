/*
 * CgiNeighbour.cpp
 *
 *  Created on: 2011-6-9
 *      Author: dada
 */

#include "LogicInc.h"

class CCgiNeighbour : public CCgiBase
{
public:

	CCgiNeighbour() : CCgiBase("neighbour")
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

	CGI_ACTION_DEFAULT_MAP_BEGIN(GetNeighbour)
	CGI_SET_ACTION_DEFAULT_MAP("getinfo", GetInfo)
	CGI_SET_ACTION_DEFAULT_MAP("reset", ResetNeighbours)
	CGI_SET_ACTION_DEFAULT_MAP("getchallenger", GetChallenger)
	CGI_ACTION_DEFAULT_MAP_END

	int GetNeighbour()
	{
		CLogicMap logicMap;
		Json::Value neighbours;
		int result = logicMap.GetMapUsersJson(m_uid, OpenPlatform::GetType(), neighbours);
		if(result != 0)
		{
			return result;
		}

		m_jsonResult["neighbours"] = neighbours;
		m_jsonResult["currenttime"] = Time::GetGlobalTime();

		CGI_SEND_LOG("uid=%u&platform=%d&neighbours=%d", m_uid, OpenPlatform::GetType(), neighbours.size());
		return R_SUCCESS;
	}

	int GetInfo()
	{
		CLogicNeighbour logicNeighbour;
		int ret = logicNeighbour.GetUserInfoJson(m_data["uidlist"], m_jsonResult["userinfo"]);
		if (ret != 0)
			return ret;
		m_jsonResult["currenttime"] = Time::GetGlobalTime();

		CGI_SEND_LOG("action=getinfo&uid=%u", m_uid);
		return R_SUCCESS;
	}

	int ResetNeighbours()
	{
		CLogicMap logicMap;
		Json::Value neighbours;
		int result = logicMap.GetMapUsersJson(m_uid, OpenPlatform::GetType(), neighbours, true);
		if(result != 0)
			return result;

		m_jsonResult["neighbours"] = neighbours;
		m_jsonResult["currenttime"] = Time::GetGlobalTime();

		CGI_SEND_LOG("action=reset&uid=%u&platform=%d&neighbours=%d&coins=%u",
				m_uid, OpenPlatform::GetType(), neighbours.size());
		return R_SUCCESS;
	}

	int GetChallenger()
	{
		CLogicMap logicMap;
		unsigned challenger;
		PlatformType platform = OpenPlatform::GetType();
		int result = logicMap.GetChallenger(m_uid, challenger, platform);
		if(result != 0)
			return result;
		m_jsonResult["challenger"] = challenger;
		CLogicUserBasic logicUserBasic;
		DataUserBasic userBasic;
		result = logicUserBasic.GetUserBasic(challenger, platform, userBasic);
		if (0 != result)
			return result;
		m_jsonResult["name"] = userBasic.name;

		CGI_SEND_LOG("action=GetChallenger&uid=%u&platform=%d&challenger=%u",
						m_uid, OpenPlatform::GetType(), challenger);
		return R_SUCCESS;
	}
};

CGI_MAIN(CCgiNeighbour)
