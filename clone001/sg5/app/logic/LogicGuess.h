#ifndef _LOGIC_GUESS_H_
#define _LOGIC_GUESS_H_


#include "LogicInc.h"
typedef pair<unsigned, unsigned> U_U_MAP;

class CLogicGuess{
public:
	CLogicGuess();
	static CGuessData* GetDataGuess(int type);
	int GuessorsPay(unsigned gid,int type);
	int GuessorsUid(unsigned gid, int type); // 根据gid 查找支持他的用户id
	int GuessorsUidCoins(unsigned gid, vector<U_U_MAP> &vecGuessors, int type);
};


#endif
