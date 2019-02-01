#ifndef _LOGIC_DRAGONBALL_H_
#define _LOGIC_DRAGONBALL_H_

#include "LogicInc.h"

class CLogicDragonball
{
public:
	int WhichDragonballHold(unsigned uid, unsigned &ballId, unsigned &ts);
	int GetDragonballs(Json::Value &data);
	int ViewAllDragonball(DragonballData &balls);
	bool EnableAttackForDragonball(unsigned ballid, unsigned level);
	int SnatchDragonball(unsigned ballid, unsigned snatchUid, const string &snatchName);
	int GetAllianceDragons(map<unsigned,AllianceBall>& allianceBalls);
	int PayAlliance(AllianceBall& aBall);
	int SetAlliance(AllianceBall& aBall);
	int ResetAlliance();
	bool IsDragonballActivityTs(void);
protected:
	CDataDragonball* GetCDataDragonball();
};

#endif
