/*
 *LogicNationalWarRgt.h
 *
 *  Created on: 2013-08-13
 *      Author: Administrator
 */

#ifndef LOGICNATIONALWARREGISTRATION_H___
#define LOGICNATIONALWARREGISTRATION_H___
#include "LogicInc.h"

class CLogicNationalWarRgt {
public:
	CLogicNationalWarRgt();
	virtual ~CLogicNationalWarRgt();
	static CDataNationalWarRgt* GetNationalWarRgtData();
	int GetAllCityState(unsigned uid, CDataCitiesInState Citystate[]);
	int GetAllCityStateJson(unsigned uid, Json::Value &result);
	int UpdateCityState(int unsigned uid, Json::Value &input,Json::Value &result);
	int InviteAlliance(int unsigned uid, Json::Value &input,Json::Value &result);
	int CleanCityStateDate(void);
};
#endif /* LOGICNATIONALWARREGISTRATION_H___ */
