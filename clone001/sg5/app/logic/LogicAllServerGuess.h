#ifndef _LOGIC_ALL_SERVER_GUESS_H_
#define _LOGIC_ALL_SERVER_GUESS_H_


#include "LogicInc.h"
typedef pair<unsigned, unsigned> U_U_MAP;

class CLogicAllServerGuess{
public:
	static CGuessAllServerData* GetDataAllServerGuess(int type,int lev=0);
	int GuessorsPay(unsigned gid,int type,int lev=0);
	int GuessorsUid(unsigned gid, int type,int lev=0); // 根据gid 查找支持他的用户id
	int GuessorsUidCoins(unsigned gid, vector<U_U_MAP> &vecGuessors, int type,int lev=0);
	int SetInitValues(int type,int lev,const vector<GuessViewData> &dataVec);
	int GetSupportCount(unsigned gid,int type,int lev,unsigned &coins);
	int ViewGuess(unsigned uid, Json::Value &data, int type, int lev);
	int ApplyGuess(unsigned uid,unsigned gid, unsigned coins, int type, int lev);
};


#endif
