/*
 * LogicPointRank.h
 *
 *  Created on: 2012-10-17
 *      Author: Administrator
 */

#ifndef LOGICPOINTRANK_H_
#define LOGICPOINTRANK_H_
#include "LogicInc.h"
class CLogicPointsRank {
public:
	CLogicPointsRank();
	virtual ~CLogicPointsRank();
	static CDataPointsRank* GetPointsRankData();
	int GetPointsRank(int unsigned uid, DataPoints vecPoints[]);
	int GetPointsRankJson(int unsigned uid, Json::Value &result);
	int UpdatePointsRank(int unsigned uid, const DataPoints &points);
	int RewardPointRank(void);
	int CleanRankList(void);
};
#endif /* LOGICPOINTRANK_H_ */
