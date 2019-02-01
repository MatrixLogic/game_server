#ifndef __DATA_EQUIPMENT_H__
#define __DATA_EQUIPMENT_H__

#include "Kernel.h"

struct DataEquipment
{
	unsigned uid;
	unsigned id;
	string   data;
};

class CDataEquipment : public CDataBaseDBC
{
public:
	CDataEquipment(int table = DB_EQUIPMENT) : CDataBaseDBC(table) {}

	int AddEquipment(unsigned uid, unsigned id, const string &data);
	int SetEquipment(unsigned uid, unsigned id, const string &data);
	int GetEquipment(unsigned uid, unsigned id, string &data);
	int RemoveEquipment(unsigned uid, unsigned id);
	int RemoveEquipment(unsigned uid);

	int GetEquipment(unsigned uid, vector<DataEquipment> &datas);
	int ReplaceEquipment(unsigned uid, unsigned id, const string &data);
};

#endif
