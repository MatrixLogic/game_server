#ifndef __DATA_PAY_HISTORY_H__
#define __DATA_PAY_HISTORY_H__

#include "Kernel.h"

struct DataPayHistory
{
	unsigned int uid;
	unsigned long long pay_id;
	int type;
	int channel;
	string channel_pay_id;
	int credit;
	int count;
	int status;
	unsigned int time;
	string open_id;
};

class CDataPayHistory : public CDataBaseDBC
{
public:
	CDataPayHistory(int table = DB_PAY_HISTORY) : CDataBaseDBC(table) {}

	int AddPayHistory(const DataPayHistory &data);

	int GetPayHistory(unsigned uid, unsigned long long pay_id, DataPayHistory &data);
	int GetPayHistory(unsigned uid, int channel, const string &channel_pay_id, DataPayHistory &data);

	int SetPayHistoryStatus(unsigned uid, unsigned long long pay_id, char status);
	int SetPayHistoryStatus(unsigned uid, int channel, const string &channel_pay_id, char status);

	int SetPayHistory(unsigned uid, unsigned long long pay_id, const DataPayHistory &data);
	int SetPayHistory(unsigned uid, int channel, const string &channel_pay_id, const DataPayHistory &data);
};

#endif
