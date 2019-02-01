/*
 * DataBlackUser.h
 *
 *  Created on: 2013-4-22
 *      Author: Administrator
 */

#ifndef DATABLACKUSER_H_
#define DATABLACKUSER_H_

#include "Kernel.h"

struct DataBlackUser
{
	string open_id;
	string close_reason;
	unsigned block_time;
};

class CDataBlackUser : public CDataBaseDBC
{
public:
	CDataBlackUser(int table = DB_BLACK_USER) : CDataBaseDBC(table) {}
	int GetBlackUser(const string &open_id, DataBlackUser &data);
	int SetBlackUser(const DataBlackUser &data);
	int AddBlackUser(const DataBlackUser &data);
};

#endif /* DATABLACKUSER_H_ */
