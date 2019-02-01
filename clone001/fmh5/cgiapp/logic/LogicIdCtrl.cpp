/*
 * LogicIdCtrl.cpp
 *
 *  Created on: 2011-7-14
 *      Author: dada
 */

#include "LogicIdCtrl.h"

int CLogicIdCtrl::GetNextId(int key, uint64_t &nextId, unsigned db)
{
	int ret;
	CDataIdCtrl dbIdCtrl;
	ret = dbIdCtrl.GetId(key, nextId, db);
	if(ret != 0)
	{
		error_log("[GetId fail][key=%d,db=%u,ret=%u]", key,db,ret);
		DB_ERROR_RETURN_MSG("db_get_id_ctrl_fail");
	}
	nextId++;
	ret = dbIdCtrl.SetId(key, nextId, db);
	if(ret != 0)
	{
		error_log("[SetId fail][key=%d,nextId=%llu,db=%u,ret=%u]", key, nextId,db,ret);
		DB_ERROR_RETURN_MSG("db_set_id_ctrl_fail");
	}
	return 0;
}

int CLogicIdCtrl::GetCurrentId(int key, uint64_t &currId, unsigned db)
{
	int ret;
	CDataIdCtrl dbIdCtrl;
	ret = dbIdCtrl.GetId(key, currId, db);
	if(ret != 0)
	{
		error_log("[GetId fail][key=%d,db=%u]", key,db);
		DB_ERROR_RETURN_MSG("get_id_fail");
	}
	return 0;
}
