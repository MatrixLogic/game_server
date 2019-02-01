/*
 * DataUserRank.h
 *
 *  Created on: 2013-1-22
 *      Author: colin
 */

#ifndef DATAUSERRANK_H_
#define DATAUSERRANK_H_

#include "Kernel.h"

struct DataUserRank
{
	unsigned uid;
	unsigned power;
	unsigned basePoints;
	unsigned passNums;
	unsigned gainRes;
	DataUserRank& operator=(const DataUserRank &other)
	{
		uid = other.uid;
		power = other.power;
		basePoints = other.basePoints;
		passNums = other.passNums;
		gainRes = other.gainRes;
		return *this;
	}
};

class CDataUserRank : public CDataBaseDBC
{
public:
	CDataUserRank(int table = DB_USER_RANK) : CDataBaseDBC(table) {}

public:
	int AddUserRank(const DataUserRank &dataUserRank);
	int SetUserRank(unsigned uid, const DataUserRank &dataUserRank);
	int GetUserRank(unsigned uid, DataUserRank &dataUserRank);
	int RemoveUser(unsigned uid);

//	int UpdateLootsPowerMcity(unsigned uid, int r1, int r2, int r3, int r4, int r5, int power, int mcity);
//	int UpdatePassGateNums(unsigned uid, unsigned num);
//	int UpdatePowerMcity(unsigned uid, const int power, const int mcity);
//	int UpdateLoots(unsigned uid, const int rAll_loots);
//
//
//	int GetUserPowerMcity(unsigned uid, unsigned& power, unsigned& mcity);
//	int SetUserPowerMcity(unsigned uid, const unsigned power, const unsigned mcity);
//	int GetUserGate(unsigned uid, unsigned& gate);
//	int GetUserLoots(unsigned uid, unsigned& loots);
};

#endif /* DATAUSERRANK_H_ */
