/*
 * DataUserAcccharge.h
 *
 *  Created on: 2011-7-8
 *      Author: dada
 */

#ifndef DATAUSERACCCHARGE_H_
#define DATAUSERACCCHARGE_H_

#include "Kernel.h"

struct DataUserReward
{
	unsigned accCharge;
	int level;
};

class CDataUserAccCharge : public CDataBaseDBC
{
public:
	CDataUserAccCharge(int table = DB_USER_ACCCHARGE) : CDataBaseDBC(table) {}

public:
	int AddMapping(const string &openid, unsigned acccharge, unsigned level);
	int GetAccCharge(const string &openid, DataUserReward &data);
	int RemoveAccCharge(const string &openid);
};

#endif /* DATAUSERACCCHARGE_H_ */
