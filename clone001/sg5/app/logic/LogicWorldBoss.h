#ifndef _LOGIC_WORLD_BOSS_H_
#define _LOGIC_WORLD_BOSS_H_

#include "LogicInc.h"

class CLogicWorldBoss
{
public:
	static int GetWorldBossArchive(unsigned bossId, Json::Value &data);
	static CDataWorldBoss* GetCDataWorldBoss();

	int GetWorldBossInfo(unsigned uid, Json::Value &data);
	int Load(unsigned bossId, unsigned uidBy, Json::Value &result);
	int Save(unsigned bossId, DataUser &userBy, Json::Value &data, Json::Value &result);
	int ViewWorldBoss(unsigned uid, unsigned bossId, Json::Value &data);

	int ExportWorldBossData(unsigned uid, unsigned bossId, const string &dir);
};

#endif
