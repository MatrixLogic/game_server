#include "DataInventory.h"

int CDataInventory::AddItem(unsigned uid, const string &type, unsigned count)
{
	DBCREQ_DECLARE(DBC::InsertRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_STR_S(type);
	DBCREQ_SET_INT_S(count);
	DBCREQ_EXEC;
	return 0;
}

int CDataInventory::SetItem(unsigned uid, const string &type, unsigned count)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, type, type.c_str());
	DBCREQ_SET_INT_V(count);
	return 0;
}

int CDataInventory::GetItem(unsigned uid, const string &type, unsigned &count)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, type, type.c_str());
	DBCREQ_GET_INT_V(count);
	return 0;
}

int CDataInventory::GetItems(unsigned uid, map<string, unsigned> &items)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(type);
	DBCREQ_NEED(count);

	DBCREQ_EXEC;

	items.clear();
	int count = m_dbcret.TotalRows();
	for(int i = 0; i < count; i++)
	{
		DBCREQ_IFFETCHROW;
		items[m_dbcret.StringValue(1)] = m_dbcret.IntValue(2);
	}
	return 0;
}

int CDataInventory::RemoveItem(unsigned uid, const string &type)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, type, type.c_str());
	DBCREQ_EXEC;
	return 0;
}

int CDataInventory::RemoveItems(unsigned uid)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_EXEC;
	return 0;
}
