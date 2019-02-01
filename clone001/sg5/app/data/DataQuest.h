#ifndef __DATA_QUEST_H__
#define __DATA_QUEST_H__

#include "Kernel.h"

struct DataQuest
{
	unsigned uid;
	unsigned id;
	string   data;
};

class CDataQuest : public CDataBaseDBC
{
public:
	CDataQuest(int table = DB_QUEST) : CDataBaseDBC(table) {}

	int AddQuest(unsigned uid, unsigned id, const string &data);
	int SetQuest(unsigned uid, unsigned id, const string &data);
	int GetQuest(unsigned uid, unsigned id, string &data);
	int RemoveQuest(unsigned uid, unsigned id);
	int RemoveQuest(unsigned uid);

	int GetQuest(unsigned uid, vector<DataQuest> &datas);
	int ReplaceQuest(unsigned uid, unsigned id, const string &data);
};

#endif
