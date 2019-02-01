#ifndef DATAPAY_H_
#define DATAPAY_H_

#include "Kernel.h"

struct DataPay
{
	unsigned uid;
	unsigned cash;
	unsigned coins;
};

class CDataPay : public CDataBaseDBC
{
public:
	CDataPay(int table = DB_PAY) : CDataBaseDBC(table) {}

public:
	int AddPay(const DataPay &pay);
	int SetPay(unsigned uid, const DataPay &pay);
	int GetPay(unsigned uid, DataPay &pay);
	int RemovePay(unsigned uid);

};

#endif /* DATAPAY_H_ */
