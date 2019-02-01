#include "DataBuilding.h"

int CDataBuilding::AddBuilding(unsigned uid, unsigned id, const string &data)
{
	DBCREQ_DECLARE(DBC::InsertRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_INT_S(id);
	DBCREQ_SET_STR_S(data);

	DBCREQ_EXEC;
	return 0;
}


int CDataBuilding::AddBuilding(unsigned uid, unsigned id,unsigned worldpos, const string &data)
{
	DBCREQ_DECLARE(DBC::InsertRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_INT_S(id);
	DBCREQ_SET_INT_S(worldpos);
	DBCREQ_SET_STR_S(data);

	DBCREQ_EXEC;
	return 0;
}

int CDataBuilding::SetBuilding(unsigned uid, unsigned id, const string &data)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, id, id);

	DBCREQ_SET_STR_V(data);
	return 0;
}
int CDataBuilding::GetBuilding(unsigned uid, unsigned id, string &data)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, id, id);

	DBCREQ_GET_STR_V(data);
	return 0;
}

int CDataBuilding::RemoveBuilding(unsigned uid, unsigned id)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, id, id);
	DBCREQ_EXEC;
	return 0;
}
int CDataBuilding::RemoveBuilding(unsigned uid, unsigned id,unsigned worldpos)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, id, id);
	DBCREQ_SET_CONDITION(EQ, worldpos, worldpos);
	DBCREQ_EXEC;
	return 0;
}
int CDataBuilding::RemoveBuilding(unsigned uid)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_EXEC;
	return 0;
}
int CDataBuilding::RemoveBuildingOnWordPos(unsigned uid, unsigned worldpos)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, worldpos, worldpos);
	DBCREQ_EXEC;
	return 0;
}

int CDataBuilding::GetBuilding(unsigned uid, vector<DataBuilding> &datas)
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

int CDataBuilding::GetBuilding(unsigned uid, unsigned worldpos, vector<DataBuilding> &datas)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, worldpos, worldpos);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(id);
	DBCREQ_NEED(worldpos);
	DBCREQ_NEED(data);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(datas);
	DBCREQ_ARRAY_GET_INT(datas, uid);
	DBCREQ_ARRAY_GET_INT(datas, id);
	DBCREQ_ARRAY_GET_INT(datas, worldpos);
	DBCREQ_ARRAY_GET_STR(datas, data);
	DBCREQ_ARRAY_GET_END();

	return 0;
}
int CDataBuilding::ReplaceBuilding(unsigned uid, unsigned id, const string &data)
{
	DBCREQ_DECLARE(DBC::ReplaceRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_INT_S(id);
	DBCREQ_SET_STR_S(data);

	DBCREQ_EXEC;
	return 0;
}
int CDataBuilding::ReplaceBuilding(unsigned uid, unsigned id, unsigned worldpos, const string &data)
{
	DBCREQ_DECLARE(DBC::ReplaceRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_INT_S(id);
	DBCREQ_SET_INT_S(worldpos);
	DBCREQ_SET_STR_S(data);

	DBCREQ_EXEC;
	return 0;
	return 0;
}
int CDataBuilding::GetBuilding(unsigned uid, unsigned id,unsigned worldpos, string &data)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, id, id);
	DBCREQ_SET_CONDITION(EQ, worldpos, worldpos);

	DBCREQ_GET_STR_V(data);
	return 0;
}
int CDataBuilding::GetBuilding(unsigned uid, map<unsigned, string> &datas)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(id);
	DBCREQ_NEED(data);

	DBCREQ_EXEC;

	datas.clear();
	int count = m_dbcret.TotalRows();
	for(int i = 0; i < count; i++)
	{
		DBCREQ_IFFETCHROW;
		datas[m_dbcret.IntValue(1)] = m_dbcret.StringValue(2);
	}
	return 0;
}


int CDataBuilding::GetBuilding(unsigned uid, unsigned worldpos,map<unsigned, string> &datas)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, worldpos, worldpos);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(id);
	DBCREQ_NEED(data);

	DBCREQ_EXEC;

	datas.clear();
	int count = m_dbcret.TotalRows();
	for(int i = 0; i < count; i++)
	{
		DBCREQ_IFFETCHROW;
		datas[m_dbcret.IntValue(1)] = m_dbcret.StringValue(2);
	}
	return 0;
}
