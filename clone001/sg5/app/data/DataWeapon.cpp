/*
 * DataWeapon.cpp
 *
 *  Created on: 2013-3-13
 *      Author: Administrator
 */

#include "DataWeapon.h"

int CDataWeapon::GetInfo(unsigned uid, string &data)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_GET_STR_V(data);
	return 0;
}

int CDataWeapon::SetInfo(unsigned uid, const string &data)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_STR_V(data);
	return 0;
}

int CDataWeapon::AddInfo(unsigned uid, const string &data)
{
	DBCREQ_DECLARE(DBC::InsertRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_STR_S(data);

	DBCREQ_EXEC;
	return 0;
}
/**********************************
int CDataWeapon::GetShip(DataShip &data)
{
	DBCREQ_DECLARE(DBC::GetRequest, data.uid);
	DBCREQ_SET_KEY(data.uid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(type);
	DBCREQ_NEED(start_time);
	DBCREQ_NEED(posy);
	DBCREQ_NEED(attack_num);
	DBCREQ_NEED(end_time);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(data, uid);
	DBCREQ_GET_INT(data, type);
	DBCREQ_GET_INT(data, start_time);
	DBCREQ_GET_INT(data, posy);
	DBCREQ_GET_INT(data, attack_num);
	DBCREQ_GET_INT(data, end_time);
	return 0;
}

int CDataWeapon::SetShip(const DataShip &data)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, data.uid);
	DBCREQ_SET_KEY(data.uid);
	DBCREQ_SET_INT(data, type);
	DBCREQ_SET_INT(data, start_time);
	DBCREQ_SET_INT(data, posy);
	DBCREQ_SET_INT(data, attack_num);
	DBCREQ_SET_INT(data, end_time);
	DBCREQ_EXEC;
	return 0;
}

int CDataWeapon::AddShip(const DataShip &data)
{
	DBCREQ_DECLARE(DBC::InsertRequest, data.uid);
	DBCREQ_SET_KEY(data.uid);
	DBCREQ_SET_INT(data, type);
	DBCREQ_SET_INT(data, start_time);
	DBCREQ_SET_INT(data, posy);
	DBCREQ_SET_INT(data, attack_num);
	DBCREQ_SET_INT(data, end_time);
	DBCREQ_EXEC;
	return 0;
}
***********************************/
int CDataWeapon::SetAttackLog(unsigned uid, const string &attack_log)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_STR_V(attack_log);
	return 0;
}
int CDataWeapon::AddAttackLog(unsigned uid, const string &attack_log)
{
	DBCREQ_DECLARE(DBC::InsertRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_STR_V(attack_log);
	return 0;
}
int CDataWeapon::GetAttackLog(unsigned uid, string &attack_log)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_GET_STR_V(attack_log);
	return 0;
}
