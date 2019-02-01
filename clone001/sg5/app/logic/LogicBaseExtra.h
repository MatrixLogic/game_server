/*
 * LogicBaseExtra.h
 *
 *  Created on: 2012-10-25
 *      Author: Administrator
 */

#ifndef LOGICBASEEXTRA_H_
#define LOGICBASEEXTRA_H_
#include "LogicInc.h"
class CLogicBaseExtra {
public:
	CLogicBaseExtra();
	virtual ~CLogicBaseExtra();

	int AddBaseExtra(unsigned uid, const DataBaseExtra &data);
	int GetBaseExtra(unsigned uid, unsigned worldpos,  DataBaseExtra &data);
	int GetBaseExtra(unsigned uid, vector<DataBaseExtra> &data);
	int GetBaseExtraOnType(unsigned uid,int type, vector<DataBaseExtra> &data);
	int SetBaseExtra(unsigned uid, unsigned worldpos,const DataBaseExtra &data);
	int SetStationeHeros(unsigned uid, unsigned worldpos,const string &heros);
	int GetStationeHeros(unsigned uid, unsigned worldpos,string &heros);
	int GetStationedSoldiers(unsigned uid, unsigned worldpos,string &soldiers);
	int SetStationedSoldiers(unsigned uid, unsigned worldpos,const string &soldiers);
	int GetWorldPosList(unsigned uid,Json::Value &worldposList);
	int GetSubBaseInfoJson(unsigned uid,Json::Value &resource);
	int SetSubBaseResource(unsigned uid, unsigned worldpos,string const &resource);
	int GetProtectedTime(unsigned uid,unsigned worldpos,unsigned &protectedTime);
	int UpdateProtectedTime(unsigned uid,unsigned worldpos,unsigned const protectedTime);
	int UpdateLastBreathTime(unsigned uid,unsigned worldpos,unsigned const last_save_time);
	int UpdateBreathInfo(unsigned uid,unsigned worldpos, const DataBreathInfo &breathInfo);
	int GetLastBreathTime(unsigned uid,unsigned worldpos,unsigned &last_breath_time);
	int RemoveSubBase(unsigned uid, unsigned worldpos);

};

#endif /* LOGICBASEEXTRA_H_ */
