/*
 * LogiAllServerLadder.h
 *
 *  Created on: 2013-6-18
 *      Author: Administrator
 */

#ifndef LOGIALLSERVERLADDER_H_
#define LOGIALLSERVERLADDER_H_

#include "LogicInc.h"

class CLogicAllServerLadder
{
public:
	int ViewLadder(unsigned uid, Json::Value &result,int level);
	int Refresh(unsigned uid, Json::Value &result,int level);
	int ReportResult(unsigned uid, const Json::Value &data,int level);
	int ViewTopOne(Json::Value &result);

	static CDataLadder* GetCDataLadder(int level);

protected:
	//返回true：已初始化，false：不需要初始化
	void CheckLadderJson(Json::Value &ladder, bool *init = NULL);
	void RandLadderPlayers(Json::Value &ladder);
};

#endif /* LOGIALLSERVERLADDER_H_ */
