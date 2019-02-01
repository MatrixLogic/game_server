#ifndef DATAALLIANCEPAY_H_
#define DATAALLIANCEPAY_H_

#include "Kernel.h"

struct DataAlliancePay
{
	unsigned alliance_id;
	unsigned coins;
};

class CDataAlliancePay : public CDataBaseDBC
{
public:
	CDataAlliancePay(int table = DB_ALLIANCE_PAY) : CDataBaseDBC(table) {}

public:
	int AddPay(const DataAlliancePay &pay);
	int SetPay(unsigned alliance_id, const DataAlliancePay &pay);
	int GetPay(unsigned alliance_id, DataAlliancePay &pay);
	int RemovePay(unsigned alliance_id);

};

#endif /* DATAALLIANCEPAY_H_ */
