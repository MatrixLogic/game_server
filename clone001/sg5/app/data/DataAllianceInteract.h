/*
 * DataAllianceInteract.h
 *
 *  Created on: 2013-4-18
 *      Author: Administrator
 */

#ifndef DATAALLIANCEINTERACT_H_
#define DATAALLIANCEINTERACT_H_

#include "Kernel.h"
#include "DataUserInteract.h"

class CDataAllianceInteract : public CDataBaseDBC
{
public:
	CDataAllianceInteract(int table = DB_ALLIANCE_INTERACT) : CDataBaseDBC(table) {}

public:
	int AddAllianceInteract(const DataUserInteract &interact);
	int SetAllianceInteract(const DataUserInteract &interact);
	int GetAllianceInteract(unsigned uid, unsigned opposite_uid, DataUserInteract &interact);
	int GetAllianceInteracts(unsigned uid, vector<DataUserInteract> &interacts);
	int GetAllianceInteracts(unsigned uid, map<unsigned, DataUserInteract> &interacts);
	int GetAllianceInteractsAttackAfter(unsigned uid, unsigned last_attack_time, vector<DataUserInteract> &interacts);
	int RemoveAllianceInteracts(unsigned uid);
	int RemoveAllianceInteracts(unsigned uid, unsigned opposite_uid);
};

#endif /* DATAALLIANCEINTERACT_H_ */
