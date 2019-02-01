/*
 * DataWeapon.h
 *
 *  Created on: 2013-3-13
 *      Author: Administrator
 */

#ifndef DATAWEAPON_H_
#define DATAWEAPON_H_

#include "Kernel.h"
/**********************************
struct DataShip
{
	unsigned uid;
	int type;
	unsigned start_time;
	unsigned end_time;
	int posy;
	int attack_num;
};
***********************************/
class CDataWeapon : public CDataBaseDBC
{
public:
	CDataWeapon(int table = DB_WEAPON) : CDataBaseDBC(table) {}
	int GetInfo(unsigned uid, string &data);
	int SetInfo(unsigned uid, const string &data);
	int AddInfo(unsigned uid, const string &data);

	/**********************************
	int GetShip(DataShip &data);
	int SetShip(const DataShip &data);
	int AddShip(const DataShip &data);
	***********************************/
	int SetAttackLog(unsigned uid, const string &attack_log);
	int AddAttackLog(unsigned uid, const string &attack_log);
	int GetAttackLog(unsigned uid, string &attack_log);
};

#endif /* DATAWEAPON_H_ */
