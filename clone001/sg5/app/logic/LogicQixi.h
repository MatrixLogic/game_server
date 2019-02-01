/*
 *LogicQixi.h
 *
 *  Created on: 2013-08-06
 *      Author: Administrator
 */

#ifndef LOGICQIXIJIE_H___
#define LOGICQIXIJIE_H___
#include "LogicInc.h"
class CLogicQixijie {
public:
	CLogicQixijie();
	virtual ~CLogicQixijie();
	static CDataQixijie* GetQixijieData();
	int GetQixiActivityRank(unsigned uid, DataQixiRank vecPoints[]);
	int GetQixiActivityRankJson(unsigned uid, Json::Value &result);
	int UpdateQixiActivityRank(int unsigned uid, Json::Value &input);
	int RewardQixiActivityRank(void);
	int CleanRankList(void);
};
#endif /* LOGICPOINTRANK_H_ */
