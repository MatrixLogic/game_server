/*
 * DataPromotionMapping.cpp
 *
 *  Created on: 2012-12-5
 *      Author: Administrator
 */

#include "DataPromotionMapping.h"



CDataPromotionMapping::~CDataPromotionMapping() {
	// TODO Auto-generated destructor stub
}

int CDataPromotionMapping::AddPromotionMapping( const DataPromotionMapping &data )
{
	DBCREQ_DECLARE(DBC::InsertRequest, data.uid);
	DBCREQ_SET_KEY(data.uid);
	DBCREQ_SET_INT(data, pid);
	DBCREQ_SET_INT(data, usetime);
	DBCREQ_SET_INT(data, useflag);
	DBCREQ_EXEC;
	return 0;
}
int CDataPromotionMapping::SetPromotionMapping( const DataPromotionMapping &data )
{
	DBCREQ_DECLARE(DBC::UpdateRequest, data.uid);
	DBCREQ_SET_KEY(data.uid);
	DBCREQ_SET_INT(data, pid);
	DBCREQ_SET_INT(data, usetime);
	DBCREQ_SET_INT(data, useflag);
	DBCREQ_EXEC;
	return 0;

}
int CDataPromotionMapping::GetPromotionMapping( DataPromotionMapping &data )
{
	DBCREQ_DECLARE(DBC::GetRequest, data.uid);
	DBCREQ_SET_KEY(data.uid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(pid);
	DBCREQ_NEED(usetime);
	DBCREQ_NEED(useflag);
	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(data, uid);
	DBCREQ_GET_INT(data, pid);
	DBCREQ_GET_INT(data, usetime);
	DBCREQ_GET_INT(data, useflag);
	return 0;
}
