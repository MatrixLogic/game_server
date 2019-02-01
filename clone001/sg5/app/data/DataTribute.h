#ifndef __DATA_TRIBUTE_H__
#define __DATA_TRIBUTE_H__

#include "Kernel.h"

struct DataTribute
{
	unsigned uid;
	unsigned id;
	string   data;
};

class CDataTribute : public CDataBaseDBC
{
public:
	CDataTribute(int table = DB_TRIBUTE) : CDataBaseDBC(table) {}

	int AddTribute(unsigned uid, unsigned id, const string &data);
	int SetTribute(unsigned uid, unsigned id, const string &data);
	int GetTribute(unsigned uid, unsigned id, string &data);
	int RemoveTribute(unsigned uid, unsigned id);

	int GetTribute(unsigned uid, vector<DataTribute> &datas);
	int ReplaceTribute(unsigned uid, unsigned id, const string &data);

	int GetTributeIds(unsigned uid, vector<unsigned> &ids);
};

#endif
