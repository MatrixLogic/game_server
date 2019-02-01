#ifndef _DATA_BASEMATCH_H_
#define _DATA_BASEMATCH_H_

#include "Kernel.h"

#define BASEMATCH_TEAMS_MAX 512
#define BASEMATCH_PLAYERS_MAX 60
#define BASEMATCH_NAME_LEN_MAX 32
struct BaseMatchPlayer
{
	unsigned uid;
	unsigned instanceid;
	char     name[BASEMATCH_NAME_LEN_MAX];
};
struct BaseMatchTeam
{
	unsigned aid;
	char     aname[BASEMATCH_NAME_LEN_MAX];
	int      flag;
	unsigned number;
	BaseMatchPlayer players[BASEMATCH_PLAYERS_MAX];
};
struct BaseMatchRegularRival
{
	BaseMatchPlayer player;
	BaseMatchPlayer rplayers[3];
	char     damages[3];
	short    damage;
};
struct BaseMatchRegularTeam
{
	unsigned aid;
	char     aname[BASEMATCH_NAME_LEN_MAX];
	int      flag;
	unsigned damage;
	BaseMatchRegularRival rivals[9];
};
struct BaseMatchPlayoffTeam
{
	unsigned aid;
	char     aname[BASEMATCH_NAME_LEN_MAX];
	int      flag;
	BaseMatchPlayer players[9];
	char     damages[9];
	short    damage;
};
struct BaseMatchData
{
	unsigned instance_ndx;
	char stage;					//0:报名阶段,1:常规赛,2:季后赛
	unsigned numOfApply;		//报名的联盟数量
	BaseMatchTeam teams[BASEMATCH_TEAMS_MAX];	//已报名的队
	unsigned numOfReg;			//常规赛联盟数
	BaseMatchRegularTeam rteams[BASEMATCH_TEAMS_MAX];
	BaseMatchPlayoffTeam top8[8];
	BaseMatchPlayoffTeam top4[4];
	BaseMatchPlayoffTeam top2[2];
	BaseMatchPlayoffTeam champion;
};

enum BaseMatchStage
{
	BMS_APPLY,
	BMS_REGULAR,
	BMS_PLAYOFF8,
	BMS_PLAYOFF4,
	BMS_PLAYOFF2,
	BMS_MATCH_FINISH,
	BMS_GUESS = 10
};

class CDataBaseMatch
{
public:
	CDataBaseMatch();
	int Init(const string &path, semdat sem=sem_basematch);
	int View(unsigned aid, unsigned uid, int &stage, BaseMatchTeam &team, BaseMatchRegularRival &rival,
			vector<BaseMatchPlayoffTeam> &playoff);
	int Apply(unsigned aid, const string &aname, int flag, unsigned uid, const string &name, unsigned &instanceid,bool allserver=false);
	int ReportResult(unsigned uid, int order, int damage);
	int StartRegular();
	int StartGuess(bool allserver=false);
	int StartPlayoff8();
	int StartPlayoff4(bool allserver=false);
	int StartPlayoff2(bool allserver=false);
	int FinishMatch(const BaseMatchData **p,bool allserver=false);
	int NextTurn();
	int IsBaseMatchProtect(unsigned aid, unsigned uid, bool &prot);

	int GetApplyPlayersOfTeam(unsigned aid, vector<BaseMatchPlayer> &players);
	int GetRegularScore(unsigned aid, unsigned &damage, unsigned &rank, vector<BaseMatchRegularRival> &players);

	int ResetInsts();
	int GetStage(int& stage);

	BaseMatchData * GetMatchData();

protected:
	CShareMemory m_sh;
	bool m_init;
};

#endif
