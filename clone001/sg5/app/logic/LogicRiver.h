/*
 * LogicRiver.h
 *
 *  Created on: 2013-3-20
 *      Author: Administrator
 */

#ifndef LOGICRIVER_H_
#define LOGICRIVER_H_

#include "LogicInc.h"

class CLogicRiver {
public:
	CLogicRiver();
	static CDataRiver* GetDataRiver(void);
	int SetShip(unsigned uid,const Json::Value &param);
	int GetAllShipJson(Json::Value &result);
	int RemoveShip(unsigned uid);
	int GetShipJson(unsigned uid, Json::Value &result);
	int GetShipJson(DataShip ship, Json::Value &result);
	int UpdatesEndOfShipping(DataShip ship);
	int SetAttackLog(unsigned uid, const Json::Value &logdata);
	int GetAttackLog(unsigned uid, Json::Value &logdata);
};

#endif /* LOGICRIVER_H_ */
