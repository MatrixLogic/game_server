#ifndef __DATA_ARENA_H__
#define __DATA_ARENA_H__

#include "Kernel.h"

#define DATA_ARENA_HEAD \
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
struct DataArena
{
	DATA_ARENA_HEAD
	char     archive[1048128];		//存档
};
struct DataArenaLimit
{
	DATA_ARENA_HEAD
};

class CDataArena
{
public:
	CDataArena();
	int Init(const string &path, semdat sem=sem_arena);
	int GetArena(int arenaId, DataArena &data);
	int GetArenaLimit(int arenaId, DataArenaLimit &data);
	int SetArena(int arenaId, const DataArena &data);
	int SetArenaLimit(int arenaId, const DataArenaLimit &data);
	bool EnableChallenge(int arenaId, unsigned challenger, unsigned addPrize, DataArena &data);
	int UpdateWinArena(int arenaId, unsigned host, unsigned level, const string &name,
			const string &pic, const string &archive, DataArenaLimit &data);
	int BreathArena(int arenaId, unsigned challenger);

protected:
	CShareMemory m_sh[ARENA_ID_MAX - ARENA_ID_MIN + 1];
	bool m_init;
};

#endif
