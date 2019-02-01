#include "DataPay.h"

int CDataPay::AddPay(const DataPay &pay)
{
	DBCREQ_DECLARE(DBC::InsertRequest, pay.uid);
	DBCREQ_SET_KEY(pay.uid);
	DBCREQ_SET_INT(pay, cash);
	DBCREQ_SET_INT(pay, coins);
	DBCREQ_EXEC;
	return 0;
}

int CDataPay::SetPay(unsigned uid, const DataPay &pay)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_INT(pay, cash);
	DBCREQ_SET_INT(pay, coins);
	DBCREQ_EXEC;
	return 0;
}

int CDataPay::GetPay(unsigned uid, DataPay &pay)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(cash);
	DBCREQ_NEED(coins);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(pay, uid);
	DBCREQ_GET_INT(pay, cash);
	DBCREQ_GET_INT(pay, coins);
	return 0;
}

int CDataPay::RemovePay(unsigned uid)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_EXEC;
	return 0;
}

