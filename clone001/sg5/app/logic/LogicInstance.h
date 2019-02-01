#ifndef __LOGIC_INSTANCE_H__
#define __LOGIC_INSTANCE_H__

#include "LogicInc.h"

class CLogicInstance
{
public:
	int GetInstanceInfoJson(unsigned uid, Json::Value &data);
	int Load(unsigned instanceId, unsigned uidBy, bool restart, Json::Value &result);
	int Save(unsigned instanceId, DataUser &userBy, Json::Value &data, Json::Value &result);
	int GetMinInstance(unsigned uid, Json::Value &data);
	int ReplaceInstance(const DataInstance &instance);
	int ExportInstanceData(unsigned uid, unsigned instanceId, const string &dir);

	int AddInstance(const DataInstance &data);
	int SetInstance(unsigned uid, unsigned id, const DataInstance &data);
	int GetInstance(unsigned uid, unsigned id, DataInstance &data);
	int RemoveInstance(unsigned uid, unsigned id);
	int RemoveInstance(unsigned uid);
	int GetInstanceLimit(unsigned uid, unsigned id, DataInstance &data);
	int GetInstanceLimit(unsigned uid, vector<DataInstance> &datas);
	int SetInstanceLimit(unsigned uid, unsigned id, const DataInstance &data);

private:
	static int GetInstanceArchive(unsigned id, Json::Value &data);
};

#endif
