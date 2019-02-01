/*
 * LogicTopTenUser.h
 *
 *  Created on: 2013-3-6
 *      Author: Administrator
 */

#ifndef LOGICTOPTENUSER_H_
#define LOGICTOPTENUSER_H_

#include "LogicInc.h"

class CLogicTopTenUser {
public:
	CLogicTopTenUser();
	virtual ~CLogicTopTenUser();

	int GetInfo(DataTopTenUser &data);
	int SetUserLevel(unsigned uid, int level);
	int SetHeroPower(unsigned uid, int power, string& stat);
	int SetUserPlunder(unsigned uid);
	bool CheckTime(int type);
	int Reward(int type);
protected :
	CDataTopTenUser* GetCDataTopTenUser();
};

#endif /* LOGICTOPTENUSER_H_ */
