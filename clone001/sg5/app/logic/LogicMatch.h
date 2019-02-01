#ifndef _LOGIC_MATCH_H_
#define _LOGIC_MATCH_H_

#include "LogicInc.h"

class CLogicMatch
{
public:
	static CDataMatch* GetCDataMatch();

	int GetMatchInfo(unsigned aid, unsigned uid, Json::Value &data);
	int Apply(unsigned uid);
	int ReportResult(unsigned uid, int order, int result);
	bool IsMatchProtect(unsigned aid, unsigned uid);
	int GetApplyPlayers(unsigned aid, Json::Value &result);
	int GetRegularScore(unsigned aid, Json::Value &result);
	int GetStage(int& stage);
};

#endif
