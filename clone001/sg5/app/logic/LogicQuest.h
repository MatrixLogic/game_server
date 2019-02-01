#ifndef __LOGIC_QUEST_H__
#define __LOGIC_QUEST_H__

#include "LogicInc.h"

class CLogicQuest
{
public:
	int GetQuest(unsigned uid, Json::Value &data);
	int UpdateQuest(unsigned uid, const Json::Value &data);
};

#endif
