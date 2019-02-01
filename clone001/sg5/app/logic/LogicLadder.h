#ifndef _LOGIC_LADDER_H_
#define _LOGIC_LADDER_H_

#include "LogicInc.h"

class CLogicLadder
{
public:
	int ViewLadder(unsigned uid, Json::Value &result);
	int Refresh(unsigned uid, Json::Value &result);
	int ReportResult(unsigned uid, const Json::Value &data);
	int BroadcastWinNumber(unsigned uid, int win, bool top=false);

	static CDataLadder* GetCDataLadder();

protected:
	//返回true：已初始化，false：不需要初始化
	void CheckLadderJson(Json::Value &ladder, bool *init = NULL);
	void RandLadderPlayers(Json::Value &ladder);

};

#endif
