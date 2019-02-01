/*
 * DataMatchInstance.cpp
 *
 *  Created on: 2013-4-23
 *      Author: Administrator
 */

#include "DataMatchInstance.h"

int CDataMatchInstance::AddInstance(const DataInstance &data)
{
	int serverid = 0;
	Config::GetDB(serverid);

	DBCREQ_DECLARE(DBC::InsertRequest, data.uid);
	DBCREQ_SET_KEY(data.uid);

	DBCREQ_SET_INT(data, id);
	DBCREQ_SET_INT(data, count);
	DBCREQ_SET_INT(data, lastts);
	DBCREQ_SET_INT(data, win);
	DBCREQ_SET_INT(data, damage);
	DBCREQ_SET_STR(data, data);
	DBCREQ_SET_INT_S(serverid);

	DBCREQ_EXEC;
	return 0;
}

int CDataMatchInstance::SetInstance(unsigned uid, unsigned id, const DataInstance &data)
{
	int serverid = 0;
	Config::GetDB(serverid);

	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, id, id);
	DBCREQ_SET_CONDITION(EQ, serverid, serverid);

	DBCREQ_SET_INT(data, count);
	DBCREQ_SET_INT(data, lastts);
	DBCREQ_SET_INT(data, win);
	DBCREQ_SET_INT(data, damage);
	DBCREQ_SET_STR(data, data);

	DBCREQ_EXEC;
	return 0;
}

int CDataMatchInstance::GetInstance(unsigned uid, unsigned id, DataInstance &data)
{
	int serverid = 0;
	Config::GetDB(serverid);

	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, id, id);
	DBCREQ_SET_CONDITION(EQ, serverid, serverid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(id);
	DBCREQ_NEED(count);
	DBCREQ_NEED(lastts);
	DBCREQ_NEED(win);
	DBCREQ_NEED(damage);
	DBCREQ_NEED(data);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(data, uid);
	DBCREQ_GET_INT(data, id);
	DBCREQ_GET_INT(data, count);
	DBCREQ_GET_INT(data, lastts);
	DBCREQ_GET_INT(data, win);
	DBCREQ_GET_INT(data, damage);
	DBCREQ_GET_STR(data, data);

	return 0;
}

int CDataMatchInstance::RemoveInstance(unsigned uid, unsigned id)
{
	int serverid = 0;
	Config::GetDB(serverid);

	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, id, id);
	DBCREQ_SET_CONDITION(EQ, serverid, serverid);
	DBCREQ_EXEC;
	return 0;
}

int CDataMatchInstance::RemoveInstance(unsigned uid)
{
	int serverid = 0;
	Config::GetDB(serverid);
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, serverid, serverid);
	DBCREQ_EXEC;
	return 0;
}

int CDataMatchInstance::GetInstanceLimit(unsigned uid, unsigned id, DataInstance &data)
{
	int serverid = 0;
	Config::GetDB(serverid);
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, id, id);
	DBCREQ_SET_CONDITION(EQ, serverid, serverid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(id);
	DBCREQ_NEED(count);
	DBCREQ_NEED(lastts);
	DBCREQ_NEED(win);
	DBCREQ_NEED(damage);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(data, uid);
	DBCREQ_GET_INT(data, id);
	DBCREQ_GET_INT(data, count);
	DBCREQ_GET_INT(data, lastts);
	DBCREQ_GET_INT(data, win);
	DBCREQ_GET_INT(data, damage);

	return 0;
}

int CDataMatchInstance::GetInstanceLimit(unsigned uid, vector<DataInstance> &datas)
{
	int serverid = 0;
	Config::GetDB(serverid);
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, serverid, serverid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(id);
	DBCREQ_NEED(count);
	DBCREQ_NEED(lastts);
	DBCREQ_NEED(win);
	DBCREQ_NEED(damage);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(datas);
	DBCREQ_ARRAY_GET_INT(datas, uid);
	DBCREQ_ARRAY_GET_INT(datas, id);
	DBCREQ_ARRAY_GET_INT(datas, count);
	DBCREQ_ARRAY_GET_INT(datas, lastts);
	DBCREQ_ARRAY_GET_INT(datas, win);
	DBCREQ_ARRAY_GET_INT(datas, damage);
	DBCREQ_ARRAY_GET_END();

	return 0;
}

int CDataMatchInstance::SetInstanceLimit(unsigned uid, unsigned id, const DataInstance &data)
{
	int serverid = 0;
	Config::GetDB(serverid);
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, id, id);
	DBCREQ_SET_CONDITION(EQ, serverid, serverid);

	DBCREQ_SET_INT(data, count);
	DBCREQ_SET_INT(data, lastts);
	DBCREQ_SET_INT(data, win);
	DBCREQ_SET_INT(data, damage);

	DBCREQ_EXEC;
	return 0;
}

int CDataMatchInstance::ReplaceInstance(const DataInstance &data)
{
	int serverid = 0;
	Config::GetDB(serverid);
	DBCREQ_DECLARE(DBC::ReplaceRequest, data.uid);
	DBCREQ_SET_KEY(data.uid);
	//DBCREQ_SET_CONDITION(EQ, serverid, serverid);

	DBCREQ_SET_INT(data, id);
	DBCREQ_SET_INT_S(serverid);
	DBCREQ_SET_INT(data, count);
	DBCREQ_SET_INT(data, lastts);
	DBCREQ_SET_INT(data, win);
	DBCREQ_SET_INT(data, damage);
	DBCREQ_SET_STR(data, data);

	DBCREQ_EXEC;
	return 0;
}

