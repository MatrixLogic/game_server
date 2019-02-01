#ifndef __LOGIC_FIGHTING_H__
#define __LOGIC_FIGHTING_H__

#include "LogicInc.h"

class CLogicFighting
{
public:
	int Load(unsigned fightingId, unsigned uidBy, Json::Value &result);
	int Save(unsigned fightingId, DataUser &userBy, const Json::Value &data, Json::Value &result);

	int ExportFightingData(unsigned uid, unsigned fightingId, const string &dir);

	static int GetFightingArchive(unsigned id, Json::Value &data);
};

#endif
