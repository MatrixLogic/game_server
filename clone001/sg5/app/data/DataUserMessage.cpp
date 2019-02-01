#include "DataUserMessage.h"

int CDataUserMessage::AddUserMessage(const DataUserMessage &data)
{
	DBCREQ_DECLARE(DBC::InsertRequest, data.uid);
	DBCREQ_SET_KEY(data.uid);

	DBCREQ_SET_INT(data, message_id);
	DBCREQ_SET_INT(data, opposite_uid);
	DBCREQ_SET_INT(data, type);
	DBCREQ_SET_INT(data, toward);
	DBCREQ_SET_INT(data, status);
	DBCREQ_SET_INT(data, time);
	DBCREQ_EXEC;
	return 0;
}

int CDataUserMessage::SetUserMessage(const DataUserMessage &data)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, data.uid);
	DBCREQ_SET_KEY(data.uid);
	DBCREQ_SET_CONDITION(EQ, message_id, data.message_id);
	DBCREQ_SET_INT(data, message_id);
	DBCREQ_SET_INT(data, opposite_uid);
	DBCREQ_SET_INT(data, type);
	DBCREQ_SET_INT(data, toward);
	DBCREQ_SET_INT(data, status);
	DBCREQ_SET_INT(data, time);
	DBCREQ_EXEC;
	return 0;
}

int CDataUserMessage::SetUserMessageStatus(unsigned uid, uint64_t message_id, int status)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, message_id, message_id);
	DBCREQ_SET_INT_V(status);
	return 0;
}

int CDataUserMessage::GetUserMessage(unsigned uid, uint64_t message_id, DataUserMessage &data)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, message_id, message_id);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(message_id);
	DBCREQ_NEED(opposite_uid);
	DBCREQ_NEED(type);
	DBCREQ_NEED(toward);
	DBCREQ_NEED(status);
	DBCREQ_NEED(time);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(data, uid);
	DBCREQ_GET_INT(data, message_id);
	DBCREQ_GET_INT(data, opposite_uid);
	DBCREQ_GET_INT(data, type);
	DBCREQ_GET_INT(data, toward);
	DBCREQ_GET_INT(data, status);
	DBCREQ_GET_INT(data, time);

	return 0;
}

int CDataUserMessage::GetUserMessage(unsigned uid, int type, int status,
		unsigned startTime, vector<DataUserMessage> &datas)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(GT, time, startTime);
	DBCREQ_SET_CONDITION(EQ, type, type);
	DBCREQ_SET_CONDITION(EQ, status, status);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(message_id);
	DBCREQ_NEED(opposite_uid);
	DBCREQ_NEED(type);
	DBCREQ_NEED(toward);
	DBCREQ_NEED(status);
	DBCREQ_NEED(time);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(datas);
	DBCREQ_ARRAY_GET_INT(datas, uid);
	DBCREQ_ARRAY_GET_INT(datas, message_id);
	DBCREQ_ARRAY_GET_INT(datas, opposite_uid);
	DBCREQ_ARRAY_GET_INT(datas, type);
	DBCREQ_ARRAY_GET_INT(datas, toward);
	DBCREQ_ARRAY_GET_INT(datas, status);
	DBCREQ_ARRAY_GET_INT(datas, time);
	DBCREQ_ARRAY_GET_END();

	return 0;
}

int CDataUserMessage::GetUserMessageForTime(unsigned uid, int type, unsigned startTime, vector<DataUserMessage> &datas)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(GT, time, startTime);
	DBCREQ_SET_CONDITION(EQ, type, type);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(message_id);
	DBCREQ_NEED(opposite_uid);
	DBCREQ_NEED(type);
	DBCREQ_NEED(toward);
	DBCREQ_NEED(status);
	DBCREQ_NEED(time);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(datas);
	DBCREQ_ARRAY_GET_INT(datas, uid);
	DBCREQ_ARRAY_GET_INT(datas, message_id);
	DBCREQ_ARRAY_GET_INT(datas, opposite_uid);
	DBCREQ_ARRAY_GET_INT(datas, type);
	DBCREQ_ARRAY_GET_INT(datas, toward);
	DBCREQ_ARRAY_GET_INT(datas, status);
	DBCREQ_ARRAY_GET_INT(datas, time);
	DBCREQ_ARRAY_GET_END();

	return 0;
}

int CDataUserMessage::GetUserMessageForTime(unsigned uid, int type, int toward, unsigned startTime, vector<DataUserMessage> &datas)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(GT, time, startTime);
	DBCREQ_SET_CONDITION(EQ, type, type);
	DBCREQ_SET_CONDITION(EQ, toward, toward);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(message_id);
	DBCREQ_NEED(opposite_uid);
	DBCREQ_NEED(type);
	DBCREQ_NEED(toward);
	DBCREQ_NEED(status);
	DBCREQ_NEED(time);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(datas);
	DBCREQ_ARRAY_GET_INT(datas, uid);
	DBCREQ_ARRAY_GET_INT(datas, message_id);
	DBCREQ_ARRAY_GET_INT(datas, opposite_uid);
	DBCREQ_ARRAY_GET_INT(datas, type);
	DBCREQ_ARRAY_GET_INT(datas, toward);
	DBCREQ_ARRAY_GET_INT(datas, status);
	DBCREQ_ARRAY_GET_INT(datas, time);
	DBCREQ_ARRAY_GET_END();

	return 0;
}
