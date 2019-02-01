/*
 * LogicWeapon.h
 *
 *  Created on: 2013-3-13
 *      Author: Administrator
 */

#ifndef LOGICWEAPON_H_
#define LOGICWEAPON_H_

#include "LogicInc.h"

class CLogicWeapon {
public:
	int SetWeapon(unsigned uid, const Json::Value &data);
	int GetWeapon(unsigned uid, Json::Value &data);
};

#endif /* LOGICWEAPON_H_ */
