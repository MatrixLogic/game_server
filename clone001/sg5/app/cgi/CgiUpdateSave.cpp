
#include "LogicInc.h"

class CCgiUpdateSave : public CCgiBase
{

public:

	CCgiUpdateSave() : CCgiBase("updatesave")
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

	CGI_DIRECT_PROCESS(UpdateSave)

	int UpdateSave()
	{
		string ip = m_ipstr;
		unsigned uid = 0;
		if (!Json::GetUInt(m_data, "userid", uid) || uid == 0)
		{
			uid = m_uid;
		}
		unsigned worldpos = 0;
		if (!Json::GetUInt(m_data, "world_pos", worldpos))
		{
			//uid = m_uid;
			worldpos = 0;
		}

		CLogicArchive logicArchive;
		int result = logicArchive.UpdateSave(uid, m_uid, worldpos,m_jsonResult);
		if(result != 0)
		{
			return result;
		}

		m_jsonResult["currenttime"] = Time::GetGlobalTime();

		CGI_SEND_LOG("uid=%u&userid=%u", m_uid, uid);

		result = logicArchive.SaveRefresh(m_uid, ip, m_data);

		if (0 != result)
		{
			error_log("save data error! ret=%d", result);
			return result;
		}
		// add by aaron 20121228
		if(IsValidUid(uid))
		{
			CLogicUser logicUser;
			DataUser userBy;
			result = logicUser.GetUserLimit(uid,userBy) ;
			if (0 != result)
			{
				error_log("get_user_limit_failed ret=%d", result);
				return result;
			}
			if((userBy.last_active_time - userBy.last_login_time) % 1800 > 1770)
			{
				string openkey;
				string openid;
				string pf;
				if(!Json::GetString(m_data,"openkey",openkey))
				{
					return 0;
				}
				if(!Json::GetString(m_data,"openid",openid))
				{
					return 0;
				}
				if(!Json::GetString(m_data,"pf",pf))
				{
					return 0;
				}
				OpenPlatform::GetPlatform()->Is_Login(openid,openkey,pf);;
			}
		}
		// add by aaron 20121228
		return R_SUCCESS;
	}

};

CGI_MAIN(CCgiUpdateSave)
