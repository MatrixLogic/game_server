#ifndef _DATA_GUESS_H_
#define _DATA_GUESS_H_

#include "Kernel.h"

#define MAX_GUESS_COUNT 1000
#define GUESS_GROUP_COUNT 8
#define PER_BET_COINS 10
#define GUESS_DATA_PATH "guess_data_path"
#define ALL_SERVER_GUESS_DATA_PATH "all_server_guess_data_path"

struct GuessItem{
	unsigned uid;
	unsigned coins;
};

struct GuessStat{
	unsigned gid;
	char gname[64];
	unsigned numOfApply;
	unsigned coins;
	GuessItem supports[MAX_GUESS_COUNT];
};


struct GuessData{
	GuessStat guessor[GUESS_GROUP_COUNT];
};

struct GuessViewData{
	unsigned gid;
	char gname[64];
	unsigned numOfApply;
	unsigned coins;
	unsigned ucoins;
};

class CGuessData{
public:
	CGuessData();
	int Init(const string &path,int type, semdat sem=sem_guess);
	int SetInitValues(const vector<GuessViewData>& data);
	int Apply(unsigned uid,unsigned gid,unsigned coins);
	int View(unsigned uid,vector<GuessViewData>& data);
	int GetSupportCount(unsigned gid,unsigned& count);
	GuessData* GetData();
	int Dump();
private:
	CShareMemory m_sh;
	bool m_init;
};

#endif
