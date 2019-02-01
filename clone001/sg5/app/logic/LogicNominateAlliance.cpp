#include "LogicMatch.h"

CDataNominateAlliance* CLogicNominateAlliance::GetCDataNominateAlliance()
{
	GET_MEM_DATA_SEM(CDataNominateAlliance, CONFIG_NOMINATE_ALLIANCE_PATH, sem_nominatealliance)
	/*static CDataNominateAlliance* pData = NULL;
	if (!pData)
	{
		pData = new CDataNominateAlliance();
		if (pData->Init(Config::GetValue(CONFIG_NOMINATE_ALLIANCE_PATH)) != 0)
		{
			delete pData;
			pData = NULL;
		}
	}
	return pData;*/
}

int CLogicNominateAlliance::GetAlliance(unsigned kingdom, Json::Value &data)
{
	CDataNominateAlliance *pData = GetCDataNominateAlliance();
	if (NULL == pData)
	{
		return R_ERR_DATA;
	}
	vector<DataNominateAlliance> allianceList;
	int ret = pData->GetNominateAlliance(allianceList);
	if (0 != ret)
	{
		error_log("[GetNominateAlliance fail!][ret=%e]", ret);
		return ret;
	}
	vector<DataNominateAlliance>::iterator itr = allianceList.begin();
	CLogicAlliance logicAlliance;
	DataAlliance alliance;
	map<int, vector<DataAlliance> > allianceMap;
	map<unsigned, int> allianceFlag;
	CLogicUser logicUser;
	DataUser user;
	for(; itr!=allianceList.end(); ++itr)
	{
		ret = logicAlliance.GetAlliance(itr->alliance_id, alliance);
		if (0 != ret)
		{
			error_log("[GetAlliance fail!][ret=%d,aid=%u]", ret, itr->alliance_id);
			ret = pData->RemoveData(itr->alliance_id);
			if (0 != ret)
			{
				error_log("remove alliance from list fail,aid=%u", itr->alliance_id);
			}
			continue;
		}

		int membercount = logicAlliance.GetMemberMaxCount(alliance.level);
		if (alliance.member_count >= membercount)
		{
			ret = pData->RemoveData(itr->alliance_id);
			if (0 != ret)
			{
				error_log("remove alliance from list fail,aid=%u", itr->alliance_id);
			}
			continue;
		}

		ret = logicUser.GetUserLimit(alliance.leader_uid, user);
		if (0 != ret)
		{
			ret = pData->RemoveData(itr->alliance_id);
			if (0 != ret)
			{
				error_log("remove alliance from list fail,aid=%u", itr->alliance_id);
			}
			continue;
		}
/************************
		if (!IsOnlineUser(user.last_active_time))
		{
			error_log("leader is not online!aid=%u,leader=%u", alliance.alliance_id, alliance.leader_uid);
			ret = pData->RemoveData(itr->alliance_id);
			if (0 != ret)
			{
				error_log("remove alliance from list fail,aid=%u", itr->alliance_id);
			}
			continue;
		}
************************/
		if (itr->sign != 0)
		{
			allianceMap[alliance.level].insert(allianceMap[alliance.level].begin(), alliance);
		}
		else
		{
			allianceMap[alliance.level].push_back(alliance);
		}
		allianceFlag[itr->alliance_id] = itr->sign;
	}

	data.clear();
	CDataUserBasic dbUserBasic;
	string userName;
	for (int i=10; i>0; i--)
	{
		if (allianceMap[i].empty())
		{
			continue;
		}
		vector<DataAlliance>::iterator aitr = allianceMap[i].begin();
		for(; aitr!=allianceMap[i].end(); ++aitr)
		{
			if(aitr->kingdom != kingdom)
				continue;

			Json::Value &tempData = data[data.size()];
			tempData["aid"] = aitr->alliance_id;
			tempData["kingdom"] = aitr->kingdom;
			tempData["rank"] = aitr->rank;
			tempData["name"] = aitr->name;
			tempData["membercount"] = aitr->member_count;
			tempData["level"] = aitr->level;
			tempData["sign"] = allianceFlag[aitr->alliance_id];
			tempData["leader_uid"] = aitr->leader_uid;
			ret = dbUserBasic.GetUserName(aitr->leader_uid, OpenPlatform::GetType(), userName);
			if (0 == ret)
			{
				tempData["leader_name"] = userName;
			}
			Json::Value extraData;
			Json::FromString(extraData, aitr->extra_data);
			if (extraData.isMember("activeD"))
			{
				tempData["activeD"] = extraData["activeD"];
			}
			if (extraData.isMember("attackD"))
			{
				tempData["attackD"] = extraData["attackD"];
			}
		}
	}
	return 0;
}

int CLogicNominateAlliance::GetAlliance(unsigned aid, int &sign)
{
	CDataNominateAlliance *pData = GetCDataNominateAlliance();
	if (NULL == pData)
	{
		return R_ERR_DATA;
	}
	vector<DataNominateAlliance> allianceList;
	int ret = pData->GetNominateAlliance(allianceList);
	if (0 != ret)
	{
		error_log("[GetNominateAlliance fail!][ret=%e]", ret);
		return ret;
	}
	vector<DataNominateAlliance>::iterator itr = allianceList.begin();
	for (; itr!=allianceList.end(); ++itr)
	{
		if (aid == itr->alliance_id)
		{
			sign = itr->sign;
			return 0;
		}
	}
	return R_ERR_NO_DATA;
}

int CLogicNominateAlliance::SetAlliance(unsigned alliance_id, int flag)
{
	CDataNominateAlliance *pData = GetCDataNominateAlliance();
	if (NULL == pData)
	{
		return R_ERR_DATA;
	}
	DataNominateAlliance alliance;
	alliance.alliance_id = alliance_id;
	alliance.sign = flag;
	int ret = pData->SetNominateAlliance(alliance);
	if (0 != ret)
	{
		return ret;
	}
	return 0;
}

int CLogicNominateAlliance::CheckAlliance(unsigned uid)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUserLimit(uid, user);
	if (0 != ret)
	{
		return ret;
	}

	if (!IsAllianceId(user.alliance_id))
	{
		return 0;
	}

	CLogicAlliance logicAlliance;
	if (!logicAlliance.IsLeader(uid, user.alliance_id))
	{
		return 0;
	}

	return SetAlliance(user.alliance_id, 0);
}

int CLogicNominateAlliance::RemoveAlliance(unsigned aid)
{
	CDataNominateAlliance *pData = GetCDataNominateAlliance();
	if (NULL == pData)
	{
		return R_ERR_DATA;
	}
	return pData->RemoveData(aid);
}
