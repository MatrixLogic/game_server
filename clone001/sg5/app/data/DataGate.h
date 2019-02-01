#ifndef __DATA_GATE_H__
#define __DATA_GATE_H__

#include "Kernel.h"

struct DataGate
{
	unsigned uid;
	unsigned id;
	string   data;
};

class CDataGate : public CDataBaseDBC
{
public:
	CDataGate(int table = DB_GATE) : CDataBaseDBC(table) {}

	int AddGate(unsigned uid, unsigned id, const string &data);
	int SetGate(unsigned uid, unsigned id, const string &data);
	int GetGate(unsigned uid, unsigned id, string &data);
	int RemoveGate(unsigned uid, unsigned id);
	int RemoveGate(unsigned uid);

	int GetGate(unsigned uid, vector<DataGate> &datas);
	int GetGate(unsigned uid, unsigned start, unsigned end, vector<DataGate> &datas);
	int ReplaceGate(unsigned uid, unsigned id, const string &data);
	int GetGateIds(unsigned uid, vector<unsigned> &ids);
};

#endif
