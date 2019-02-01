/*
 * DataGoods.cpp
 *
 *  Created on: 2012-9-17
 *      Author: Administrator
 */

#include "DataGoods.h"
CDataGoods::~CDataGoods() {
	// TODO Auto-generated destructor stub
}

int CDataGoods::AddGoods(DataGoods data)
{
	DBCREQ_DECLARE(DBC::InsertRequest, data.uid);
	DBCREQ_SET_KEY(data.uid);
	DBCREQ_SET_INT(data, count);
	DBCREQ_SET_INT(data, buy_time);
	DBCREQ_SET_STR(data, data);
	DBCREQ_EXEC;
	return 0;
}
//int CDataGoods::RemoveGoods(int unsigned uid,int unsigned ts)
//{
//	return 0;
//}
int CDataGoods::UpdateGoods(int unsigned uid, uint64_t id,int unsigned count)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ,id,id);
	DBCREQ_SET_INT_V(count);
	return 0;
}
int CDataGoods::GetGoods(int unsigned uid, vector<DataGoods> &vdata)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(GT,count,0);
	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(id);;
	DBCREQ_NEED(count);;
	DBCREQ_NEED(data);;

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;

	DBCREQ_ARRAY_GET_BEGIN(vdata);
	DBCREQ_ARRAY_GET_INT(vdata,id);
	DBCREQ_ARRAY_GET_INT(vdata,count);
	DBCREQ_ARRAY_GET_STR(vdata,data);

	DBCREQ_ARRAY_GET_END();

	return 0;
}
int CDataGoods::GetCountById(int unsigned uid, uint64_t id, int &count)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(NE,count,0);
	DBCREQ_SET_CONDITION(EQ,id,id);
	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(count);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;
	DBCREQ_GET_BEGIN()	;
	DBCREQ_GET_INT_S(count);
	return 0;
}
