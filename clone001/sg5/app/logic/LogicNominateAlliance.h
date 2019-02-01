#ifndef _LOGIC_NOMINATE_ALLIANCE_H_
#define _LOGIC_NOMINATE_ALLIANCE_H_

#include "LogicInc.h"

class CLogicNominateAlliance
{
public:
	static CDataNominateAlliance* GetCDataNominateAlliance();
	int GetAlliance(unsigned kingdom, Json::Value &data);
	int GetAlliance(unsigned aid, int &sign);
	int SetAlliance(unsigned alliance_id, int flag);
	int CheckAlliance(unsigned uid);
	int RemoveAlliance(unsigned aid);
};

#endif
