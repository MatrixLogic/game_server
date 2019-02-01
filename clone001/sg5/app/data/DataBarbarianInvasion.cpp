/*
 * DataBarbarianInvasion.cpp
 *
 *  Created on: 2012-12-14
 *      Author: Administrator
 */

#include "DataBarbarianInvasion.h"
#include "DataAlliance.h"
#define TEAM_REWARD_PERSENT_1   1
#define TEAM_REWARD_PERSENT_2   0.8
#define TEAM_REWARD_PERSENT_3   0.5
#define TEAM_REWARD_PERSENT_4   0.3
CDataBarbarianInvasion::CDataBarbarianInvasion() {
	// TODO Auto-generated constructor stub
	m_init = false;
}

CDataBarbarianInvasion::~CDataBarbarianInvasion() {
	// TODO Auto-generated destructor stub
}

int CDataBarbarianInvasion::Init(const std::string &path, semdat sem)
{
	if(m_init)
	{
		return 0;
	}

	int semgroup = 0;
	int semserver = 0;
	Config::GetDomain(semserver);
	if(!semserver)
		Config::GetDB(semserver);
	if(!m_sh.CreateOrOpen(path.c_str(), sizeof(DataBarbarianInvasionRank), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init__BarbarianInvasion_data_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{
		DataBarbarianInvasionRank *pdata = (DataBarbarianInvasionRank *)m_sh.GetAddress();
		if(pdata == NULL)
		{
			DB_ERROR_RETURN_MSG("init_BarbarianInvasion_fail");
		}
		memset(pdata, 0x00, sizeof(*pdata));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}
int sortPlayers(const DataPlayerItem& left,const DataPlayerItem& right){
	return left.points > right.points;
}

int sortTeam(const DataTeamItem& left,const DataTeamItem& right){
	return left.teamPoints > right.teamPoints;
}
int CDataBarbarianInvasion::RewardCashCaculate(DataBarbarianInvasionRank *pdata)
{
	int i = 0,j = 0, m = 0;
	double teamRewardPersent = 0;
	double rewardCash = 0;
	for(i = 0; i < pdata->teamNum; i++)
	{
		if(i == 0)
		{
			teamRewardPersent = TEAM_REWARD_PERSENT_1;
		}else if(i == 1 || i == 2)
		{
			teamRewardPersent = TEAM_REWARD_PERSENT_2;
		}else if(i > 2 && i < 8)
		{
			teamRewardPersent = TEAM_REWARD_PERSENT_3;
		}else{
			teamRewardPersent = TEAM_REWARD_PERSENT_4;
		}
		for(j = 0; j < pdata->teamNum; j++)
		{
			if(pdata->teamRank[i].aid == pdata->playerRank[j].aid)
			{
				for(m = 0; m < pdata->playerRank[j].playerNum; m++)
				{
					if(pdata->playerRank[j].players[m].points >= 3100)
					{
						rewardCash = 100;
					}else if(pdata->playerRank[j].players[m].points >= 2000 && pdata->playerRank[j].players[m].points <3100)
					{
						rewardCash = 70;
					}else if(pdata->playerRank[j].players[m].points >= 1400 && pdata->playerRank[j].players[m].points <2000)
					{
						rewardCash = 50;

					}else if(pdata->playerRank[j].players[m].points >= 750 && pdata->playerRank[j].players[m].points <1400)
					{
						rewardCash = 30;
					}else if(pdata->playerRank[j].players[m].points >= 200 && pdata->playerRank[j].players[m].points <750)
					{
						rewardCash = 20;
					}else{
						rewardCash = 0;
					}
					pdata->playerRank[j].players[m].rewardCash = rewardCash * teamRewardPersent;
				}
				break;
			}
		}
	}
	return 0;
}
int CDataBarbarianInvasion::UpdateRank(DataPlayerItem const &playerItem, unsigned aid)
{
	int i= 0, j = 0;
	int addPoints = 0;
	int teamIndex = -1;
	int playerIndex = -1;
	bool playerInFlag =false;
	vector<DataPlayerItem> vPlayersRank;
	vector<DataTeamItem> vTeamRank;
	DataBarbarianInvasionRank *pdata = (DataBarbarianInvasionRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}
	CAutoLock lock(&m_sh, true);
	// 联盟成员数据更新
	unsigned now = time(0);
	if (CTime::GetDayInterval(pdata->ts, now) != 0)
	{
		memset(pdata, 0x00, sizeof(DataBarbarianInvasionRank));
		pdata->ts = now;
	}
	for(i = 0 ;i < pdata->teamNum; i++)
	{
		// 联盟成员数据更新
		if(pdata->playerRank[i].aid == aid  )
		{
			playerIndex = i;
			for(j = 0; j < pdata->playerRank[i].playerNum; j++)
			{
				if(pdata->playerRank[i].players[j].uid == playerItem.uid)
				{
					addPoints = playerItem.points - pdata->playerRank[i].players[j].points;
					pdata->playerRank[i].players[j] = playerItem;
					vPlayersRank.push_back(pdata->playerRank[i].players[j]);
					playerInFlag = true;
					//break;
				}else{
					vPlayersRank.push_back(pdata->playerRank[i].players[j]);
				}
			}
			// 新成员参加活动
			if(!playerInFlag)
			{
				if(pdata->playerRank[i].playerNum == MAX_PONITS_NUM)
				{
					DB_ERROR_RETURN_MSG("players_full");
				}

				pdata->playerRank[i].players[pdata->playerRank[i].playerNum] = playerItem;
				addPoints = playerItem.points;
				pdata->playerRank[i].playerNum++;
				vPlayersRank.push_back(playerItem);
				// playerIndex = pdata->playerRank[i].playerNum - 1;
			}

		}

		if(pdata->teamRank[i].aid == aid)
		{
			teamIndex = i;
		}
		vTeamRank.push_back(pdata->teamRank[i]);
	}
	// 新联盟参加活动
	if(i >= pdata->teamNum && teamIndex == -1)
	{
		// pdata->teamNum++;
		if(pdata->teamNum >= MAX_TEAM_NUM)
		{
			DB_ERROR_RETURN_MSG("teams_full");
		}
		CDataAlliance dbAlliance;
		DataAlliance alliance;
		int ret = dbAlliance.GetAllianceLimit(aid,alliance);
		if(ret != 0)
		{
			return ret;
		}
		pdata->teamRank[pdata->teamNum].aid = aid;
		memcpy(pdata->teamRank[pdata->teamNum].aname,alliance.name.c_str(),sizeof(pdata->teamRank[pdata->teamNum].aname) - 1);
		pdata->teamRank[pdata->teamNum].teamPoints = playerItem.points;
		vTeamRank.push_back(pdata->teamRank[pdata->teamNum]);

		pdata->playerRank[pdata->teamNum].aid = aid;
		pdata->playerRank[pdata->teamNum].players[0] = playerItem;
		pdata->playerRank[pdata->teamNum].playerNum++;
		playerIndex = pdata->teamNum ;
		pdata->teamNum++;
		vPlayersRank.push_back(playerItem);
	}else{
		// 联盟总积分增加
		pdata->teamRank[teamIndex].teamPoints += addPoints;
		vTeamRank[teamIndex].teamPoints = pdata->teamRank[teamIndex].teamPoints;
	}
	// 排序
	sort(vTeamRank.begin(),vTeamRank.end(),sortTeam);
	sort(vPlayersRank.begin(),vPlayersRank.end(),sortPlayers);
	for(i = 0 ;i < pdata->teamNum; i++)
	{
		pdata->teamRank[i] = vTeamRank[i];
		if(i == playerIndex)
		{
			for(j = 0; j < pdata->playerRank[i].playerNum; j++)
			{
				pdata->playerRank[i].players[j] = vPlayersRank[j];
			}
		}
	}

	return 0;
}
int CDataBarbarianInvasion::GetTeamPlayersRank(DataPlayersPoints &playerRank, unsigned aid)
{
	int i = 0;
	DataBarbarianInvasionRank *pdata = (DataBarbarianInvasionRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}
	CAutoLock lock(&m_sh, true);
	for(i = 0; i < pdata->teamNum; i++)
	{
		if(pdata->playerRank[i].aid == aid)
		{
			//memcpy(&playerRank,&pdata->playerRank[i],sizeof(DataPlayersPoints));
			playerRank = pdata->playerRank[i];
			return 0;
		}
	}
	return 0;
}
int CDataBarbarianInvasion::GetTeamRank(DataTeamItem teamRank[MAX_TEAM_NUM],int &teamsNum)
{
	DataBarbarianInvasionRank *pdata = (DataBarbarianInvasionRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}
	CAutoLock lock(&m_sh, true);
	memcpy(teamRank,pdata->teamRank,sizeof(DataTeamItem) * (pdata->teamNum));
	teamsNum = pdata->teamNum;
	return 0;
}
