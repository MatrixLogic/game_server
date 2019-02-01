/*
 * CDataPointsRank.h
 *
 *  Created on: 2012-10-16
 *      Author: Administrator
 */

#ifndef CDATAPOINTSRANK_H_
#define CDATAPOINTSRANK_H_
#include "Kernel.h"
#define RANK_SIZE 10
struct DataPoints{
	int unsigned uid;
	int unsigned points;
	char name[32];
	byte reward;
public:
	DataPoints &operator =( const DataPoints &other )
	{
		uid = other.uid;
		points = other.points;
		memcpy(name,other.name,sizeof(name) - 1);
		reward = other.reward;
		return *this;
	}
};
struct PointsRank{
	DataPoints pointRank[RANK_SIZE + 1];
};
class CDataPointsRank {
public:
	CDataPointsRank();
	virtual ~CDataPointsRank();
	int Init(const std::string &path, semdat sem=sem_pointsrank);
	int UpdateRankList(const  DataPoints &point);
	int GetRankList( DataPoints vecPoints[]);
	int SetReward(unsigned uid, byte reward);
	int CleanRankList(void);
private:
	bool m_init;
	CShareMemory m_sh;
};

#endif /* CDATAPOINTSRANK_H_ */
