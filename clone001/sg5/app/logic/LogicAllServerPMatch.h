#ifndef _LOGIC_ALL_SERVER_PERSON_MATCH_H_
#define _LOGIC_ALL_SERVER_PERSON_MATCH_H_

#include "LogicInc.h"

class CLogicAllServerPersonMatch
{
public:
	static CAllServerPersonBaseMatch* GetAllServerPsersonBaseMatch(int level);

	int GetBaseMatchInfo(unsigned uid, Json::Value &data, int level);
	int Apply(unsigned uid,int level);
	int ReportResult(unsigned uid, int order, int damage, int level);
	int Load(unsigned instid, unsigned uidBy, Json::Value &result);
	int GetStage(int& stage,int level);
	int StartRegular(int lev);
	int StartGuess(int lev);
	int StartPlayoff8(int lev);
	int StartPlayoff4(int lev);
	int StartPlayoff2(int lev);
	int FinishMatch(int lev);
	int NextTurn(int lev);
	int GetRival(int idx,PersonMatchPlayer& rival);
	int Watch(int lev);
	int getInstPath(string &path, int lev);
	PersonMatchData* GetPersonMatchData();
private:
	int PromotionRule(const PersonMatchPlayer &p1,const PersonMatchPlayer &p2,int lev,PersonMatchPlayer &winner);
};

#endif

