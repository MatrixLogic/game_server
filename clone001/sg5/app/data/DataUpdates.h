#ifndef __DATA_UPDATES_H__
#define __DATA_UPDATES_H__

#include "Kernel.h"

struct DataUpdates
{
	unsigned uid;
	unsigned ts;
	string   data;
};

class CDataUpdates : public CDataBaseDBC
{
public:
	CDataUpdates(int table = DB_UPDATES) : CDataBaseDBC(table) {}

	int AddUpdates(unsigned uid, unsigned ts, const string &data);
	int GetUpdates(unsigned uid, vector<DataUpdates> &datas);
	int GetUpdatesBeforeTs(unsigned uid, unsigned ts ,vector<DataUpdates> &datas);
	int RemoveUpdatesBeforeTs(unsigned uid, unsigned ts);

};

#endif
