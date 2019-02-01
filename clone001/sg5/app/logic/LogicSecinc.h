#ifndef __LOGIC_SECINC_H__
#define __LOGIC_SECINC_H__

#include "LogicInc.h"

class CLogicSecinc
{
public:
	int AddSecinc(unsigned uid, unsigned eqid, unsigned amount);
	int GetSecinc(unsigned uid, Json::Value &data);
	int ProcessSecinc(unsigned uid, const Json::Value &data);
};

#endif
