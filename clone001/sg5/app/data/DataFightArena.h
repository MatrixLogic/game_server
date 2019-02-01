#ifndef __DATA_FIGHT_ARENA_H__
#define __DATA_FIGHT_ARENA_H__

#include "Kernel.h"

#define DATA_FIGHT_ARENA_HEAD \
	unsigned host;			/*擂主uid*/ \
	unsigned level;			/*擂主等级*/ \
	unsigned winCount;		/*连擂次数*/ \
	unsigned challenger;	/*挑战者*/ \
	unsigned challengeTs;	/*最近挑战开始时间*/ \
	unsigned breathTs;      /*最近挑战者活跃时间*/ \
	unsigned preHost;		/*前一个擂主*/ \
	unsigned preChallenger;	/*前一个挑战者uid*/ \
	unsigned prizePool;		/*奖金池*/ \
	int      intExtend[10];		/*预留*/ \
	char     name[32];			/*擂主姓名*/ \
	char     pic[300];          /*擂主头像*/ \
	char     charExtend[40];	/*预留*/
struct DataFightArena
{
	DATA_FIGHT_ARENA_HEAD
	char     archive[1048128];		//存档
};
struct DataFightArenaLimit
{
	DATA_FIGHT_ARENA_HEAD
};

class CDataFightArena
{
public:
	CDataFightArena();
	int Init(const string &path, semdat sem);
	int GetFightArena(int arenaId, DataFightArena &data);
	int GetFightArenaLimit(int arenaId, DataFightArenaLimit &data);
	int SetFightArena(int arenaId, const DataFightArena &data);
	int SetFightArenaLimit(int arenaId, const DataFightArenaLimit &data);
	bool EnableChallenge(int arenaId, unsigned challenger, unsigned addPrize, DataFightArenaLimit &data);
	int UpdateWinFightArena(int arenaId, unsigned host, unsigned level, const string &name,
			const string &pic, const string &archive, DataFightArenaLimit &data);
	int BreathFightArena(int arenaId, unsigned challenger);

	int UpdateLoseFightArena(int arenaId, unsigned challenger, DataFightArenaLimit &data);

protected:
	CShareMemory m_sh[FIGHT_ARENA_ID_MAX - FIGHT_ARENA_ID_MIN + 1];
	bool m_init;
};

#endif
