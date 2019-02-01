/*
 * DataTopTenUser.h
 *
 *  Created on: 2013-3-6
 *      Author: Administrator
 */

#ifndef DATATOPTENUSER_H_
#define DATATOPTENUSER_H_

#include "Kernel.h"

#define MAX_LIST_SIZE 10
struct DataUserLevel
{
	unsigned uid;
	int level;
	DataUserLevel& operator=(const DataUserLevel &data)
	{
		uid = data.uid;
		level = data.level;
		return *this;
	}
};

struct DataHeroPower
{
	unsigned uid;
	int power;

	DataHeroPower& operator=(const DataHeroPower &data)
	{
		uid = data.uid;
		power = data.power;
		return *this;
	}
};

struct DataUserPlunder
{
	unsigned uid;
	unsigned plunder;

	DataUserPlunder& operator=(const DataUserPlunder &data)
	{
		uid = data.uid;
		plunder = data.plunder;
		return *this;
	}
};


struct DataTopTenUser
{
	DataUserLevel   userLevel[MAX_LIST_SIZE+1];
	DataHeroPower   heroPower[MAX_LIST_SIZE+1];
	DataUserPlunder userPlunder[MAX_LIST_SIZE+1];
	int reward_flag;
};

enum TopTenActivityType
{
	USER_LEVEL_RANK = 1,
	HERO_POWER_RANK = 2,
	USER_LOOT_RANK = 3,
	TOP_TEN_TYPE_MAX = 10
};

class CDataTopTenUser {
public:
	CDataTopTenUser();
	virtual ~CDataTopTenUser();
public:
	int Init(const std::string &path, semdat sem=sem_toptenuser);
	int GetInfo(DataTopTenUser &data);
	int SetUserLevel(const DataUserLevel &data);
	int SetHeroPower(const DataHeroPower &data);
	int SetUserPlunder(const DataUserPlunder &data);
	int SetRewardFlage(int type);
	int GetRewardFlage(int &flage);

protected:
	bool m_init;
	CShareMemory m_sh;

};

#endif /* DATATOPTENUSER_H_ */
