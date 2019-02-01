#ifndef _LOGIC_PERSON_MATCH_H_
#define _LOGIC_PERSON_MATCH_H_

#include "LogicInc.h"

class CLogicPersonMatch
{
public:
	static CPersonBaseMatch* GetPsersonBaseMatch();

	int GetBaseMatchInfo(unsigned uid, Json::Value &data);
	int Apply(unsigned uid);
	int ReportResult(unsigned uid, int order, int damage);
	

	int Load(unsigned instid, unsigned uidBy, Json::Value &result);
	int GetStage(int& stage);
};

#endif

