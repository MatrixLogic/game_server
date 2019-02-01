#ifndef __LOGIC_USER_DATA_H__
#define __LOGIC_USER_DATA_H__

#include "LogicInc.h"

class CLogicUserData
{
public:
	int InitializeUserData(unsigned uid, DataUserData *pData = NULL);
	int GetUserData(unsigned uid, DataUserData &data);
	int SetUserData(unsigned uid, const DataUserData &data);

	int SetTributeList(unsigned uid, const Json::Value &data);
	int SetNpcattack(unsigned uid, const Json::Value &data);
	int SetGift(unsigned uid, const Json::Value &data);
	int SetLadder(unsigned uid, const Json::Value &data);
	int SetDailyQuest(unsigned uid, const Json::Value &data);
	int SetWorldposCollect(unsigned uid,const Json::Value &data);
	int GetWorldposCollect(unsigned uid, Json::Value &data);
	int AddWorldposCollect(unsigned uid,const unsigned world_pos);
	int DelWorldposCollect(unsigned uid,const unsigned world_pos);

};

#endif
