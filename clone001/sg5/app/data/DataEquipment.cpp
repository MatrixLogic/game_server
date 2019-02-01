#include "DataEquipment.h"

int CDataEquipment::AddEquipment(unsigned uid, unsigned id, const string &data)
{
	DBCREQ_DECLARE(DBC::InsertRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_INT_S(id);
	DBCREQ_SET_STR_S(data);

	DBCREQ_EXEC;
	return 0;
}
int CDataEquipment::SetEquipment(unsigned uid, unsigned id, const string &data)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, id, id);

	DBCREQ_SET_STR_V(data);
	return 0;
}
int CDataEquipment::GetEquipment(unsigned uid, unsigned id, string &data)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, id, id);

	DBCREQ_GET_STR_V(data);
	return 0;
}

int CDataEquipment::RemoveEquipment(unsigned uid, unsigned id)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, id, id);
	DBCREQ_EXEC;
	return 0;
}

int CDataEquipment::RemoveEquipment(unsigned uid)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_EXEC;
	return 0;
}

int CDataEquipment::GetEquipment(unsigned uid, vector<DataEquipment> &datas)
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

int CDataEquipment::ReplaceEquipment(unsigned uid, unsigned id, const string &data)
{
	DBCREQ_DECLARE(DBC::ReplaceRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_INT_S(id);
	DBCREQ_SET_STR_S(data);

	DBCREQ_EXEC;
	return 0;
}
