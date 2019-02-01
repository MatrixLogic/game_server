#ifndef __LOGIC_UPDATES_H__
#define __LOGIC_UPDATES_H__

#include "LogicInc.h"

class CLogicUpdates
{
public:
	int AddUpdates(unsigned uid, Json::Value &data, bool isother = false);
	int AddUpdate(unsigned uid, Json::Value &data, bool isother = false);
	int GetUpdates(unsigned uid, Json::Value &data, bool onlyother = false);
	int RemoveUpdatesBeforeTs(unsigned uid, unsigned ts);
};

#endif
