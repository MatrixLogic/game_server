#include "DataSecinc.h"

int CDataSecinc::AddSecinc(unsigned uid, unsigned sid, const string &data)
{
	DBCREQ_DECLARE(DBC::InsertRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_INT_S(sid);
	DBCREQ_SET_STR_S(data);

	DBCREQ_EXEC;
	return 0;
}

int CDataSecinc::SetSecinc(unsigned uid, unsigned sid, const string &data)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, sid, sid);

	DBCREQ_SET_STR_V(data);
	return 0;
}

int CDataSecinc::GetSecinc(unsigned uid, unsigned sid, string &data)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, sid, sid);

	DBCREQ_GET_STR_V(data);
	return 0;
}

int CDataSecinc::RemoveSecinc(unsigned uid, unsigned sid)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, sid, sid);
	DBCREQ_EXEC;
	return 0;
}

int CDataSecinc::RemoveSecinc(unsigned uid)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_EXEC;
	return 0;
}

int CDataSecinc::GetSecinc(unsigned uid, vector<DataSecinc> &datas)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(sid);
	DBCREQ_NEED(data);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(datas);
	DBCREQ_ARRAY_GET_INT(datas, uid);
	DBCREQ_ARRAY_GET_INT(datas, sid);
	DBCREQ_ARRAY_GET_STR(datas, data);
	DBCREQ_ARRAY_GET_END();

	return 0;
}
