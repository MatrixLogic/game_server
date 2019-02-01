#ifndef __LOGIC_TRIBUTE_H__
#define __LOGIC_TRIBUTE_H__

#include "LogicInc.h"

class CLogicTribute
{
public:
	int InitializeTribute(unsigned uid);
	int ProcessTribute(unsigned uid, const Json::Value &tributes);
	int GetTribute(unsigned uid, Json::Value &tributes);
	int Tribute(unsigned uid, unsigned tributeUid, const Json::Value &tribute);
	int IsEnableTribute(const DataUser &user, unsigned tributeUid, bool &enable);
	int DaliyTribute(unsigned uid, int invite_count);
	int GetTributeLimit(unsigned uid, int invite_count);
	int GetTributeList(unsigned uid, Json::Value &tributeList);
};

#endif
