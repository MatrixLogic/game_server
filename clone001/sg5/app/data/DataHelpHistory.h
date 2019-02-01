#ifndef __DATA_HELPHISTORY_H__
#define __DATA_HELPHISTORY_H__

#include "Kernel.h"

struct DataHelpHistory
{
	unsigned uid;
	unsigned ts;
	string   data;
};

class CDataHelpHist : public CDataBaseDBC
{
public:
	CDataHelpHist(int table = DB_HELP_HISTORY) : CDataBaseDBC(table) {}

	int AddHelpHist(unsigned uid, unsigned ts, const string &data);
	int GetHelpHist(unsigned uid, vector<DataHelpHistory> &datas);
	int RemoveHelpHistBeforeTs(unsigned uid, unsigned ts);
};

#endif
