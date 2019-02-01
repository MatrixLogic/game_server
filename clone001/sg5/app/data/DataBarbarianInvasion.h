/*
 * DataBarbarianInvasion.h
 *
 *  Created on: 2012-12-14
 *      Author: Administrator
 */

#ifndef DATABARBARIANINVASION_H_
#define DATABARBARIANINVASION_H_
#include "Kernel.h"
#define MAX_PONITS_NUM 60
#define TEAM_NAME_LEN_MAX 32
#define MAX_TEAM_NUM 512
struct DataPlayerItem{
	unsigned uid;
	char name[32];
	unsigned points;
	int rewardCash;
	int standTs;
	DataPlayerItem &operator=(DataPlayerItem const &other)
	{
		uid = other.uid;
		memcpy(name,other.name, sizeof(name) - 1);
		points = other.points;
		rewardCash = other.rewardCash;
		standTs = other.standTs;
		return *this;
	}

};
struct DataPlayersPoints{
	unsigned aid;
	char aname[32];
	int playerNum;
	DataPlayerItem players[MAX_PONITS_NUM];
	DataPlayersPoints &operator=(DataPlayersPoints const &other)
	{

		aid = other.aid;
		memcpy(aname,other.aname, sizeof(aname) - 1);
		playerNum = other.playerNum;
		memset(players,0x00,sizeof(players));
		for(int i= 0; i < other.playerNum; i++)
		{
			players[i] = other.players[i];
		}
		return *this;
	}
};
struct DataTeamItem{
	unsigned aid;
	char     aname[TEAM_NAME_LEN_MAX];
	unsigned teamPoints;
	DataTeamItem &operator=(DataTeamItem const &other)
	{
		aid = other.aid;
		memcpy(aname,other.aname, sizeof(aname) - 1);
		teamPoints = other.teamPoints;
		return *this;
	}
};
struct DataBarbarianInvasionRank{
	unsigned ts;
	int teamNum;
	DataTeamItem teamRank[MAX_TEAM_NUM];
	DataPlayersPoints playerRank[MAX_TEAM_NUM];
};
class CDataBarbarianInvasion {
public:
	CDataBarbarianInvasion();
	virtual ~CDataBarbarianInvasion();
	int Init(const std::string &path, semdat sem=sem_barbarian);
	int UpdateRank(DataPlayerItem const &playerItem, unsigned aid);
	int GetTeamPlayersRank(DataPlayersPoints &playerRank, unsigned aid);
	int GetTeamRank(DataTeamItem teamRank[MAX_TEAM_NUM], int &teamsNum);
	int RewardCashCaculate(DataBarbarianInvasionRank *pdata);

protected:
	bool m_init;
	CShareMemory m_sh;

};

#endif /* DATABARBARIANINVASION_H_ */
