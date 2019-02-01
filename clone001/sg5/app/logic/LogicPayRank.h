/*
 * LogicPayRank.h
 *
 *  Created on: 2012-11-7
 *      Author: Administrator
 */

#ifndef LOGICPAYRANK_H_
#define LOGICPAYRANK_H_
#include "LogicInc.h"
class CLogicPayRank {
public:
	CLogicPayRank();
	virtual ~CLogicPayRank();
public:
	static CDataPayRank* GetPayRankData();
	int GetPayRank(int unsigned uid, PayItem vecPoints[]);
	int GetPayRankJson(int unsigned uid, Json::Value &result);
	int UpdatePayRank(int unsigned uid, const PayItem &points);
	int PayRankReward(void);
};
#endif


