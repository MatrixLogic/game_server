#ifndef _DATA_PERSONMATCH_H_
#define _DATA_PERSONMATCH_H_

#include "Kernel.h"


#define PERSONMATCH_PLAYERS_MAX 5000
#define PERSONMATCH_NAME_LEN_MAX 32

#define PERSON_INST_PATH  "person_inst_path"
#define ALL_SERVER_PERSON_INST_PATH  "all_server_person_inst_path"


struct PersonMatchPlayer
{
	unsigned uid;
	unsigned instanceid;
	char     name[PERSONMATCH_NAME_LEN_MAX];
	unsigned char 	damage[3];
	char 	time[3];
	char	flag[3];
	int		rivals[3];
};

struct PersonMatchData
{
	unsigned instance_ndx;
	char stage;
	unsigned numOfApply;
	PersonMatchPlayer rival[8];
	PersonMatchPlayer mems[PERSONMATCH_PLAYERS_MAX];
	PersonMatchPlayer top8[8];
	PersonMatchPlayer top4[4];
	PersonMatchPlayer top2[2];
	PersonMatchPlayer champion;
};

enum PersonMatchStage
{
	PMS_APPLY,
	PMS_REGULAR,
	PMS_PLAYOFF8,
	PMS_PLAYOFF4,
	PMS_PLAYOFF2,
	PMS_MATCH_FINISH,
	PMS_GUESS = 10,
	PMS_MAX = 0xFFFF
};

class CPersonBaseMatch
{
public:
	CPersonBaseMatch();
	int Init(const string &path, semdat sem=sem_personmatch);
	int View(unsigned uid, int &stage,PersonMatchPlayer& person,vector<PersonMatchPlayer> &playoff);
	int Apply(unsigned uid, const string &name, unsigned &instanceid);
	int ReportResult(unsigned uid, int order, int damage);
	int StartRegular();
	int StartGuess();
	int StartPlayoff8();
	int StartPlayoff4();
	int StartPlayoff2();
	int FinishMatch(const PersonMatchData **p);
	int NextTurn();
	int GetRival(int idx,PersonMatchPlayer& rival);
	int Watch();
	int SetRivals();
	int ResetInsts();
	int GetStage(int& stage);
	int EndStage();
	//int Watch1();
	PersonMatchData* GetPersonMatchData();

private:
	int setPersonInst(int uid,int instId);
	int getInstPath(string& path);
	unsigned getInstId(PersonMatchData *pdata);
protected:
	CShareMemory m_sh;
	bool m_init;
};

#endif


