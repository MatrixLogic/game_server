
#ifndef LOGICARCHIVE_H_
#define LOGICARCHIVE_H_

#include "LogicInc.h"

class CLogicArchive
{
public:
	int Load(unsigned uid, unsigned uidBy, const string &type, const Json::Value &data, Json::Value &result);
	int Save(unsigned uid, unsigned uidBy, const string &type, Json::Value &data, Json::Value &result);
	//int UpdateSave(unsigned uid, unsigned uidBy, Json::Value &result);
	int UpdateSave(unsigned uid, unsigned uidBy, unsigned world_pos,Json::Value &result);
	int Updates(unsigned uid, unsigned uidBy, Json::Value &data, Json::Value &result);

	int ProcessOrders(unsigned uid, const Json::Value data, DataPay &payData, bool addable);
	//int UpdateAttack(unsigned uid, unsigned uidDefence, Json::Value &data);
	int UpdateAttack(const DataUser &user, const DataUser &userDefence, Json::Value &data);
	int ProcessAttackInfo(unsigned uid, Json::Value &attackinfo);
	int ProcessUpdateInfo(unsigned uid, Json::Value &updateinfo);
	int SaveRefresh(unsigned uid, const string ipStr, Json::Value &refreshinfo);
	int is_Login( string &openid, string &openkey);
	int GetActivityTime(Json::Value &data);
	static int GetInitArchive(const Json::Value *&pArchive);

	int checkUserStats(const Json::Value &old, const Json::Value &now);
	int checkUserTech(const Json::Value &old, const Json::Value &now);
};

#endif /* LOGICARCHIVE_H_ */
