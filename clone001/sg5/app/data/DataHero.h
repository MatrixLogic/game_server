#ifndef __DATA_HERO_H__
#define __DATA_HERO_H__

#include "Kernel.h"

struct DataHero
{
	unsigned uid;
	unsigned id;
	string   data;
};

class CDataHero : public CDataBaseDBC
{
public:
	CDataHero(int table = DB_HERO) : CDataBaseDBC(table) {}

	int AddHero(unsigned uid, unsigned id, const string &data);
	int SetHero(unsigned uid, unsigned id, const string &data);
	int GetHero(unsigned uid, unsigned id, string &data);
	int RemoveHero(unsigned uid, unsigned id);
	int RemoveHero(unsigned uid);

	int GetHero(unsigned uid, vector<DataHero> &datas);
	int ReplaceHero(unsigned uid, unsigned id, const string &data);
	int GetHero(unsigned uid, map<unsigned, string> &datas);
};

#endif
