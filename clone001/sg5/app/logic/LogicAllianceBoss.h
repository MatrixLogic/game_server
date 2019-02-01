#ifndef _LOGIC_ALLIANCE_BOSS_H_
#define _LOGIC_ALLIANCE_BOSS_H_

#include "LogicInc.h"


class CLogicAllianceBoss
{
public:
	static int GetAllianceBossArchive(unsigned bossId, Json::Value &data);
	static CDataAllianceBoss* GetCDataAllianceBoss();

	int GetAllianceBossInfo1(unsigned uid, Json::Value &data,unsigned vip_gr,unsigned ts);
	int GetAllianceBossInfo(unsigned uid, Json::Value &data);
	int GetLastAllianceBossInfo(unsigned uid, Json::Value &data);
	int Load(unsigned bossId, unsigned uidBy, Json::Value &result);
	int Save(unsigned bossId, DataUser &userBy, Json::Value &data, Json::Value &result);
	int ViewAllianceBoss(unsigned uid, unsigned bossId, Json::Value &data);
	int GetAllianceJsonUpdates(unsigned uid, unsigned allianceId, Json::Value &value,unsigned ts,unsigned vip_gr);

};

#endif
