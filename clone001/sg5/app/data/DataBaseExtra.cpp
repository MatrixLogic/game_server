/*
 * DataBaseExtra.cpp
 *
 *  Created on: 2012-10-25
 *      Author: Administrator
 */

#include "DataBaseExtra.h"



CDataBaseExtra::~CDataBaseExtra() {
	// TODO Auto-generated destructor stub
}

int CDataBaseExtra::AddBaseExtra(unsigned uid, const DataBaseExtra &base_extra)
{
	DBCREQ_DECLARE(DBC::InsertRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_INT(base_extra, worldpos);
	DBCREQ_SET_INT(base_extra, occupy_time);
	DBCREQ_SET_INT(base_extra, protected_time);
	DBCREQ_SET_INT(base_extra, last_collect_ts);
	DBCREQ_SET_INT(base_extra, last_breath_time);
	DBCREQ_SET_INT(base_extra, last_breath_uid);
	DBCREQ_SET_INT(base_extra, being_attack_flag);
	DBCREQ_SET_INT(base_extra, type);
	DBCREQ_SET_STR(base_extra, resource);

	DBCREQ_EXEC;
	return 0;
}
int  CDataBaseExtra::GetBaseExtra(unsigned uid, unsigned worldpos, DataBaseExtra &base_extra)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ,worldpos,worldpos);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(worldpos);
	DBCREQ_NEED(occupy_time);
	DBCREQ_NEED(protected_time);
	DBCREQ_NEED(last_collect_ts);
	DBCREQ_NEED(last_breath_time);
	DBCREQ_NEED(last_breath_uid);
	DBCREQ_NEED(being_attack_flag);;
	DBCREQ_NEED(type);
	DBCREQ_NEED(resource);


	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(base_extra, uid);
	DBCREQ_GET_INT(base_extra, worldpos);
	DBCREQ_GET_INT(base_extra, occupy_time);
	DBCREQ_GET_INT(base_extra, protected_time);
	DBCREQ_GET_INT(base_extra, last_collect_ts);
	DBCREQ_GET_INT(base_extra, last_breath_time);
	DBCREQ_GET_INT(base_extra, last_breath_uid);
	DBCREQ_GET_INT(base_extra, being_attack_flag);
	DBCREQ_GET_INT(base_extra, type);
	DBCREQ_GET_STR(base_extra, resource);

	return 0;

}
int CDataBaseExtra::GetBaseExtraOnType(unsigned uid,  int type,vector<DataBaseExtra> &data)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ,type,type);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(worldpos);
	DBCREQ_NEED(occupy_time);
	DBCREQ_NEED(protected_time);
	DBCREQ_NEED(last_collect_ts);
	DBCREQ_NEED(last_breath_time);
	DBCREQ_NEED(type);
	DBCREQ_NEED(resource);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_ARRAY_GET_BEGIN(data);
	DBCREQ_ARRAY_GET_INT(data, uid);
	DBCREQ_ARRAY_GET_INT(data, worldpos);
	DBCREQ_ARRAY_GET_INT(data, occupy_time);
	DBCREQ_ARRAY_GET_INT(data, protected_time);
	DBCREQ_ARRAY_GET_INT(data, last_collect_ts);
	DBCREQ_ARRAY_GET_INT(data, last_breath_time);
	DBCREQ_ARRAY_GET_INT(data, type);
	DBCREQ_ARRAY_GET_STR(data, resource);

	DBCREQ_ARRAY_GET_END();
	return 0;
}

int CDataBaseExtra::GetBaseExtra(unsigned uid,   vector<DataBaseExtra> &data)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(worldpos);
	DBCREQ_NEED(occupy_time);
	DBCREQ_NEED(protected_time);
	DBCREQ_NEED(last_collect_ts);
	DBCREQ_NEED(last_breath_time);
	DBCREQ_NEED(last_breath_uid);
	DBCREQ_NEED(type);
	DBCREQ_NEED(resource);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	//DBCREQ_IFFETCHROW;

	DBCREQ_ARRAY_GET_BEGIN(data);
	DBCREQ_ARRAY_GET_INT(data, uid);
	DBCREQ_ARRAY_GET_INT(data, worldpos);
	DBCREQ_ARRAY_GET_INT(data, occupy_time);
	DBCREQ_ARRAY_GET_INT(data, protected_time);
	DBCREQ_ARRAY_GET_INT(data, last_collect_ts);
	DBCREQ_ARRAY_GET_INT(data, last_breath_time);
	DBCREQ_ARRAY_GET_INT(data, last_breath_uid);
	DBCREQ_ARRAY_GET_INT(data, type);
	DBCREQ_ARRAY_GET_STR(data, resource);

	DBCREQ_ARRAY_GET_END();
	return 0;
}
int  CDataBaseExtra::SetBaseExtra(unsigned uid, unsigned worldpos, const DataBaseExtra &data)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ,worldpos,worldpos);

	DBCREQ_SET_INT(data, occupy_time);
	DBCREQ_SET_INT(data, protected_time);
	DBCREQ_SET_INT(data, last_collect_ts);
	DBCREQ_SET_INT(data, last_breath_time);
	DBCREQ_SET_INT(data, last_breath_uid);
	DBCREQ_SET_INT(data, being_attack_flag);
	DBCREQ_SET_INT(data, type);
	DBCREQ_SET_STR(data, resource);

	DBCREQ_EXEC;
	return 0;
}
int CDataBaseExtra::GetWorldposList(unsigned uid, vector<DataBaseExtra> &worldposList)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(worldpos);
	DBCREQ_NEED(type);
	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;

	DBCREQ_ARRAY_GET_BEGIN(worldposList);
	DBCREQ_ARRAY_GET_INT(worldposList, worldpos);
	DBCREQ_ARRAY_GET_INT(worldposList, type);
	DBCREQ_ARRAY_GET_END();
	return 0;
}

int CDataBaseExtra::GetSubBaseInfo(unsigned uid, vector<DataBaseExtra> &vSubBases)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(worldpos);
	DBCREQ_NEED(resource);
	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;

	DBCREQ_ARRAY_GET_BEGIN(vSubBases);
	DBCREQ_ARRAY_GET_INT(vSubBases, worldpos);
	DBCREQ_ARRAY_GET_STR(vSubBases, resource);
	DBCREQ_ARRAY_GET_END();
	return 0;
}


int CDataBaseExtra::SetSubBaseResource(unsigned uid, unsigned worldpos,string const &resource)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ,worldpos,worldpos);

	DBCREQ_SET_STR_V(resource);
	return 0;
}

int CDataBaseExtra::UpdateProtectedTime(unsigned uid, unsigned worldpos, const unsigned protected_time)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ,worldpos,worldpos);

	DBCREQ_SET_INT_V(protected_time);
	return 0;
}
int CDataBaseExtra::UpdateBreathInfo(unsigned uid, unsigned worldpos,const DataBreathInfo& attackInfo)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ,worldpos,worldpos);

	DBCREQ_SET_INT(attackInfo, last_breath_time);
	DBCREQ_SET_INT(attackInfo, last_breath_uid);
	DBCREQ_SET_INT(attackInfo, being_attack_flag);
	DBCREQ_EXEC;
	return 0;
}
int CDataBaseExtra::GetProtectedTime(unsigned uid, unsigned worldpos, unsigned &last_breath_time)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ,worldpos,worldpos);
	DBCREQ_GET_INT_V(last_breath_time);
	return 0;
}
int CDataBaseExtra::GetLastBreathTime(unsigned uid, unsigned worldpos, unsigned &protected_time)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ,worldpos,worldpos);
	DBCREQ_GET_INT_V(protected_time);
	return 0;
}

int CDataBaseExtra::RemoveSubBase(unsigned uid, unsigned worldpos)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ,worldpos,worldpos);
	DBCREQ_EXEC;
	return 0;
}


