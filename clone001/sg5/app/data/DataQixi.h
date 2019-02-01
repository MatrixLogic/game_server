/*
 * DataQixi.h
 *
 *  Created on: 2013-08-06
 *      Author: Administrator
 */

#ifndef CDATAQIXI_H___
#define CDATAQIXI_H___
#include "Kernel.h"
#define RANK_SIZE 10

struct DataQixiRank{
	int unsigned uid;
	int unsigned pay;
	char name[32];
public:
	DataQixiRank &operator =( const DataQixiRank &other )
	{
		uid = other.uid;
		pay = other.pay;
		memcpy(name,other.name,sizeof(name) - 1);
		return *this;
	}
};

struct DataGetWinners{
	int unsigned uid;
	int reward;
	unsigned ts;
	char name[32];

};


struct QixiAcvitityPayRank
{
	int Lotterynumber;
	unsigned ts;
	int markreward;
	int prizenum[4];
	int rewardflag;
	DataGetWinners GetRewards[4];
	DataQixiRank QixiAcvitityRank[RANK_SIZE + 1];
};

class CDataQixijie {
public:
	CDataQixijie();
	virtual ~CDataQixijie();
	int Init(const std::string &path, semdat sem=sem_qixijie);
	int UpdateRankList(const  DataQixiRank &point);
	int GetRankList( DataQixiRank vecPoints[]);
	int SetPrizeNum(const  DataGetWinners &prize);
	int GetPrizeNum(DataGetWinners vecPrize[]);
	int LotteryResults(unsigned uid,Json::Value input,int s1, int s2, int platform);
	int SetRecordLotterynumber();
	int GetRecordLotterynumber(int &num);
	int GetPrizeNum11(int *num);
	int GetMarkreward(int &markReward);
	int Setrewardflag();
	int Getrewardflag(int &rewardflag);
	int CleanRankList(void);

private:
	bool m_init;
	CShareMemory m_sh;
};

#endif /* CDATAQIXI_H_ */
