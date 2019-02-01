#ifndef _DATA_ALL_SERVER_PERSONMATCH_H_
#define _DATA_ALL_SERVER_PERSONMATCH_H_

#include "Kernel.h"
#include "DataPersonMatch.h"

class CAllServerPersonBaseMatch
{
public:
	CAllServerPersonBaseMatch();
	int Init(const string &path, semdat sem);
	int View(unsigned uid, int &stage,PersonMatchPlayer& person,vector<PersonMatchPlayer> &playoff);
	int Apply(unsigned uid, const string &name, unsigned &instanceid);
	int ReportResult(unsigned uid, int order, int damage);
	int SetCurrInstId(unsigned instId);
	int GetNextInstId(unsigned &instId);
	int GetStage(int &stage);
	int SetRivals(PersonMatchPlayer rival[8]);
	int Watch(void);
	int NextTurn(void);
	int StartRegular(void);
	int StartGuess(void);
	int StartPlayoff8(void);
	int StartPlayoff4(PersonMatchPlayer *top4);
	int StartPlayoff2(PersonMatchPlayer *top2);
	int FinishMatch(PersonMatchPlayer *champion);
	int GetTop8Data(vector<GuessViewData> &dataVec);
	int GetRival(int idx,PersonMatchPlayer& rival);
	PersonMatchData* GetPersonMatchData(CShareMemory* sh);

private:
	int setPersonInst(int uid,int instId);
	unsigned getInstId(PersonMatchData *pdata);
protected:
	CShareMemory m_sh;
	bool m_init;
};

#endif


