/*
 * DataHundredDayActivity.h
 *
 *  Created on: 2013-07-17
 *      Author: Administrator
 */

#ifndef CDATAHUNDREDDAYSACTIVITY_H_
#define CDATAHUNDREDDAYSACTIVITY_H_
#include "Kernel.h"


#define RANK_SIZE 10

struct UpdatesHundDays{
	unsigned uid;
	unsigned ts;
	unsigned pay;
	int rank;
	string s;
};

struct DataHDaysAcvitityPay{
	int unsigned uid;
	int unsigned pay;
	char name[32];
	byte reward;
public:
	DataHDaysAcvitityPay &operator =( const DataHDaysAcvitityPay &other )
	{
		uid = other.uid;
		pay = other.pay;
		memcpy(name,other.name,sizeof(name) - 1);
		reward = other.reward;
		return *this;
	}
};

struct AcvitityPayRank
{
	int flag;
	int awardflag[3];
	DataHDaysAcvitityPay HDAcvitityRank[RANK_SIZE + 1];
};
class CDataHDaysAcvitityRank {
public:
	CDataHDaysAcvitityRank();
	virtual ~CDataHDaysAcvitityRank();
	int Init(const std::string &path, semdat sem=sem_hundreddaysactivity);
	int UpdateRankList(const  DataHDaysAcvitityPay &point);
	int GetRankList( DataHDaysAcvitityPay vecPoints[]);
	int RewardHundredDaysActivityRank(vector <UpdatesHundDays> &data);
	int CleanRankList(void);
private:
	bool m_init;
	CShareMemory m_sh;
};

#endif /* CDATAPOINTSRANK_H_ */
