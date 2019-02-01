/*
 * DataRiver.h
 *
 *  Created on: 2013-3-20
 *      Author: Administrator
 */

#ifndef DATARIVER_H_
#define DATARIVER_H_

#include "Kernel.h"
#include "DataInc.h"

struct DataShip
{
	unsigned uid;
	int type;
	unsigned start_time;
	unsigned end_time;
	int posy;
	int attack_num;
	int user_level;
	char user_name[32];
	char alliance_name[32];
};

struct userNode {
	DataShip ship;
	int last;
	int next;
};

struct DataRiver {
	userNode Ships[SHIP_USER_COUNT];
	int head;
};

class CDataRiver {
public:
	CDataRiver();
	int Init(const string &path, semdat sem=sem_river);
	int AddShip(DataShip &ship);
	int RemoveShip(unsigned uid);
	int GetShips(vector<DataShip> &ships);
	int GetOneShip(unsigned uid,DataShip &ship);

protected:
	CShareMemory m_sh;
	bool m_init;
};

#endif /* DATARIVER_H_ */
