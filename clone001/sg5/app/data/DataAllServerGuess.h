#ifndef _DATA_ALL_SERVER_GUESS_H_
#define _DATA_ALL_SERVER_GUESS_H_

#include "Kernel.h"
#include "DataGuess.h"

class CGuessAllServerData{
public:
	CGuessAllServerData();
	int Init(const string &path,int type, semdat sem=sem_guessallserver);
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
