/*
 * DataMarketTask.cpp
 *
 *  Created on: 2013-3-8
 *      Author: Administrator
 */

#include "DataMarketTask.h"

int CDataMarketTask::AddTask(DataMarketTask &data)
{
	unsigned crc = CCRC::GetCrc32(data.openid);
	if (crc > UID_MAX)
	{
		crc %= UID_MAX;
		if (crc < UID_MIN)
		{
			crc += UID_MIN;
		}
	}
	DBCREQ_DECLARE(DBC::InsertRequest, crc);
	DBCREQ_SET_KEY(data.openid.c_str());
	DBCREQ_SET_INT(data, uid);
	DBCREQ_SET_STR(data, contractid);
	DBCREQ_SET_STR(data, taskid);
	DBCREQ_SET_INT(data, flag);
	DBCREQ_EXEC;
	return 0;
}

int CDataMarketTask::GetTask(DataMarketTask &data)
{
	unsigned crc = CCRC::GetCrc32(data.openid);
	if (crc > UID_MAX)
	{
		crc %= UID_MAX;
		if (crc < UID_MIN)
		{
			crc += UID_MIN;
		}
	}
	DBCREQ_DECLARE(DBC::GetRequest, crc);
	DBCREQ_SET_KEY(data.openid.c_str());
	DBCREQ_SET_CONDITION(EQ, contractid, data.contractid.c_str());

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(flag);
	DBCREQ_NEED(taskid);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(data, uid);
	DBCREQ_GET_INT(data, flag);
	DBCREQ_GET_INT(data, taskid);
	return 0;
}

int CDataMarketTask::SetTask(DataMarketTask &data)
{

	unsigned crc = CCRC::GetCrc32(data.openid);
	if (crc > UID_MAX)
	{
		crc %= UID_MAX;
		if (crc < UID_MIN)
		{
			crc += UID_MIN;
		}
	}
	DBCREQ_DECLARE(DBC::UpdateRequest, crc);
	DBCREQ_SET_KEY(data.openid.c_str());
	DBCREQ_SET_CONDITION(EQ, contractid, data.contractid.c_str());

	DBCREQ_SET_INT(data, flag);

	DBCREQ_EXEC;
	return 0;
}

int CDataMarketTask::RemoveTask(DataMarketTask &data)
{
	unsigned crc = CCRC::GetCrc32(data.openid);
	if (crc > UID_MAX)
	{
		crc %= UID_MAX;
		if (crc < UID_MIN)
		{
			crc += UID_MIN;
		}
	}
	DBCREQ_DECLARE(DBC::DeleteRequest, crc);
	DBCREQ_SET_KEY(data.openid.c_str());
	DBCREQ_SET_CONDITION(EQ, contractid, data.contractid.c_str());
	DBCREQ_EXEC;
	return 0;
}

int CDataMarketTask::GetTask(string openid, vector<DataMarketTask> &datas)
{
	unsigned crc = CCRC::GetCrc32(openid);
	if (crc > UID_MAX)
	{
		crc %= UID_MAX;
		if (crc < UID_MIN)
		{
			crc += UID_MIN;
		}
	}
	DBCREQ_DECLARE(DBC::GetRequest, crc);
	DBCREQ_SET_KEY(openid.c_str());

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(openid);
	DBCREQ_NEED(uid);
	DBCREQ_NEED(contractid);
	DBCREQ_NEED(taskid);
	DBCREQ_NEED(flag);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(datas);
	DBCREQ_ARRAY_GET_STR(datas, openid);
	DBCREQ_ARRAY_GET_INT(datas, uid);
	DBCREQ_ARRAY_GET_STR(datas, contractid);
	DBCREQ_ARRAY_GET_STR(datas, taskid);
	DBCREQ_ARRAY_GET_INT(datas, flag);
	DBCREQ_ARRAY_GET_END();
	return 0;
}

