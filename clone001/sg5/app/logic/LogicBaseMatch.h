#ifndef _LOGIC_BASE_MATCH_H_
#define _LOGIC_BASE_MATCH_H_

#include "LogicInc.h"

class CLogicBaseMatch
{
public:
	static CDataBaseMatch* GetCDataBaseMatch();
	static CDataBaseMatch* GetAllServerCDataBaseMatch();

	int GetBaseMatchInfo(unsigned aid, unsigned uid, Json::Value &data,bool allsesver=false);
	int Apply(unsigned uid, bool allsesver=false);
	int ReportResult(unsigned uid, int order, int damage,bool allsesver=false);
	bool IsBaseMatchProtect(unsigned aid, unsigned uid,bool allsesver=false);
	int GetApplyPlayers(unsigned aid, Json::Value &result,bool allsesver=false);
	int GetRegularScore(unsigned aid, Json::Value &result,bool allsesver=false);
	int Load(unsigned instid, unsigned uidBy, Json::Value &result,bool allsesver=false);
	int Save(unsigned instid, DataUser &userBy, Json::Value &data, Json::Value &result,bool allsesver=false);
	int GetStage(int& stage,bool allsesver=false);
	int RequestBaseMatch(const string &url, Json::Value &data);
	bool IsAllServerBaseMatch(void);
};

#endif
