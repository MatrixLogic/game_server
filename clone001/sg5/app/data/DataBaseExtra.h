/*
 * DataBaseExtra.h
 *
 *  Created on: 2012-10-25
 *      Author: Administrator
 */

#ifndef DATABASEEXTRA_H_
#define DATABASEEXTRA_H_
#include "Kernel.h"
struct DataBaseExtra{
	unsigned uid;
	unsigned worldpos;
	unsigned occupy_time;
	unsigned protected_time;
	unsigned last_collect_ts;
	unsigned last_breath_time;
	unsigned last_breath_uid;
	int being_attack_flag;  // 0:
	int type;
	string resource;
};
struct DataBreathInfo{
	unsigned last_breath_uid;
	unsigned last_breath_time;
	int being_attack_flag;
};
enum WorldAttackFlag{
	NO_ATTACK = 0,
	BEING_ATTACK = 1
};
class CDataBaseExtra :public CDataBaseDBC{
public:
	CDataBaseExtra(int table = DB_BASE_EXTRA):CDataBaseDBC(table){}
	virtual ~CDataBaseExtra();
	int AddBaseExtra(unsigned uid, const DataBaseExtra &data);
	int GetBaseExtra(unsigned uid, unsigned worldpos,  DataBaseExtra &data);
	int GetBaseExtra(unsigned uid,   vector<DataBaseExtra> &data);
	int GetBaseExtraOnType(unsigned uid,  int type,vector<DataBaseExtra> &data);
	int SetBaseExtra(unsigned uid, unsigned worldpos,const DataBaseExtra &data);
	//int SetStationeHeros(unsigned uid, unsigned worldpos,const string &heros);
	//int GetStationeHeros(unsigned uid, unsigned worldpos,string &heros);
	//int GetStationedSoldiers(unsigned uid, unsigned worldpos,string &soldiers);
	//int SetStationedSoldiers(unsigned uid, unsigned worldpos,const string &soldiers);
	int GetWorldposList(unsigned uid, vector<DataBaseExtra> &worldposList);
	int GetSubBaseInfo(unsigned uid, vector<DataBaseExtra> &vSubBases);
	int SetSubBaseResource(unsigned uid, unsigned worldpos,string const &worldres);
	int UpdateProtectedTime(unsigned uid, unsigned worldpos, const unsigned protect_time);
	int UpdateBreathInfo(unsigned uid, unsigned worldpos,const DataBreathInfo& attackInfo);
	int GetProtectedTime(unsigned uid, unsigned worldpos,  unsigned &protected_time);
	int GetLastBreathTime(unsigned uid, unsigned worldpos, unsigned &protected_time);
	int RemoveSubBase(unsigned uid, unsigned worldpos);
};

#endif /* DATABASEEXTRA_H_ */
