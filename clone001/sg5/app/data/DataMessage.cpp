#include "DataMessage.h"

int CDataMessage::AddMessage(const DataMessage &data)
{
	DBCREQ_DECLARE(DBC::InsertRequest, data.message_id);
	DBCREQ_SET_KEY(data.message_id);

	DBCREQ_SET_INT(data, from_uid);
	DBCREQ_SET_INT(data, to_uid);
	DBCREQ_SET_INT(data, from_platform);
	DBCREQ_SET_STR(data, from_user_name);
	DBCREQ_SET_STR(data, from_photo);
	DBCREQ_SET_INT(data, to_platform);
	DBCREQ_SET_STR(data, to_user_name);
	DBCREQ_SET_STR(data, to_photo);
	DBCREQ_SET_INT(data, time);
	DBCREQ_SET_INT(data, type);
	DBCREQ_SET_INT(data, status);
	DBCREQ_SET_STR(data, content);
	DBCREQ_SET_INT(data, reply_id);
	DBCREQ_EXEC;
	return 0;
}

int CDataMessage::GetMessage(uint64_t message_id, DataMessage &data)
{
	DBCREQ_DECLARE(DBC::GetRequest, message_id);
	DBCREQ_SET_KEY(message_id);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(message_id);
	DBCREQ_NEED(from_uid);
	DBCREQ_NEED(to_uid);
	DBCREQ_NEED(from_platform);
	DBCREQ_NEED(from_user_name);
	DBCREQ_NEED(from_photo);
	DBCREQ_NEED(to_platform);
	DBCREQ_NEED(to_user_name);
	DBCREQ_NEED(to_photo);
	DBCREQ_NEED(time);
	DBCREQ_NEED(type);
	DBCREQ_NEED(status);
	DBCREQ_NEED(content);
	DBCREQ_NEED(reply_id);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(data, message_id);
	DBCREQ_GET_INT(data, from_uid);
	DBCREQ_GET_INT(data, to_uid);
	DBCREQ_GET_INT(data, from_platform);
	DBCREQ_GET_STR(data, from_user_name);
	DBCREQ_GET_STR(data, from_photo);
	DBCREQ_GET_INT(data, to_platform);
	DBCREQ_GET_STR(data, to_user_name);
	DBCREQ_GET_STR(data, to_photo);
	DBCREQ_GET_INT(data, time);
	DBCREQ_GET_INT(data, type);
	DBCREQ_GET_INT(data, status);
	DBCREQ_GET_STR(data, content);
	DBCREQ_GET_INT(data, reply_id);

	return 0;
}

int CDataMessage::SetMessage(const DataMessage &message)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, message.message_id);
	DBCREQ_SET_KEY(message.message_id);
	DBCREQ_SET_INT(message, from_uid);
	DBCREQ_SET_INT(message, to_uid);
	DBCREQ_SET_INT(message, from_platform);
	DBCREQ_SET_STR(message, from_user_name);
	DBCREQ_SET_STR(message, from_photo);
	DBCREQ_SET_INT(message, to_platform);
	DBCREQ_SET_STR(message, to_user_name);
	DBCREQ_SET_STR(message, to_photo);
	DBCREQ_SET_INT(message, time);
	DBCREQ_SET_INT(message, type);
	DBCREQ_SET_INT(message, status);
	DBCREQ_SET_STR(message, content);
	DBCREQ_SET_INT(message, reply_id);
	DBCREQ_EXEC;
	return 0;
}

int CDataMessage::RemoveMessage(uint64_t message_id)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, message_id);
	DBCREQ_SET_KEY(message_id);
	DBCREQ_EXEC;
	return 0;
}

int CDataMessage::SetMessageStatus(uint64_t message_id, int status)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, message_id);
	DBCREQ_SET_KEY(message_id);
	DBCREQ_SET_INT_V(status);
	return 0;
}

int CDataMessage::SetMessageContent(uint64_t message_id, const string &content)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, message_id);
	DBCREQ_SET_KEY(message_id);
	DBCREQ_SET_STR_V(content);
	return 0;
}
