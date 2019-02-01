/*
 * DataMarketTask.h
 *
 *  Created on: 2013-3-8
 *      Author: Administrator
 */

#ifndef DATAMARKETTASK_H_
#define DATAMARKETTASK_H_

#include "Kernel.h"

struct DataMarketTask
{
	int flag;
	unsigned uid;
	string taskid;
	string openid;
	string contractid;

	DataMarketTask& operator=(const DataMarketTask &data)
	{
		openid = data.openid;
		uid = data.uid;
		taskid= data.taskid;
		flag = data.flag;
		contractid = data.contractid;
		return *this;
	}
};

class CDataMarketTask : public CDataBaseDBC
{
public:
	CDataMarketTask(int table = DB_MARKET_TASK) : CDataBaseDBC(table) {}
public:
	int AddTask(DataMarketTask &data);
	int GetTask(DataMarketTask &data);
	int GetTask(string openid, vector<DataMarketTask> &datas);
	int SetTask(DataMarketTask &data);
	int RemoveTask(DataMarketTask &data);
};

#endif /* DATAMARKETTASK_H_ */
