/*
 *LogicHundredDaysActivity.h
 *
 *  Created on: 2013-07-17
 *      Author: Administrator
 */

#ifndef LOGICHUNDERDDAYSACTIVITY_H_
#define LOGICHUNDERDDAYSACTIVITY_H_
#include "LogicInc.h"
class CLogicHundredDaysActivityRank {
public:
	CLogicHundredDaysActivityRank();
	virtual ~CLogicHundredDaysActivityRank();
	static CDataHDaysAcvitityRank* GetHundredDaysActivityRankData();
	int GetHundredDaysActivityRank(int unsigned uid, DataHDaysAcvitityPay vecPoints[]);
	int GetHundredDaysActivityRankJson(unsigned uid, Json::Value &result);
	int UpdateHundredDaysActivityRank(int unsigned uid, const DataHDaysAcvitityPay &points);
//	int RewardHundredDaysActivityRank(void);
	int CleanRankList(void);
};
#endif /* LOGICPOINTRANK_H_ */
