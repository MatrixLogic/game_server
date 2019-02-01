/*
 * DataUserInteract.h
 *
 *  Created on: 2011-7-13
 *      Author: dada
 */

#ifndef DATAUSERINTERACT_H_
#define DATAUSERINTERACT_H_

#include "Kernel.h"

struct DataUserInteract
{
	unsigned uid;
	unsigned opposite_uid;
	int help_from;
	int help_to;
	int attack_from;
	int attack_to;
	int retaliate_count;
	unsigned last_attack_time;
	unsigned truce_time;
};

class CDataUserInteract : public CDataBaseDBC
{
public:
	CDataUserInteract(int table = DB_USER_INTERACT) : CDataBaseDBC(table) {}

public:

	int AddInteract(const DataUserInteract &interact);
	int SetInteract(const DataUserInteract &interact);
	int GetInteract(unsigned uid, unsigned opposite_uid, DataUserInteract &interact);
	int GetInteracts(unsigned uid, vector<DataUserInteract> &interacts);
	int GetInteracts(unsigned uid, map<unsigned, DataUserInteract> &interacts);	//opposite_uid -> DataUserInteract
	int GetInteractsAttackAfter(unsigned uid, unsigned last_attack_time, vector<DataUserInteract> &interacts);
	int RemoveInteracts(unsigned uid);
	int RemoveInteracts(unsigned uid, unsigned opposite_uid);
};

#endif /* DATAUSERINTERACT_H_ */
