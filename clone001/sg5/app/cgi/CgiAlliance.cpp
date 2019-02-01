/*
 * CgiAlliance.cpp
 *
 *  Created on: 2011-9-21
 *      Author: dada
 */

#include "LogicInc.h"

class CCgiAlliance : public CCgiBase
{

public:

	CCgiAlliance() : CCgiBase("alliance")
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

	CGI_ACTION_MAP_BEGIN
	CGI_SET_ACTION_MAP("AddAlliance", AddAlliance)
	CGI_SET_ACTION_MAP("SetAlliance", SetAlliance)
	CGI_SET_ACTION_MAP("GetAlliance", GetAlliance)
	CGI_SET_ACTION_MAP("SearchAlliance", SearchAlliance)
	CGI_SET_ACTION_MAP("GetAlliances", GetAlliances)
	CGI_SET_ACTION_MAP("GetAllianceEnemys", GetAllianceEnemys)
	CGI_SET_ACTION_MAP("RemoveAlliance", RemoveAlliance)
	CGI_SET_ACTION_MAP("RemoveMember", RemoveMember)
	CGI_SET_ACTION_MAP("Donate", Donate)
	CGI_SET_ACTION_MAP("SetData", SetData)
	CGI_SET_ACTION_MAP("SetAllianceBoss", SetAllianceBoss)
	CGI_SET_ACTION_MAP("setmemberdata", SetMemberData)
	CGI_SET_ACTION_MAP("joinalliance", JoinAlliance)
	CGI_SET_ACTION_MAP("allianceemail", AllianceEmail)
	CGI_SET_ACTION_MAP("Upgrade", Upgrade)
	CGI_SET_ACTION_MAP("SetMemberType", SetMemberType)
	CGI_SET_ACTION_MAP("nominatealliance", NominateAlliance)
	CGI_SET_ACTION_MAP("viewmatch", ViewMatch)
	CGI_SET_ACTION_MAP("applymatch", ApplyMatch)
	CGI_SET_ACTION_MAP("reportmatch", ReportMatch)
	CGI_SET_ACTION_MAP("viewapplyers", ViewApplyers)
	CGI_SET_ACTION_MAP("viewregular", ViewRegular)
	CGI_SET_ACTION_MAP("viewbasematch", ViewBaseMatch)
	CGI_SET_ACTION_MAP("viewallsereverbasematch", ViewAllServerBaseMatch)
	CGI_SET_ACTION_MAP("applybasematch", ApplyBaseMatch)
	CGI_SET_ACTION_MAP("applyallserverbasematch", ApplyAllServerBaseMatch)
	CGI_SET_ACTION_MAP("reportbasematch", ReportBaseMatch)
	CGI_SET_ACTION_MAP("reportallserverbasematch", ReportAllServerBaseMatch)
	CGI_SET_ACTION_MAP("viewbaseapplyers", ViewBaseApplyers)
	CGI_SET_ACTION_MAP("viewallserverbaseapplyers", ViewAllServerBaseApplyers)
	CGI_SET_ACTION_MAP("viewbaseregular", ViewBaseRegular)
	CGI_SET_ACTION_MAP("viewallserverbaseregular", ViewAllServerBaseRegular)
	CGI_SET_ACTION_MAP("viewpersonmatch", ViewPersonMatch)
	CGI_SET_ACTION_MAP("viewallserverpersonmatch", ViewAllServerPersonMatch)
	CGI_SET_ACTION_MAP("applypersonmatch", ApplyPersonMatch)
	CGI_SET_ACTION_MAP("applyallserverpersonmatch", ApplyAllServerPersonMatch)
	CGI_SET_ACTION_MAP("reportpersonmatch", ReportPersonMatch)
	CGI_SET_ACTION_MAP("reporallservertpersonmatch", ReportAllServerPersonMatch)
	CGI_SET_ACTION_MAP("guessapply", GuessApply)
	CGI_SET_ACTION_MAP("allserverguessapply", AllServerGuessApply)
	CGI_SET_ACTION_MAP("guessview", GuessView)
	CGI_SET_ACTION_MAP("allserverguessview", AllServerGuessView)
	CGI_SET_ACTION_MAP("getnationalwarrgtdata", GetNationalWarrgtData)
	CGI_SET_ACTION_MAP("updatenationalwarrgtdata", UpdateNationalWarrgtdata)
	CGI_SET_ACTION_MAP("invitealliancenationalwarrgtdata", InviteAllianceNationalWarrgtdata)
	CGI_SET_ACTION_MAP("getgroupsfightingjsondata", GetGroupsFightingData)
	CGI_SET_ACTION_MAP("updatasgroupsfightingdata", UpdatasGroupsFightingData)
	CGI_SET_ACTION_MAP("addfullmorale", AddFullmorale)



	CGI_ACTION_MAP_END

	int AddAlliance()
	{
		int ret;
		string name;
		if(!Json::GetString(m_data, "name", name))
		{
			return R_ERR_PARAM;
		}
		int flag;
		if(!Json::GetInt(m_data, "flag", flag))
		{
			return R_ERR_PARAM;
		}
		string description;
		if(!Json::GetString(m_data, "desc", description))
		{
			return R_ERR_PARAM;
		}
		StringFilter::Replace(description);
		int cash = 0;
		Json::GetInt(m_data, "cash", cash);
		if(cash < 0)
		{
			return R_ERR_PARAM;
		}

		int coins = 0;
		Json::GetInt(m_data, "coins", coins);
		if (coins < 0)
		{
			return R_ERR_PARAM;
		}

		int useres = 1;
		Json::GetInt(m_data, "useres", useres);

		CLogicAlliance logicAlliance;
		DataAlliance alliance;
		ret = logicAlliance.AddAlliance(m_uid, name, flag, description, cash, coins, useres != 0, alliance);
		if(ret != 0)
		{
			return ret;
		}
		ret = logicAlliance.GetAllianceJson(m_uid, alliance.alliance_id, m_jsonResult);
		if(ret != 0)
		{
			return ret;
		}
		CGI_SEND_LOG("action=AddAlliance&uid=%u&aid=%u&name=%s&cash=%d&useres=%d",
				m_uid, alliance.alliance_id, name.c_str(), cash, useres);
		return R_SUCCESS;
	}

	int SetAlliance()
	{
		int ret;
		unsigned allianceId;
		if(!Json::GetUInt(m_data, "aid", allianceId))
		{
			return R_ERR_PARAM;
		}
		int flag = -1;
		bool bFlag = Json::GetInt(m_data, "flag", flag);
		string desc;
		bool bDesc = Json::GetString(m_data, "desc", desc);
		string notice;
		bool bNotice = Json::GetString(m_data, "notice", notice);
		unsigned leader = 0;
		bool bLeader = Json::GetUInt(m_data, "leader", leader);
		unsigned leaderType = 0;
		bool bLeaderType = Json::GetUInt(m_data, "type", leaderType);
		int sign;
		bool bsign = Json::GetInt(m_data, "sign", sign);
		if(!bFlag && !bDesc && !bNotice && !bLeader && !bLeaderType && !bsign)
		{
			return R_ERR_PARAM;
		}

		DataAlliance alliance;
		CLogicAlliance logicAlliance;
		if(!bDesc && !bNotice)
		{
			ret = logicAlliance.GetAllianceLimit(allianceId, alliance);
		}
		else
		{
			ret = logicAlliance.GetAlliance(allianceId, alliance);
		}
		if(ret != 0)
		{
			return ret;
		}
		if(alliance.leader_uid != m_uid && !logicAlliance.IsManager(m_uid, allianceId))
		{
			error_log("[is_not_leader][aid=%u,uid=%u]", allianceId, m_uid);
			ERROR_RETURN_MSG(R_ERR_LOGIC, "is_not_leader");
		}
		if(bFlag)
		{
			alliance.flag = flag;
		}
		if(bDesc)
		{
			alliance.description = desc;
			StringFilter::Replace(alliance.description);
		}
		if(bNotice)
		{
			alliance.notice = notice;
			StringFilter::Replace(alliance.notice);
		}
		if(!bDesc && !bNotice)
		{
			ret = logicAlliance.SetAllianceLimit(alliance);
		}
		else
		{
			ret = logicAlliance.SetAlliance(alliance);
		}
		if(ret != 0)
		{
			return ret;
		}
		if(!bLeaderType && bLeader && leader != alliance.leader_uid)
		{
			ret = logicAlliance.ChangeLeader(allianceId, leader);
			if(ret != 0)
			{
				return ret;
			}
		}
		if(bLeaderType && bLeader)
		{
			ret = logicAlliance.CompeteLaeder(allianceId, leader);
			if(ret != 0)
			{
				return ret;
			}
		}
		if (bsign)
		{
			CLogicNominateAlliance nAlliance;
			ret = nAlliance.SetAlliance(allianceId, sign);
			if (ret != 0)
			{
				return ret;
			}
		}
		CGI_SEND_LOG("action=SetAlliance&uid=%u&aid=%u&flag=%d&desc=%d&notice=%d&leader=%u", m_uid, allianceId,
				flag, bDesc ? (int)desc.size() : -1, bNotice ? (int)notice.size() : -1, leader);
		return R_SUCCESS;
	}

	int GetAlliance()
	{
		int ret;
		unsigned allianceId = 0;
		unsigned userid = 0;
		Json::GetUInt(m_data, "aid", allianceId);
		Json::GetUInt(m_data, "userid", userid);
		if (allianceId == 0)
		{
			if (IsValidUid(userid))
			{
				DataUser user;
				CLogicUser logicUser;
				ret = logicUser.GetUserLimit(userid, user);
				if (ret != 0)
					return ret;
				allianceId = user.alliance_id;
			}
		}
		if (!IsAllianceId(allianceId))
		{
			ERROR_RETURN_MSG(R_ERR_NO_DATA, "alliance_not_exist");
		}
		CLogicAlliance logicAlliance;
		ret = logicAlliance.GetAllianceJson(m_uid, allianceId, m_jsonResult);
		if(ret != 0)
		{
			return ret;
		}
		CGI_SEND_LOG("action=GetAlliance&uid=%u&aid=%u", m_uid, allianceId);
		return R_SUCCESS;
	}

	int SearchAlliance()
	{
		int ret;
		string name;
		if(!Json::GetString(m_data, "name", name))
		{
			return R_ERR_PARAM;
		}
		if(name.empty())
		{
			return R_ERR_PARAM;
		}
		CLogicAlliance logicAlliance;
		ret = logicAlliance.SearchAllianceJson(name, m_jsonResult);
		if(ret != 0)
		{
			return ret;
		}
		CGI_SEND_LOG("action=SearchAlliance&uid=%u&name=%s", m_uid, name.c_str());
		return R_SUCCESS;
	}

	int GetAlliances()
	{
		int ret;
		CLogicAlliance logicAlliance;
		ret = logicAlliance.GetRandomAlliancesJson(m_jsonResult["alliances"]);
		if(ret != 0)
		{
			return ret;
		}
		CGI_SEND_LOG("action=GetAlliances&uid=%u&count=%u", m_uid, (unsigned)m_jsonResult.size());
		return R_SUCCESS;
	}

	int GetAllianceEnemys()
	{
		int ret;
		unsigned allianceId;
		if(!Json::GetUInt(m_data, "aid", allianceId))
		{
			return R_ERR_PARAM;
		}
		unsigned startIndex;
		if(!Json::GetUInt(m_data, "start", startIndex))
		{
			return R_ERR_PARAM;
		}
		unsigned count;
		if(!Json::GetUInt(m_data, "count", count))
		{
			return R_ERR_PARAM;
		}

		CLogicAlliance logicAlliance;
		ret = logicAlliance.GetAllianceEnemys(m_uid, allianceId, startIndex, count, m_jsonResult);
		if(ret != 0)
		{
			return ret;
		}
		CGI_SEND_LOG("action=GetAllianceEnemys&uid=%u&aid=%u&start=%u&count=%u&rcount=%u",
				m_uid, allianceId, startIndex, count, (unsigned)m_jsonResult["enemys"].size());
		return R_SUCCESS;
	}

	int RemoveAlliance()
	{
		int ret;
		unsigned allianceId;
		if(!Json::GetUInt(m_data, "aid", allianceId))
		{
			return R_ERR_PARAM;
		}
		DataAlliance alliance;
		CLogicAlliance logicAlliance;
		ret = logicAlliance.GetAllianceLimit(allianceId, alliance);
		if(ret != 0)
		{
			return ret;
		}
		if(alliance.leader_uid != m_uid)
		{
			error_log("[is_not_leader][aid=%u,uid=%u,luid=%u]", allianceId, m_uid, alliance.leader_uid);
			ERROR_RETURN_MSG(R_ERR_LOGIC, "is_not_leader");
		}
		ret = logicAlliance.RemoveAlliance(allianceId, false);
		if(ret != 0)
		{
			return ret;
		}
		m_jsonResult["etime"] = Time::GetGlobalTime();
		CGI_SEND_LOG("action=RemoveAlliance&uid=%u&aid=%u", m_uid, alliance.alliance_id);
		return R_SUCCESS;
	}

	int RemoveMember()
	{
		int ret;
		unsigned allianceId;
		if(!Json::GetUInt(m_data, "aid", allianceId))
		{
			return R_ERR_PARAM;
		}
		unsigned removeUid;
		if(!Json::GetUInt(m_data, "remove", removeUid))
		{
			return R_ERR_PARAM;
		}
		CLogicAlliance logicAlliance;
		ret = logicAlliance.RemoveMember(m_uid, allianceId, removeUid);
		if(ret != 0)
		{
			return ret;
		}
		CGI_SEND_LOG("action=RemoveMember&uid=%u&aid=%u&ruid=%u", m_uid, allianceId, removeUid);
		return R_SUCCESS;
	}

	int Donate()
	{
		//return R_ERR_REFUSE;
		int ret;
		unsigned allianceId;
		if(!Json::GetUInt(m_data, "aid", allianceId))
		{
			return R_ERR_PARAM;
		}
		unsigned r1;
		unsigned r2;
		unsigned r3;
		unsigned r4;
		unsigned r5 = 0;
		if( !Json::GetUInt(m_data, "r1", r1) ||
			!Json::GetUInt(m_data, "r2", r2) ||
			!Json::GetUInt(m_data, "r3", r3) ||
			!Json::GetUInt(m_data, "r4", r4)  )
		{
			return R_ERR_PARAM;
		}
		unsigned coins;
		unsigned cash;
		bool bCoins = false;
		bool bCash = false;
		bCoins = Json::GetUInt(m_data, "coins", coins);
		bCash = Json::GetUInt(m_data, "cash", cash);
		if (!bCoins && !bCash)
		{
			return R_ERR_PARAM;
		}

		CLogicAlliance logicAlliance;
		if (bCoins)
		{
			ret = logicAlliance.Donate(m_uid, allianceId, r1, r2, r3, r4, r5, coins);
		}
		else
		{
			ret = logicAlliance.Donate(m_uid, allianceId, r1, r2, r3, r4, r5, cash, false);
		}

		if(ret != 0)
		{
			return ret;
		}
		CGI_SEND_LOG("action=Donate&uid=%u&aid=%u&r1=%u&r2=%u&r3=%u&r4=%u&r5=%u&coin=%u",
				m_uid, allianceId, r1, r2, r3, r4, r5, bCoins?coins:cash);
		return R_SUCCESS;
	}

	int SetData()
	{
		//return R_ERR_REFUSE;
		int ret;
		unsigned allianceId;
		if(!Json::GetUInt(m_data, "aid", allianceId))
		{
			return R_ERR_PARAM;
		}
		bool isManager = false;
		CLogicAlliance logicAlliance;
		if(Json::IsObject(m_data, "resource"))
		{
			Json::Value &resource = m_data["resource"];
			int r1 = 0;
			int r2 = 0;
			int r3 = 0;
			int r4 = 0;
			int r5 = 0;
			int cash = 0;
			Json::GetInt(resource, "r1", r1);
			Json::GetInt(resource, "r2", r2);
			Json::GetInt(resource, "r3", r3);
			Json::GetInt(resource, "r4", r4);
			Json::GetInt(resource, "r5", r5);
			Json::GetInt(resource, "cash", cash);
			if(r1 > 0 || r2 > 0 || r3 > 0 || r4 > 0 || r5 > 0 || cash > 0)
			{
				error_log("[change_resource_error][uid=%u,r1=%d,r2=%d,r3=%d,r4=%d,r5=%d,cash=%d]",
						m_uid, r1, r2, r3, r4, r5, cash);
				return R_ERR_PARAM;
			}
			if(r1 != 0 || r2 != 0 || r3 != 0 || r4 != 0 || r5 != 0 || cash != 0)
			{
				if(!logicAlliance.IsManager(m_uid, allianceId))
				{
					error_log("[is_not_leader][aid=%u,uid=%u]", allianceId, m_uid);
					ERROR_RETURN_MSG(R_ERR_LOGIC, "is_not_leader");
				}
				isManager = true;
				ret  = logicAlliance.ChangeResource(allianceId, r1, r2, r3, r4, r5, cash, "ALUSE");
				if(ret != 0)
				{
					return ret;
				}
			}
		}
		if(Json::IsObjectOrArray(m_data, "memberdata"))
		{
			ret = logicAlliance.SetMemberData(m_uid, allianceId, m_data["memberdata"]);
			if(ret != 0)
			{
				return ret;
			}
		}
		if(Json::IsObjectOrArray(m_data, "alliancedata"))
		{
			if(!isManager && !logicAlliance.IsManager(m_uid, allianceId))
			{
				error_log("[is_not_leader][aid=%u,uid=%u]", allianceId, m_uid);
				ERROR_RETURN_MSG(R_ERR_LOGIC, "is_not_leader");
			}
			ret = logicAlliance.SetAllianceData(m_uid, allianceId, m_data);
			if(ret != 0)
			{
				return ret;
			}
		}
		CGI_SEND_LOG("action=SetData&uid=%u&allianceId=%u", m_uid, allianceId);
		return 0;
	}

	int SetAllianceBoss()
	{

		if(Json::IsObject(m_data, "alliancedata"))
		{
			unsigned allianceId;
			if(!Json::GetUInt(m_data, "aid", allianceId))
			{
				return R_ERR_PARAM;
			}

			Json::Value &resource = m_data["alliancedata"]["allianceboss"];
			int ts_start = 0;
			int vip = 0;
			string name;
			if(!Json::GetInt(resource, "ts", ts_start) || !Json::GetInt(resource, "vip", vip) )
			{
				return R_ERR_PARAM;
			}
			CLogicAllianceBoss logicAllianceBoss;
			CLogicAlliance logicAlliance;
			int ret = logicAllianceBoss.GetAllianceBossInfo1(m_uid, m_jsonResult["allianceboss"],vip,ts_start);
			if (ret != 0)
				return ret;
			int r = logicAlliance.SetAllianceData(m_uid, allianceId, m_data);
			if (r != 0)
				return r;
			CGI_SEND_LOG("action=allianceboss&uid=%u&vip=%u",m_uid,vip);

		}

		return R_SUCCESS;

	}

	int SetMemberData()
	{
		int ret;
		unsigned allianceId;
		if(!Json::GetUInt(m_data, "aid", allianceId))
		{
			return R_ERR_PARAM;
		}

		int point = 0;
		if(!Json::GetInt(m_data, "point", point))
		{
			return R_ERR_PARAM;
		}

		CLogicAlliance logicAlliance;
		ret = logicAlliance.SetAllianceMemberData(m_uid,allianceId,point,m_data["data"]);
		if (0 != ret)
		{
			return ret;
		}

		CGI_SEND_LOG("action=SetMemberData&uid=%u&allianceId=%u", m_uid, allianceId);
		return 0;
	}

	int JoinAlliance()
	{
		unsigned allianceId = 0;
		if(!Json::GetUInt(m_data, "aid", allianceId))
		{
			return R_ERR_PARAM;
		}

		unsigned uid = 0;
		if(!Json::GetUInt(m_data, "userid", uid))
		{
			return R_ERR_PARAM;
		}

		CLogicAlliance logicAlliance;
		int ret = logicAlliance.AddMember(allianceId, uid);
		if (0 != ret)
		{
			return ret;
		}

		CGI_SEND_LOG("action=JoinAlliance&uid=%u&allianceId=%u", uid, allianceId);
		return 0;
	}

	int AllianceEmail()
	{
		unsigned allianceId = 0;
		if(!Json::GetUInt(m_data, "aid", allianceId))
		{
			return R_ERR_PARAM;
		}
		DataEmail data;
		if(!Json::GetString(m_data,"title",data.title))
		{
			return R_ERR_PARAM;
		}
		if(!Json::GetString(m_data,"text",data.text))
		{
			return R_ERR_PARAM;
		}
		data.text = "{\"text\":\"" + data.text + "\",\"vt\":0,\"vl\":0}";
		data.attach_flag = 0;
		data.post_ts = Time::GetGlobalTime();
		data.read_ts = 0;
		CLogicAlliance logicAlliance;
		int ret = logicAlliance.AllianceEmail(allianceId, m_uid, data);
		if (0 != ret)
		{
			return ret;
		}

		CGI_SEND_LOG("action=AllianceEmail&uid=%u&allianceId=%u", m_uid, allianceId);
		return 0;
	}

	int Upgrade()
	{
		//return R_ERR_REFUSE;
		int ret;
		unsigned allianceId;
		if(!Json::GetUInt(m_data, "aid", allianceId))
		{
			return R_ERR_PARAM;
		}
		uint64_t r1;
		uint64_t r2;
		uint64_t r3;
		uint64_t r4;
		uint64_t r5;
		if( !Json::GetUInt64(m_data, "r1", r1) ||
			!Json::GetUInt64(m_data, "r2", r2) ||
			!Json::GetUInt64(m_data, "r3", r3) ||
			!Json::GetUInt64(m_data, "r4", r4) ||
			!Json::GetUInt64(m_data, "r5", r5) )
		{
			return R_ERR_PARAM;
		}
		unsigned cash;
		if(!Json::GetUInt(m_data, "cash", cash))
		{
			return R_ERR_PARAM;
		}
		int level;
		if(!Json::GetInt(m_data, "level", level))
		{
			return R_ERR_PARAM;
		}
		CLogicAlliance logicAlliance;
		ret = logicAlliance.Upgrade(m_uid, allianceId, r1, r2, r3, r4, r5, cash, level);
		if(ret != 0)
		{
			return ret;
		}
		m_jsonResult["level"] = level;
		CGI_SEND_LOG("action=Upgrade&uid=%u&aid=%u&r1=%llu&r2=%llu&r3=%llu&r4=%llu&r5=%llu&cash=%u&newlevel=%d",
				m_uid, allianceId, r1, r2, r3, r4, r5, cash, level);
		return R_SUCCESS;
	}

	int SetMemberType()
	{
		int ret;
		unsigned allianceId;
		if(!Json::GetUInt(m_data, "aid", allianceId))
		{
			return R_ERR_PARAM;
		}
		unsigned memberUid;
		if(!Json::GetUInt(m_data, "member", memberUid))
		{
			return R_ERR_PARAM;
		}
		int type;
		if(!Json::GetInt(m_data, "type", type))
		{
			return R_ERR_PARAM;
		}
		CLogicAlliance logicAlliance;
		ret = logicAlliance.SetMemberType(m_uid, allianceId, memberUid, type);
		if(ret != 0)
		{
			error_log("[SetMemberType fail][result=%d,uid=%u,aid=%u,type=%d]",
					ret, m_uid, allianceId, type);
			return ret;
		}
		CGI_SEND_LOG("action=SetMemberType&uid=%u&aid=%u&member=%u&type=%d", m_uid, allianceId, memberUid, type);
		return R_SUCCESS;
	}

	int ViewMatch()
	{
		CLogicMatch logicMatch;
		unsigned aid = 0;
		if (!Json::GetUInt(m_data, "aid", aid))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		int ret = logicMatch.GetMatchInfo(aid, m_uid, m_jsonResult);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=viewmatch&uid=%u&aid=%u", m_uid, aid);
		return R_SUCCESS;
	}

	int ApplyMatch()
	{
		CLogicMatch logicMatch;
		int ret = logicMatch.Apply(m_uid);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=applymatch&uid=%u", m_uid);
		return R_SUCCESS;
	}

	int ReportMatch()
	{
		CLogicMatch logicMatch;
		int order = 0;
		int result = 0;
		if (!Json::GetInt(m_data, "order", order) || !Json::GetInt(m_data, "result", result))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		int ret = logicMatch.ReportResult(m_uid, order, result);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=reportmatch&uid=%u&order=%d&result=%d", m_uid, order, result);
		return R_SUCCESS;
	}

	int ViewApplyers()
	{
		unsigned aid = 0;
		if (!Json::GetUInt(m_data, "aid", aid))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicMatch logicMatch;
		int ret = logicMatch.GetApplyPlayers(aid, m_jsonResult);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=viewapplyers&uid=%u", m_uid);
		return R_SUCCESS;
	}

	int ViewRegular()
	{
		unsigned aid = 0;
		if (!Json::GetUInt(m_data, "aid", aid))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicMatch logicMatch;
		int ret = logicMatch.GetRegularScore(aid, m_jsonResult);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=viewregular&uid=%u", m_uid);
		return R_SUCCESS;
	}

	int ViewBaseMatch()
	{
		CLogicBaseMatch logicBaseMatch;
		unsigned aid = 0;
		if (!Json::GetUInt(m_data, "aid", aid))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		int ret = logicBaseMatch.GetBaseMatchInfo(aid, m_uid, m_jsonResult);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=viewbasematch&uid=%u&aid=%u", m_uid, aid);
		return R_SUCCESS;
	}

	int ViewAllServerBaseMatch()
	{
		int ret = 0;
		string aid;
		if (!Json::GetString(m_data, "aid", aid))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		string url = "action=viewbasematch";
		url.append("&aid=").append(aid);
		string uid;
		String::Format(uid,"%u",m_uid);
		url.append("&uid=");
		url += uid;

		Json::Value Result;
		CLogicBaseMatch logicBaseMatch;
		ret = logicBaseMatch.RequestBaseMatch(url, Result);
		if (0 != ret)
		{
			return ret;
		}

		m_jsonResult["apply"] = Result["apply"];
		m_jsonResult["stage"] = Result["stage"];
		m_jsonResult["number"] = Result["number"];
		m_jsonResult["status"] = Result["status"];
		m_jsonResult["teams"] = Result["teams"];
		m_jsonResult["fights"] = Result["fights"];
		m_jsonResult["rivals"] = Result["rivals"];

		CGI_SEND_LOG("action=viewallserverbasematch&uid=%u&aid=%s", m_uid, aid.c_str());
		return 0;
	}

	int ApplyBaseMatch()
	{
		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.Apply(m_uid);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=applybasematch&uid=%u", m_uid);
		return R_SUCCESS;
	}

	int ApplyAllServerBaseMatch()
	{
		string url = "action=applybasematch";
		string uid;
		String::Format(uid,"%u",m_uid);
		url.append("&uid=");
		url += uid;

		Json::Value result;
		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.RequestBaseMatch(url,result);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=applyallserverbasematch&uid=%u", m_uid);
		return 0;
	}

	int ReportBaseMatch()
	{
		CLogicBaseMatch logicBaseMatch;
		int order = 0;
		int result = 0;
		if (!Json::GetInt(m_data, "order", order) || !Json::GetInt(m_data, "result", result))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		int ret = logicBaseMatch.ReportResult(m_uid, order, result);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=reportbasematch&uid=%u&order=%d&result=%d", m_uid, order, result);
		return R_SUCCESS;
	}

	int ReportAllServerBaseMatch()
	{
		CLogicBaseMatch logicBaseMatch;
		string order;
		string result;
		if (!Json::GetString(m_data, "order", order) || !Json::GetString(m_data, "result", result))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		string url = "action=reportbasematch";
		string uid;
		String::Format(uid,"%u",m_uid);
		url.append("&uid=");
		url += uid;
		url.append("&order=");
		url += order;
		url.append("&result=");
		url += result;
		Json::Value Request;
		int ret = logicBaseMatch.RequestBaseMatch(url, Request);
		if (0 != ret)
		{
			return ret;
		}
		CGI_SEND_LOG("action=reportallserverbasematch&uid=%u&order=%s&result=%s", m_uid, order.c_str(), result.c_str());
		return 0;
	}

	int ViewBaseApplyers()
	{
		unsigned aid = 0;
		if (!Json::GetUInt(m_data, "aid", aid))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.GetApplyPlayers(aid, m_jsonResult);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=viewbaseapplyers&uid=%u", m_uid);
		return R_SUCCESS;
	}

	int ViewAllServerBaseApplyers()
	{
		string aid;
		if (!Json::GetString(m_data, "aid", aid))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		string url = "action=viewbaseapplyers";
		url.append("&aid=");
		url += aid;

		Json::Value Response;
		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.RequestBaseMatch(url,Response);
		if (0 != ret)
		{
			return ret;
		}
		m_jsonResult["players"] = Response["players"];
		return 0;
	}

	int ViewBaseRegular()
	{
		unsigned aid = 0;
		if (!Json::GetUInt(m_data, "aid", aid))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.GetRegularScore(aid, m_jsonResult);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=viewbaseregular&uid=%u", m_uid);
		return R_SUCCESS;
	}

	int ViewAllServerBaseRegular()
	{
		string aid;
		if (!Json::GetString(m_data, "aid", aid))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		string url = "action=viewbaseregular";
		url.append("&aid=");
		url += aid;

		Json::Value Response;
		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.RequestBaseMatch(url,Response);
		if (0 != ret)
		{
			return ret;
		}
		m_jsonResult["rank"] = Response["rank"];
		m_jsonResult["score"] = Response["score"];
		m_jsonResult["players"] = Response["players"];
		CGI_SEND_LOG("action=viewallserverbaseregular&uid=%u", m_uid);
		return 0;
	}

	int ViewPersonMatch(){
		CLogicPersonMatch logicPersonMatch;

		int ret = logicPersonMatch.GetBaseMatchInfo(m_uid,m_jsonResult);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=viewpersonmatch&uid=%u", m_uid);
		
		return R_SUCCESS;
	}

	int ViewAllServerPersonMatch()
	{
		string level;
		if (!Json::GetString(m_data, "level", level))
		{
			PARAM_ERROR_RETURN_MSG("param_error_level");
		}
		string url = "action=viewpersonmatch";
		string uid;
		String::Format(uid, "%u", m_uid);
		url.append("&uid=").append(uid);
		url.append("&level=").append(level);
		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.RequestBaseMatch(url, m_jsonResult);
		if (0 != ret)
		{
			return ret;
		}
		return 0;
	}

	int ApplyPersonMatch(){
		CLogicPersonMatch logicPersonMatch;
		int ret = logicPersonMatch.Apply(m_uid);

		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=applypersonmatch&uid=%u", m_uid);
		
		return R_SUCCESS;
	}

	int ApplyAllServerPersonMatch()
	{
		string level;
		if (!Json::GetString(m_data, "level", level))
		{
			PARAM_ERROR_RETURN_MSG("param_error_level");
		}

		string url = "action=applypersonmatch";
		string uid;
		String::Format(uid, "%u", m_uid);
		url.append("&uid=");
		url += uid;
		url.append("&level=").append(level);

		Json::Value Response;
		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.RequestBaseMatch(url, Response);
		if (0 != ret)
		{
			return ret;
		}
		return 0;
	}

	int ReportPersonMatch(){
		CLogicPersonMatch logicPersonMatch;
		int order = 0;
		int result = 0;
		if (!Json::GetInt(m_data, "order", order) || !Json::GetInt(m_data, "result", result))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		int ret = logicPersonMatch.ReportResult(m_uid, order, result);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=reportpersonmatch&uid=%u&order=%d&result=%d", m_uid, order, result);
		return R_SUCCESS;
	}

	int ReportAllServerPersonMatch()
	{
		string order;
		string result;
		string level;

		if (   !Json::GetString(m_data, "order", order)
			|| !Json::GetString(m_data, "result", result)
			|| !Json::GetString(m_data, "level", level))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		string url = "action=reportpersonmatch";
		string uid;
		String::Format(uid, "%u", m_uid);
		url.append("&uid=");
		url += uid;
		url.append("&order=");
		url += order;
		url.append("&result=");
		url += result;
		url.append("&level=").append(level);

		Json::Value Response;
		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.RequestBaseMatch(url, Response);
		if (0 != ret)
		{
			return ret;
		}
		return 0;
	}

	int GuessApply(){
		unsigned gid = 0;
		unsigned coins = 0;
		int type;
		if (!Json::GetUInt(m_data, "gid", gid) || !Json::GetUInt(m_data, "coins", coins) || coins <= 0
				|| !Json::GetInt(m_data,"type",type) || type > 2 || type < 0)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CGI_SEND_LOG("action=guessapply&uid=%u&gid=%d&coins=%d", m_uid, gid, coins);

		int stage = 10;
		if(type == 0){
			CLogicMatch match;
			if(match.GetStage(stage) != 0){
				error_log("get stage failed");
				return R_ERR_LOGIC;
			}
		}
		else if(type == 1){
			CLogicBaseMatch match;
			if(match.GetStage(stage) != 0){
				error_log("get stage failed");
				return R_ERR_LOGIC;
			}
		}
		else {
			CLogicPersonMatch match;
			if(match.GetStage(stage) != 0){
				error_log("get stage failed");
				return R_ERR_LOGIC;
			}
		}

		if(stage != 10){
			error_log("stage expect 10 but %d",stage);
			return R_ERR_LOGIC;
		}

		CLogicPay logicPay;
		//unsigned cash;
		DataPay data_pay;
		if(logicPay.GetPay(m_uid,data_pay) != 0){
			error_log("get %u's cash failed",m_uid);
			return R_ERR_DB;
		}
		if(data_pay.coins < unsigned(coins * PER_BET_COINS)){
			error_log("user %u's coins not enough[%u,%u]",m_uid,data_pay.coins,coins * PER_BET_COINS);
			return R_ERR_LOGIC;
		}

		CGuessData guessData;
		if(guessData.Init(Config::GetValue(GUESS_DATA_PATH),type) != 0){
			error_log("GuessApply init guess data failed");
			return R_ERR_LOGIC;
		}
		if(guessData.Apply(m_uid,gid,coins) != 0){
			error_log("user %u apply guess failed [%u %u]", m_uid, gid, coins);
			return R_ERR_LOGIC;
		}


		logicPay.ChangePay(m_uid,0,coins*-1*PER_BET_COINS,"MATCHGUESSAPPLY",1);
		return R_SUCCESS;
	}

	int AllServerGuessApply()
	{
		string gid;
		string coins;
		string type;
		if (   !Json::GetString(m_data, "gid", gid)
			|| !Json::GetString(m_data, "coins", coins)
			|| !Json::GetString(m_data,"type",type))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		string url = "action=guessapply";
		string uid;
		String::Format(uid, "%u", m_uid);
		url.append("&uid=");
		url += uid;
		url.append("&gid=");
		url += gid;
		url.append("&coins=");
		url += coins;
		url.append("&type=");
		url += type;
		string level;
		if (Json::GetString(m_data, "level", level))
		{
			url.append("&level=").append(level);
		}
		Json::Value Request;
		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.RequestBaseMatch(url, Request);
		if (0 != ret)
		{
			return ret;
		}
		CGI_SEND_LOG("action=guessapply&uid=%u&gid=%s&coins=%s", m_uid, gid.c_str(), coins.c_str());
		return 0;
	}

	int GuessView(){
		int type ;
		if(!Json::GetInt(m_data,"type",type) || type > 2 || type < 0){
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CGuessData guessData;
		if(guessData.Init(Config::GetValue(GUESS_DATA_PATH),type) != 0){
			error_log("GuessView init guess data failed");
			return R_ERR_LOGIC;
		}

		vector<GuessViewData> dataVec;
		if(guessData.View(m_uid,dataVec) != 0){
			error_log("GuessView view failed");
			return R_ERR_LOGIC;
		}

		m_jsonResult["teams"].resize(dataVec.size());
		for(unsigned i = 0; i < dataVec.size();++i){
			m_jsonResult["teams"][i]["gid"] = dataVec[i].gid;
			m_jsonResult["teams"][i]["gname"] = dataVec[i].gname;
			m_jsonResult["teams"][i]["coins"] = dataVec[i].coins;
			m_jsonResult["teams"][i]["uc"] = dataVec[i].numOfApply;
			m_jsonResult["teams"][i]["ucoins"] = dataVec[i].ucoins;
		}

		return 0;
	}

	int AllServerGuessView()
	{
		string type ;
		if(!Json::GetString(m_data,"type",type))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		string url = "action=guessview";
		string uid;
		String::Format(uid, "%u", m_uid);
		url.append("&uid=");
		url += uid;
		url.append("&type=");
		url += type;
		string level;
		if (Json::GetString(m_data, "level", level))
		{
			url.append("&level=").append(level);
		}
		Json::Value Request;
		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.RequestBaseMatch(url, Request);
		if (0 != ret)
		{
			return ret;
		}
		m_jsonResult["teams"] = Request["teams"];
		return 0;
	}

	int NominateAlliance()
	{
		DataUser user;
		CLogicUser logicUser;
		int ret = logicUser.GetUserLimit(m_uid, user);
		if (ret != 0)
			return ret;

		CLogicNominateAlliance nAlliance;
		ret = nAlliance.GetAlliance(user.kingdom, m_jsonResult["nominate"]);
		if(0 != ret)
		{
			return ret;
		}
		return 0;
	}

	int GetNationalWarrgtData()
	{

		CLogicNationalWarRgt nAlliance;
		int ret = nAlliance.GetAllCityStateJson(m_uid, m_jsonResult["rgtdata"]);
		if(0 != ret)
		{
			return ret;
		}
		return 0;
	}
	int UpdateNationalWarrgtdata()
	{
		CLogicNationalWarRgt nAlliance;
		int ret = nAlliance.UpdateCityState(m_uid,m_data,m_jsonResult["updatergtdata"]);
		if(0 != ret)
		{
			return ret;
		}
		ret = nAlliance.GetAllCityStateJson(m_uid, m_jsonResult["rgtdata"]);
		if(0 != ret)
		{
			return ret;
		}
		return 0;
	}

	int InviteAllianceNationalWarrgtdata()
	{
		CLogicNationalWarRgt nAlliance;
		int ret = nAlliance.InviteAlliance(m_uid,m_data,m_jsonResult["invitergtdata"]);
		if(0 != ret)
		{
			return ret;
		}
		return 0;
	}

	int GetGroupsFightingData()
	{
		CLogicGroupsFighting Alliancefight;
		int cityid;
		if(!Json::GetInt(m_data, "cityId", cityid))
		{
			return R_ERR_PARAM;
		}
		int ret = Alliancefight.GetGroupsFightingJsonData(cityid,m_jsonResult["groupfightingdata"],m_data);
		if(0 != ret)
		{
			return ret;
		}
		return 0;
	}

	int UpdatasGroupsFightingData()
	{
		CLogicGroupsFighting Alliancefight;
		int cityid;
		if(!Json::GetInt(m_data, "cityId", cityid))
		{
			return R_ERR_PARAM;
		}
		int ret = Alliancefight.LogicUpdatasFightingData(cityid,m_data,m_uid);
		if(0 != ret)
		{
			return ret;
		}
		return 0;
	}

	int AddFullmorale()
	{
		CLogicGroupsFighting Alliancefight;
		int cityid;
		if(!Json::GetInt(m_data, "cityId", cityid))
		{
			return R_ERR_PARAM;
		}
		int ret = Alliancefight.LogicAddFullmorale(m_uid,cityid);
		if(0 != ret)
		{
			return ret;
		}
		return 0;
	}

};

CGI_MAIN(CCgiAlliance)

