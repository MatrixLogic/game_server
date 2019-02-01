/*
 * LogicUserRank.h
 *
 *  Created on: 2013-2-4
 *      Author: colin
 */

#ifndef LOGICUSERRANK_H_
#define LOGICUSERRANK_H_

#include "LogicInc.h"

class CLogicUserRank
{
public:
	int UpdateLootsPowerMcity(unsigned uid, int r1, int r2, int r3, int r4, int r5, int power, int mcity);
	int UpdatePassGateNums(unsigned uid, unsigned num);
	int UpdatePowerMcity(unsigned uid, const int power, const int mcity);
	int UpdateLoots(unsigned uid, const int rAll_loots);
	int GetUserPowerMcity(unsigned uid, unsigned& power, unsigned& mcity);
	int SetUserPowerMcity(unsigned uid, const unsigned power, const unsigned mcity);

};
#endif /* LOGICUSERRANK_H_ */
