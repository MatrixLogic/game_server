/*
 * DataBlackUser.cpp
 *
 *  Created on: 2013-4-22
 *      Author: Administrator
 */

#include "DataBlackUser.h"

int CDataBlackUser::GetBlackUser(const string &open_id, DataBlackUser &data)
{
	unsigned crc = CCRC::GetCrc32(open_id);
	if (crc > UID_MAX)
	{
		crc %= UID_MAX;
		if (crc < UID_MIN)
		{
			crc += UID_MIN;
		}
	}
	DBCREQ_DECLARE(DBC::GetRequest, crc);
	DBCREQ_SET_KEY(open_id.c_str());

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(open_id);
	DBCREQ_NEED(block_time);
	DBCREQ_NEED(close_reason);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_STR(data,open_id);
	DBCREQ_GET_INT(data,block_time);
	DBCREQ_GET_STR(data,close_reason);

	return 0;
}

int CDataBlackUser::SetBlackUser(const DataBlackUser &data)
{
	unsigned crc = CCRC::GetCrc32(data.open_id);
	if (crc > UID_MAX)
	{
		crc %= UID_MAX;
		if (crc < UID_MIN)
		{
			crc += UID_MIN;
		}
	}
	DBCREQ_DECLARE(DBC::ReplaceRequest, crc);
	DBCREQ_SET_KEY(data.open_id.c_str());
	DBCREQ_SET_INT(data,block_time);
	DBCREQ_SET_STR(data,close_reason);

	DBCREQ_EXEC;

	return 0;
}

int CDataBlackUser::AddBlackUser(const DataBlackUser &data)
{
	unsigned crc = CCRC::GetCrc32(data.open_id);
	if (crc > UID_MAX)
	{
		crc %= UID_MAX;
		if (crc < UID_MIN)
		{
			crc += UID_MIN;
		}
	}
	DBCREQ_DECLARE(DBC::InsertRequest, crc);
	DBCREQ_SET_KEY(data.open_id.c_str());
	DBCREQ_SET_INT(data,block_time);
	DBCREQ_SET_STR(data,close_reason);

	DBCREQ_EXEC;

	return 0;
}
