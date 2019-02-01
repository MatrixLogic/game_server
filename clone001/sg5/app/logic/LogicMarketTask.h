/*
 * LogicMarketTask.h
 *
 *  Created on: 2013-3-8
 *      Author: Administrator
 */

#ifndef LOGICMARKETTASK_H_
#define LOGICMARKETTASK_H_

#include "LogicInc.h"

class CLogicMarketTask {
public:
	int AddMarketTask(const string &openid, const string &contractid, const string &taskid, int flag);
	int SetMarketTask(const string &openid, const string &contractid, const string &taskid, int flag);
	int GetMarketTask(const string &openid, const string &contractid, string &taskid, int &flag);
	int GetMarketTask(const string &openid, vector<DataMarketTask> &data);
};

#endif /* LOGICMARKETTASK_H_ */
