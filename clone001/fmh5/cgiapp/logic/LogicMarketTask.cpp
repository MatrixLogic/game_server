/*
 * LogicMarketTask.cpp
 *
 *  Created on: 2013-3-8
 *      Author: Administrator
 */

#include "LogicMarketTask.h"

int CLogicMarketTask::AddMarketTask(const string &openid, const string &contractid, const string &taskid, int flag)
{
	int ret = 0;
	CDataMarketTask dbMarketTask;
	DataMarketTask task;
	/*
	int temflag = 0;
	string stTaskId;
	ret = GetMarketTask(openid, contractid, stTaskId, temflag);
	if (0 == ret)
	{
		return 0;
	}
	else if(R_ERR_NO_DATA != ret)
	{
		return R_ERR_DB;
	}
	*/

	task.openid = openid;
	task.contractid = contractid;
	task.taskid = taskid;
	CDataUserMapping dbUserMapping;
	unsigned uid;
	ret = dbUserMapping.GetMapping(openid, OpenPlatform::GetType(), uid);
	if (0 != ret)
	{
		error_log("[GetMapping fail][openid=%s]", openid.c_str());
		return ret;
	}
	task.uid = uid;
	task.flag = flag;
	ret = dbMarketTask.AddTask(task);
	if (0 != ret)
	{
		error_log("[AddTask fail][ret=%d,openid=%s,uid=%u]", ret, openid.c_str(),uid);
		return ret;
	}
	return R_SUCCESS;
}

int CLogicMarketTask::SetMarketTask(const string &openid, const string &contractid, const string &taskid, int flag)
{
	int ret = 0;
	CDataMarketTask dbMarketTask;
	DataMarketTask task;
	task.openid = openid;
	task.contractid = contractid;

	CDataUserMapping dbUserMapping;
	unsigned uid;
	ret = dbUserMapping.GetMapping(openid, OpenPlatform::GetType(), uid);
	if (0 != ret)
	{
		error_log("[GetMapping fail][openid=%s]", openid.c_str());
		return ret;
	}
	task.uid = uid;
	int result = 0;
	ret = dbMarketTask.GetTask(task);
	if (R_ERR_NO_DATA == ret)
	{
		task.taskid = taskid;
		task.flag = flag;
		result = dbMarketTask.AddTask(task);
	}
	else if(0 == ret)
	{
		task.taskid = taskid;
		task.flag = flag;
		result = dbMarketTask.SetTask(task);
	}
	else
	{
		error_log("[db GetTask fail][ret=%d,openid=%s,contractid=%s]",
				ret, openid.c_str(), contractid.c_str());
		return ret;
	}

	if (0 != result)
	{
		error_log("[SetTask fail][result=%d,openid=%s,uid=%u,contractid=%d]",
				result, openid.c_str(), uid, contractid.c_str());
		return ret;
	}
	return R_SUCCESS;
}

int CLogicMarketTask::GetMarketTask(const string &openid, const string &contractid, string &taskid, int &flag)
{
	int ret = 0;
	CDataMarketTask dbMarketTask;
	DataMarketTask task;
	task.openid = openid;
	task.contractid = contractid;

	ret = dbMarketTask.GetTask(task);
	if (0 != ret)
	{
		error_log("[GetTask fail][ret=%d,openid=%s,contractid=%s]",
				ret, openid.c_str(), contractid.c_str());
		return ret;
	}
	taskid = task.taskid;
	flag = task.flag;
	return R_SUCCESS;
}

int CLogicMarketTask::GetMarketTask(const string &openid, vector<DataMarketTask> &data)
{
	int ret = 0;
	CDataMarketTask dbMarketTask;
	DataMarketTask task;
	ret = dbMarketTask.GetTask(openid, data);
	if(0 != ret)
	{
		error_log("[db_get_marketTask fail][ret=%d,openid=%s]", ret,  openid.c_str());
		return ret;
	}
	return 0;
}
