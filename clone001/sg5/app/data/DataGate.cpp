#include "DataGate.h"

int CDataGate::AddGate(unsigned uid, unsigned id, const string &data)
{
	DBCREQ_DECLARE(DBC::InsertRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_INT_S(id);
	DBCREQ_SET_STR_S(data);

	DBCREQ_EXEC;
	return 0;
}
int CDataGate::SetGate(unsigned uid, unsigned id, const string &data)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, id, id);

	DBCREQ_SET_STR_V(data);
	return 0;
}
int CDataGate::GetGate(unsigned uid, unsigned id, string &data)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, id, id);

	DBCREQ_GET_STR_V(data);
	return 0;
}

int CDataGate::RemoveGate(unsigned uid, unsigned id)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, id, id);
	DBCREQ_EXEC;
	return 0;
}

int CDataGate::RemoveGate(unsigned uid)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_EXEC;
	return 0;
}

int CDataGate::GetGate(unsigned uid, vector<DataGate> &datas)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(id);
	DBCREQ_NEED(data);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(datas);
	DBCREQ_ARRAY_GET_INT(datas, uid);
	DBCREQ_ARRAY_GET_INT(datas, id);
	DBCREQ_ARRAY_GET_STR(datas, data);
	DBCREQ_ARRAY_GET_END();

	return 0;
}

int CDataGate::GetGate(unsigned uid, unsigned start, unsigned end, vector<DataGate> &datas)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(GE, id, start);
	DBCREQ_SET_CONDITION(LE, id, end);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(id);
	DBCREQ_NEED(data);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(datas);
	DBCREQ_ARRAY_GET_INT(datas, uid);
	DBCREQ_ARRAY_GET_INT(datas, id);
	DBCREQ_ARRAY_GET_STR(datas, data);
	DBCREQ_ARRAY_GET_END();

	return 0;
}

int CDataGate::ReplaceGate(unsigned uid, unsigned id, const string &data)
{
	DBCREQ_DECLARE(DBC::ReplaceRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_INT_S(id);
	DBCREQ_SET_STR_S(data);

	DBCREQ_EXEC;
	return 0;
}

int CDataGate::GetGateIds(unsigned uid, vector<unsigned> &ids)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(id);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(ids);
	DBCREQ_ARRAY_GET_INT_S(ids, id);
	DBCREQ_ARRAY_GET_END();

	return 0;
}
