#include "DataAlliancePay.h"

int CDataAlliancePay::AddPay(const DataAlliancePay &pay)
{
	DBCREQ_DECLARE(DBC::InsertRequest, pay.alliance_id);
	DBCREQ_SET_KEY(pay.alliance_id);
	DBCREQ_SET_INT(pay, coins);
	DBCREQ_EXEC;
	return 0;
}

int CDataAlliancePay::SetPay(unsigned alliance_id, const DataAlliancePay &pay)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, alliance_id);
	DBCREQ_SET_KEY(alliance_id);
	DBCREQ_SET_INT(pay, coins);
	DBCREQ_EXEC;
	return 0;
}

int CDataAlliancePay::GetPay(unsigned alliance_id, DataAlliancePay &pay)
{
	DBCREQ_DECLARE(DBC::GetRequest, alliance_id);
	DBCREQ_SET_KEY(alliance_id);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(alliance_id);
	DBCREQ_NEED(coins);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(pay, alliance_id);
	DBCREQ_GET_INT(pay, coins);
	return 0;
}

int CDataAlliancePay::RemovePay(unsigned alliance_id)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, alliance_id);
	DBCREQ_SET_KEY(alliance_id);
	DBCREQ_EXEC;
	return 0;
}

