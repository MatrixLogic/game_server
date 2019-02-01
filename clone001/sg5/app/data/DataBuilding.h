#ifndef __DATA_BUILDING_H__
#define __DATA_BUILDING_H__

#include "Kernel.h"

struct DataBuilding
{
	unsigned uid;
	unsigned id;
	unsigned worldpos;
	string   data;
};

class CDataBuilding : public CDataBaseDBC
{
public:
	CDataBuilding(int table = DB_BUILDING) : CDataBaseDBC(table) {}

	int AddBuilding(unsigned uid, unsigned id,unsigned worldpos, const string &data);
	int AddBuilding(unsigned uid, unsigned id, const string &data);
	int SetBuilding(unsigned uid, unsigned id, const string &data);
	int SetBuilding(unsigned uid, unsigned id, unsigned worldpos,const string &data);

	int GetBuilding(unsigned uid, unsigned id, string &data);
	int GetBuilding(unsigned uid, unsigned id,unsigned worldpos, string &data);

	int RemoveBuilding(unsigned uid, unsigned id);
	int RemoveBuilding(unsigned uid, unsigned worldpos,unsigned id);
	int RemoveBuildingOnWordPos(unsigned uid, unsigned worldpos);
	int RemoveBuilding(unsigned uid);
	int GetBuilding(unsigned uid, vector<DataBuilding> &datas);
	int ReplaceBuilding(unsigned uid, unsigned id, const string &data);
	int ReplaceBuilding(unsigned uid, unsigned id, unsigned worldpos, const string &data);
	int GetBuilding(unsigned uid, map<unsigned, string> &datas);
	int GetBuilding(unsigned uid, unsigned worldpos,map<unsigned, string> &datas);
	int GetBuilding(unsigned uid, unsigned worldpos, vector<DataBuilding> &datas);
	//int SetBuilding(unsigned uid, unsigned id,unsigned worldpos, const string &data);
};

#endif
