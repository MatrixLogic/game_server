#ifndef __LOGIC_HELPHISTORY_H__
#define __LOGIC_HELPHISTORY_H__

#include "LogicInc.h"

class CLogicHelpHist
{
public:
	int AddHelpHist(unsigned uid, const Json::Value &data);
	int GetHelpHist(unsigned uid, Json::Value &data);
	int AddTributeHist(unsigned uid, unsigned uidBy, unsigned ts);
	int CheckHelpHistory(unsigned uid, int addNum);
};

#endif
