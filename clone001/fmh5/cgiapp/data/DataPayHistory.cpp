#include "DataPayHistory.h"

int CDataPayHistory::AddPayHistory(const DataPayHistory &data)
{
	DBCREQ_DECLARE(DBC::InsertRequest, data.uid);
	DBCREQ_SET_KEY(data.uid);

	DBCREQ_SET_INT(data, pay_id);
	DBCREQ_SET_INT(data, type);
	DBCREQ_SET_INT(data, channel);
	DBCREQ_SET_STR(data, channel_pay_id);
	DBCREQ_SET_INT(data, credit);
	DBCREQ_SET_INT(data, count);
	DBCREQ_SET_INT(data, status);
	DBCREQ_SET_INT(data, time);
	DBCREQ_SET_STR(data, open_id);
	DBCREQ_EXEC;
	return 0;
}

int CDataPayHistory::GetPayHistory(unsigned uid, unsigned long long pay_id, DataPayHistory &data)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, pay_id, pay_id);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(pay_id);
	DBCREQ_NEED(type);
	DBCREQ_NEED(channel);
	DBCREQ_NEED(channel_pay_id);
	DBCREQ_NEED(credit);
	DBCREQ_NEED(count);
	DBCREQ_NEED(status);
	DBCREQ_NEED(time);
	DBCREQ_NEED(open_id);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(data, uid);
	DBCREQ_GET_INT(data, pay_id);
	DBCREQ_GET_INT(data, type);
	DBCREQ_GET_INT(data, channel);
	DBCREQ_GET_STR(data, channel_pay_id);
	DBCREQ_GET_INT(data, credit);
	DBCREQ_GET_INT(data, count);
	DBCREQ_GET_INT(data, status);
	DBCREQ_GET_INT(data, time);
	DBCREQ_GET_STR(data, open_id);

	return 0;
}

int CDataPayHistory::GetPayHistory(unsigned uid, int channel, const string &channel_pay_id, DataPayHistory &data)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	//DBCREQ_SET_CONDITION(EQ, channel, channel);
	DBCREQ_SET_CONDITION(EQ, channel_pay_id, channel_pay_id.c_str());

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(pay_id);
	DBCREQ_NEED(type);
	DBCREQ_NEED(channel);
	DBCREQ_NEED(channel_pay_id);
	DBCREQ_NEED(credit);
	DBCREQ_NEED(count);
	DBCREQ_NEED(status);
	DBCREQ_NEED(time);
	DBCREQ_NEED(open_id);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(data, uid);
	DBCREQ_GET_INT(data, pay_id);
	DBCREQ_GET_INT(data, type);
	DBCREQ_GET_INT(data, channel);
	DBCREQ_GET_STR(data, channel_pay_id);
	DBCREQ_GET_INT(data, credit);
	DBCREQ_GET_INT(data, count);
	DBCREQ_GET_INT(data, status);
	DBCREQ_GET_INT(data, time);
	DBCREQ_GET_STR(data, open_id);

	return 0;
}

int CDataPayHistory::SetPayHistoryStatus(unsigned uid, unsigned long long pay_id, char status)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, pay_id, pay_id);

	DBCREQ_SET_INT_V(status);
	return 0;
}

int CDataPayHistory::SetPayHistoryStatus(unsigned uid, int channel, const string &channel_pay_id, char status)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	//DBCREQ_SET_CONDITION(EQ, channel, channel);
	DBCREQ_SET_CONDITION(EQ, channel_pay_id, channel_pay_id.c_str());

	DBCREQ_SET_INT_V(status);
	return 0;
}

int CDataPayHistory::SetPayHistory(unsigned uid, unsigned long long pay_id, const DataPayHistory &data)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, pay_id, pay_id);

	DBCREQ_SET_INT(data, type);
	DBCREQ_SET_INT(data, channel);
	DBCREQ_SET_STR(data, channel_pay_id);
	DBCREQ_SET_INT(data, credit);
	DBCREQ_SET_INT(data, count);
	DBCREQ_SET_INT(data, status);
	DBCREQ_SET_INT(data, time);
	DBCREQ_SET_STR(data, open_id);
	DBCREQ_EXEC;
	return 0;
}

int CDataPayHistory::SetPayHistory(unsigned uid, int channel, const string &channel_pay_id, const DataPayHistory &data)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	//DBCREQ_SET_CONDITION(EQ, channel, channel);
	DBCREQ_SET_CONDITION(EQ, channel_pay_id, channel_pay_id.c_str());

	DBCREQ_SET_INT(data, pay_id);
	DBCREQ_SET_INT(data, type);
	DBCREQ_SET_INT(data, credit);
	DBCREQ_SET_INT(data, count);
	DBCREQ_SET_INT(data, status);
	DBCREQ_SET_INT(data, time);
	DBCREQ_SET_STR(data, open_id);
	DBCREQ_EXEC;
	return 0;
}
