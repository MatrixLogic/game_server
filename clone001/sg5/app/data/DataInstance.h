#ifndef __DATA_INSTANCE_H__
#define __DATA_INSTANCE_H__

#include "Kernel.h"

struct DataInstance
{
	unsigned uid;
	unsigned id;
	unsigned count;
	unsigned lastts;
	int      win;
	int      damage;
	string   data;
};

class CDataInstance : public CDataBaseDBC
{
public:
	CDataInstance(int table = DB_INSTANCE) : CDataBaseDBC(table) {}

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

#endif
