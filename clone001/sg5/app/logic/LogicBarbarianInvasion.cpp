/*
 * LogicBarbarianInvasion.cpp
 *
 *  Created on: 2012-12-17
 *      Author: Administrator
 */

#include "LogicBarbarianInvasion.h"

CLogicBarbarianInvasion::CLogicBarbarianInvasion() {
	// TODO Auto-generated constructor stub
	string beginStr = Config::GetValue(CONFIG_BARBARIAN_BEGIN_TS);
	string endStr = Config::GetValue(CONFIG_BARBARIAN_END_TS);
	vector<int> bloods;
	CBasic::StringSplit(beginStr, ":", m_begin_ts);
	CBasic::StringSplit(endStr, ":", m_end_ts);
	///error_log("begin_ts1 = %d,2=%d end1=%d,end2=%d",m_begin_ts[0],m_begin_ts[1],m_end_ts[0],m_end_ts[1]);
}

CLogicBarbarianInvasion::~CLogicBarbarianInvasion() {
	// TODO Auto-generated destructor stub
}

CDataBarbarianInvasion* CLogicBarbarianInvasion::GetCDataBarbarianInvasion()
{
	GET_MEM_DATA_SEM(CDataBarbarianInvasion, CONFIG_BARBARIANINVASION_PATH,sem_barbarian)
	/*static map<int, CDataBarbarianInvasion*> dataMap;
	int serverId = 0;
	Config::GetDomain(serverId);
	if (0 == serverId)
	{
		return NULL;
	}

	map<int, CDataBarbarianInvasion*>::iterator itr = dataMap.find(serverId);
	if (itr != dataMap.end() || NULL != itr->second)
	{
		return itr->second;
	}

	CDataBarbarianInvasion *pdata = new CDataBarbarianInvasion();
	if (!pdata->Init(Config::GetValue(CONFIG_BARBARIANINVASION_PATH)))
	{
		delete pdata;
		pdata = NULL;
		return NULL;
	}
	dataMap[serverId] = pdata;
	return pdata;*/
}
int CLogicBarbarianInvasion::UpdateRank(DataPlayerItem const &playerItem, unsigned aid)
{
	info_log("uid = %u,points = %d",playerItem.uid,playerItem.points);
	time_t now;
	time( &now );
	struct tm *ptm = localtime(&now);
	if(!(ptm->tm_hour >=m_begin_ts[0] && ptm->tm_hour <m_end_ts[0]))
	{
		LOGIC_ERROR_RETURN_MSG("BARBRIAN_OUT_OF_TIME");
	}
	CDataBarbarianInvasion *pdata = GetCDataBarbarianInvasion();
	if(pdata == NULL)
	{
		return -1;
	}
	int ret = pdata->UpdateRank(playerItem,aid);
	if(0 != ret)
	{
		return ret;
	}
	return 0;
}
int CLogicBarbarianInvasion::GetTeamPlayersRank(DataPlayersPoints &playerRank, unsigned aid)
{
	CDataBarbarianInvasion *pdata = GetCDataBarbarianInvasion();
	if(pdata == NULL)
	{
		return -1;
	}
	int ret = pdata->GetTeamPlayersRank(playerRank,aid);
	if(0 != ret)
	{
		return ret;
	}
	return 0;
}
int CLogicBarbarianInvasion::GetTeamRank(DataTeamItem teamRank[], int &teamsNum)
{
	CDataBarbarianInvasion *pdata = GetCDataBarbarianInvasion();
	if(pdata == NULL)
	{
		return -1;
	}
	int ret = pdata->GetTeamRank(teamRank,teamsNum);
	if(0 != ret)
	{
		return ret;
	}
	return 0;
}

