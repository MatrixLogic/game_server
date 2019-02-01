#ifndef __LOGIC_FIGHT_ARENA_H__
#define __LOGIC_FIGHT_ARENA_H__

#include "LogicInc.h"

class CLogicFightArena
{
public:
	int GetFightArena(unsigned arenaId, DataFightArena &data);
	int GetFightArenaLimit(unsigned arenaId, DataFightArenaLimit &data);
	int BreathFightArena(int arenaId, unsigned challenger);
	bool EnableChallenge(int arenaId, unsigned challenger, unsigned addPrize, DataFightArenaLimit &data);
	int UpdateWinFightArena(int arenaId, unsigned host, unsigned level, const string &name,
			const string &pic, const string &archive, DataFightArenaLimit &data);
	int UpdateLoseFightArena(int arenaId, unsigned challenger, DataFightArenaLimit &data);

	int GetFightArenaJson(Json::Value &arena);
	int Load(unsigned arenaid, unsigned uidBy, LoadType loadType, int regfee, Json::Value &result);
	int Save(unsigned arenaid, DataUser &userBy, const Json::Value &data, Json::Value &result);
	int FightOver(unsigned arenaid, unsigned challenger, bool isWin, Json::Value &result);

	static CDataFightArena* GetCDataFightArena();
};

#endif
