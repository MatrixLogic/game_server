/*
 * DataInviteMapping.cpp
 *
 *  Created on: 2012-9-4
 *      Author: Administrator
 */

#include "DataInviteMapping.h"

int CDataInviteMapping::AddMapping(unsigned uid, unsigned invite_uid)
{
	DBCREQ_DECLARE(DBC::InsertRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_INT_V(invite_uid);
	return 0;
}

int CDataInviteMapping::GetUid(unsigned uid, vector<unsigned > &vUid)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(invite_uid);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_ARRAY_GET_BEGIN(vUid);
	DBCREQ_ARRAY_GET_INT_S(vUid,invite_uid);
	DBCREQ_ARRAY_GET_END();
	return 0;
}


