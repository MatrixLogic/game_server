#ifndef __DATA_SECINC_H__
#define __DATA_SECINC_H__

#include "Kernel.h"

struct DataSecinc
{
	unsigned uid;
	unsigned sid;
	string   data;
};

class CDataSecinc : public CDataBaseDBC
{
public:
	CDataSecinc(int table = DB_SECINC) : CDataBaseDBC(table) {}

	int AddSecinc(unsigned uid, unsigned sid, const string &data);
	int SetSecinc(unsigned uid, unsigned sid, const string &data);
	int GetSecinc(unsigned uid, unsigned sid, string &data);
	int RemoveSecinc(unsigned uid, unsigned sid);
	int RemoveSecinc(unsigned uid);

	int GetSecinc(unsigned uid, vector<DataSecinc> &datas);
};

#endif
