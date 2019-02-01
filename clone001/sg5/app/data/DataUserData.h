#ifndef DATAUSER_DATA_H_
#define DATAUSER_DATA_H_

#include "Kernel.h"

struct DataUserData
{
	unsigned uid;
	string tribute_list;
	string npcattack;
	string gift;
	string ladder;
	string dailyquest;
	string worldpos_collect;
};

class CDataUserData : public CDataBaseDBC
{
public:
	CDataUserData(int table = DB_USER_DATA) : CDataBaseDBC(table) {}

	int AddUserData(DataUserData &data);
	int SetUserData(unsigned uid, const DataUserData &data);
	int GetUserData(unsigned uid, DataUserData &data);
	int RemoveUserData(unsigned uid);

	int SetTributeList(unsigned uid, const string &tribute_list);
	int SetNpcattack(unsigned uid, const string &npcattack);
	int SetGift(unsigned uid, const string &gift);
	int SetLadder(unsigned uid, const string &ladder);
	int SetDailyQuest(unsigned uid, const string &dailyquest);
	int GetWorldposCollect(unsigned uid,string &worldpos_collect);
	int SetWorldposCollect(unsigned uid,const string &worldpos_collect);
	int SetAllServerLadder(unsigned uid, const string &allserver_ladder);
	int GetAllServerLadder(unsigned uid, string &allserver_ladder);
};

#endif
