#ifndef __LOGIC_ARENA_H__
#define __LOGIC_ARENA_H__

#include "LogicInc.h"

class CLogicArena
{
public:
	int GetArena(unsigned arenaId, DataArena &data);
	int GetArenaLimit(unsigned arenaId, DataArenaLimit &data);
	int BreathArena(int arenaId, unsigned challenger);
	bool EnableChallenge(int arenaId, unsigned challenger, unsigned addPrize, DataArena &data);
	int UpdateWinArena(int arenaId, unsigned host, unsigned level, const string &name,
			const string &pic, const string &archive, DataArenaLimit &data);

	int GetArenaJson(Json::Value &arena);
	int Load(unsigned arenaid, unsigned uidBy, LoadType loadType, int regfee, Json::Value &result);
	int Save(unsigned arenaid, DataUser &userBy, Json::Value &data, Json::Value &result);

	static CDataArena* GetCDataArena();
};

#endif
