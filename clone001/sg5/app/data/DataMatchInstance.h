/*
 * DataMatchInstance.h
 *
 *  Created on: 2013-4-23
 *      Author: Administrator
 */

#ifndef DATAMATCHINSTANCE_H_
#define DATAMATCHINSTANCE_H_

#include "Kernel.h"
#include "DataInstance.h"

class CDataMatchInstance : public CDataBaseDBC
{
public:
	CDataMatchInstance(int tableId = DB_MATCH_INSTANCE) : CDataBaseDBC(tableId){};

public:
	int AddInstance(const DataInstance &data);
	int SetInstance(unsigned uid, unsigned id, const DataInstance &data);
	int GetInstance(unsigned uid, unsigned id, DataInstance &data);
	int RemoveInstance(unsigned uid, unsigned id);
	int RemoveInstance(unsigned uid);
	int GetInstanceLimit(unsigned uid, unsigned id, DataInstance &data);
	int GetInstanceLimit(unsigned uid, vector<DataInstance> &datas);
	int SetInstanceLimit(unsigned uid, unsigned id, const DataInstance &data);
	int ReplaceInstance(const DataInstance &data);
};

#endif /* DATAMATCHINSTANCE_H_ */
