#ifndef _DATA_MATCH_H_
#define _DATA_MATCH_H_

#include "Kernel.h"

#define MATCH_TEAMS_MAX 512
#define MATCH_PLAYERS_MAX 60
#define MATCH_NAME_LEN_MAX 32
struct MatchPlayer
{
	unsigned uid;
	char     name[MATCH_NAME_LEN_MAX];
};
struct MatchTeam
{
	unsigned aid;
	char     aname[MATCH_NAME_LEN_MAX];
	int      flag;
	unsigned number;
	MatchPlayer players[MATCH_PLAYERS_MAX];
};
struct MatchRegularRival
{
	MatchPlayer player;
	MatchPlayer rplayers[3];
	char     result[3];		//0:没比,1:输,3:赢
	char     score;
};
struct MatchRegularTeam
{
	unsigned aid;
	char     aname[MATCH_NAME_LEN_MAX];
	int      flag;
	unsigned score;
	MatchRegularRival rivals[9];
};
struct MatchPlayoffTeam
{
	unsigned aid;
	char     aname[MATCH_NAME_LEN_MAX];
	int      flag;
	MatchPlayer players[9];
	char     result[9];
	char     score;
};
struct MatchData
{
	char stage;					//0:报名阶段,1:常规赛,2:季后赛
	unsigned numOfApply;		//报名的联盟数量
	MatchTeam teams[MATCH_TEAMS_MAX];	//已报名的队
	unsigned numOfReg;			//常规赛联盟数
	MatchRegularTeam rteams[MATCH_TEAMS_MAX];
	MatchPlayoffTeam top8[8];
	MatchPlayoffTeam top4[4];
	MatchPlayoffTeam top2[2];
	MatchPlayoffTeam champion;
};

enum MatchStage
{
	MS_APPLY,
	MS_REGULAR,
	MS_PLAYOFF8,
	MS_PLAYOFF4,
	MS_PLAYOFF2,
	MS_MATCH_FINISH,
	MS_GUESS = 10
};

class CDataMatch
{
public:
	CDataMatch();
	int Init(const string &path, semdat sem=sem_match);
	int View(unsigned aid, unsigned uid, int &stage, MatchTeam &team, MatchRegularRival &rival,
			vector<MatchPlayoffTeam> &playoff);
	int Apply(unsigned aid, const string &aname, int flag, unsigned uid, const string &name);
	int ReportResult(unsigned uid, int order, int result);
	int StartRegular();
	int StartGuess();
	int StartPlayoff8();
	int StartPlayoff4();
	int StartPlayoff2();
	int FinishMatch(const MatchData **p);
	int NextTurn();
	int IsMatchProtect(unsigned aid, unsigned uid, bool &prot);

	int GetApplyPlayersOfTeam(unsigned aid, vector<MatchPlayer> &players);
	int GetRegularScore(unsigned aid, unsigned &score, unsigned &rank, vector<MatchRegularRival> &players);
	int GetStage(int& stage);

protected:
	CShareMemory m_sh;
	bool m_init;
};

#endif
