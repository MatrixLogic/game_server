/*
 * DataUserMapping.cpp
 *
 *  Created on: 2011-7-8
 *      Author: dada
 */

#include "DataUserAccCharge.h"
#include "crc.h"

int CDataUserAccCharge::AddMapping(const string &openid, unsigned acccharge, unsigned level)
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
	DBCREQ_DECLARE(DBC::InsertRequest, crc);
	req.SetKey(openid.c_str());
	req.Set("acccharge", acccharge);
	req.Set("level", level);
	DBCREQ_EXEC;
	return 0;
}

int CDataUserAccCharge::GetAccCharge(const string &openid, DataUserReward &data)
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
	req.SetKey(openid.c_str());

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(acccharge);
	DBCREQ_NEED(level);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(data, accCharge);
	DBCREQ_GET_INT(data, level);

	return 0;
}

int CDataUserAccCharge::RemoveAccCharge(const string &openid)
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
	DBCREQ_DECLARE(DBC::DeleteRequest, crc);
	DBCREQ_SET_KEY(openid.c_str());
	DBCREQ_EXEC;
	return 0;
}

