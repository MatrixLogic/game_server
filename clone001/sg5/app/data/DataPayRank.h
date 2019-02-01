/*
 * DataPayRank.h
 *
 *  Created on: 2012-11-7
 *      Author: Administrator
 */

#ifndef DATAPAYRANK_H_
#define DATAPAYRANK_H_
#define PAY_RANK_SIZE 10
#include "DataInc.h"
#include "Kernel.h"
struct PayItem{
	unsigned uid;
	int payTotal;
	char name[32];
	byte reward_flag;
public:
	PayItem &operator =( const PayItem &other )
	{
		uid = other.uid;
		payTotal = other.payTotal;
		memcpy(name,other.name,sizeof(name) - 1);
		reward_flag = other.reward_flag;
		return *this;
	}
};
struct DataPayRank{
	unsigned last_ts;
	PayItem payRank[PAY_RANK_SIZE + 1];
};
class CDataPayRank {
public:
	CDataPayRank();
	int Init(string const &path, semdat sem=sem_payrank);
	int UpdateRank(const PayItem &payItem);
	int GetRank(PayItem payRank[]);
	int ViewRank();
	virtual ~CDataPayRank();

	int SetReward(unsigned uid, byte flag);
private:
	bool m_init;
	CShareMemory m_sh;
};

#endif /* DATAPAYRANK_H_ */
