/*
 * DataUserRank.cpp
 *
 *  Created on: 2013-1-22
 *      Author: colin
 */
#include "DataUserRank.h"

int CDataUserRank::AddUserRank(const DataUserRank &dataUserRank)
{
	DBCREQ_DECLARE(DBC::InsertRequest, dataUserRank.uid);
	DBCREQ_SET_KEY(dataUserRank.uid);

	DBCREQ_SET_INT(dataUserRank, power);
	DBCREQ_SET_INT(dataUserRank, basePoints);
	DBCREQ_SET_INT(dataUserRank, passNums);
	DBCREQ_SET_INT(dataUserRank, gainRes);

	DBCREQ_EXEC;
	return 0;
}

int CDataUserRank::SetUserRank(unsigned uid, const DataUserRank & dataUserRank)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_INT(dataUserRank, power);
	DBCREQ_SET_INT(dataUserRank, basePoints);
	DBCREQ_SET_INT(dataUserRank, passNums);
	DBCREQ_SET_INT(dataUserRank, gainRes);

	DBCREQ_EXEC;
	return 0;
}

int CDataUserRank::GetUserRank(unsigned uid, DataUserRank &dataUserRank)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(power);
	DBCREQ_NEED(basePoints);
	DBCREQ_NEED(passNums);
	DBCREQ_NEED(gainRes);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(dataUserRank, uid);
	DBCREQ_GET_INT(dataUserRank, power);
	DBCREQ_GET_INT(dataUserRank, basePoints);
	DBCREQ_GET_INT(dataUserRank, passNums);
	DBCREQ_GET_INT(dataUserRank, gainRes);

	return 0;
}

int CDataUserRank::RemoveUser(unsigned uid)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_EXEC;
	return 0;
}

//int CDataUserRank::UpdatePowerMcity(unsigned uid, const int power, const int mcity)
//{
//	DataUserRank data;
//	int ret = GetUserRank(uid, data);
//	if(ret == 0)
//	{
//		data.power = power;
//		data.basePoints = mcity;
//		ret = SetUserRank(uid, data);
//		if(ret != 0)
//		{
//			error_log("[SetUserRank fail][ret=%d,uid=%u]", ret, uid);
//			DB_ERROR_RETURN_MSG("db_set_user_rank_fail");
//		}
//		return 0;
//	}
//	else if (ret == R_ERR_NO_DATA)
//	{
//		data.uid = uid;
//		data.power = power;
//		data.basePoints = mcity;
//		data.gainRes = 0;
//		data.passNums = 0;
//		ret = AddUserRank(data);
//		if(ret != 0)
//		{
//			error_log("[add userRank fail from UpdateLootsPowerMcity][ret=%d]", ret);
//			DB_ERROR_RETURN_MSG("db_add_user_rank_fail_updateLPM");
//		}
//		return 0;
//	}
//	else
//	{
//		error_log("[GetUserRank fail][ret=%d,uid=%u]", ret, uid);
//		DB_ERROR_RETURN_MSG("db_get_user_rank_fail");
//	}
//}
//
//int CDataUserRank::UpdateLootsPowerMcity(unsigned uid, int r1, int r2, int r3, int r4, int r5, int power, int mcity)
//{
//	int all = 0;
//	all = r1 + r2 + r3 + r4 + r5;
//	DataUserRank data;
//	int ret = GetUserRank(uid, data);
//	if(ret == 0)
//	{
//		data.gainRes += all;
//		data.power = power;
//		data.basePoints = mcity;
//		ret = SetUserRank(uid, data);
//		if(ret != 0)
//		{
//			error_log("[SetUserRank fail][ret=%d,uid=%u]", ret, uid);
//			DB_ERROR_RETURN_MSG("db_set_user_rank_fail");
//		}
//		return 0;
//		//error_log("[GetUserRank fail][ret=%d,uid=%u]", ret, uid);
//		//DB_ERROR_RETURN_MSG("db_get_user_rank_fail");
//	}
//	else if (ret == R_ERR_NO_DATA)
//	{
//		data.uid = uid;
//		data.gainRes = all;
//		data.power = power;
//		data.basePoints = mcity;
//		ret = AddUserRank(data);
//		if(ret != 0)
//		{
//			error_log("[add userRank fail from UpdateLootsPowerMcity][ret=%d]", ret);
//			DB_ERROR_RETURN_MSG("db_add_user_rank_fail_updateLPM");
//		}
//		return 0;
//	}
//	else
//	{
//		error_log("[GetUserRank fail][ret=%d,uid=%u]", ret, uid);
//		DB_ERROR_RETURN_MSG("db_get_user_rank_fail");
//	}
//
//
//}
//
//int CDataUserRank::UpdatePassGateNums(unsigned uid, unsigned num)
//{
//	DataUserRank data;
//	int ret = GetUserRank(uid, data);
//	if(ret == 0)
//	{
//		data.passNums += num;
//		ret = SetUserRank(uid, data);
//		if(ret != 0)
//		{
//			error_log("[SetUserRank fail][ret=%d,uid=%u]", ret, uid);
//			DB_ERROR_RETURN_MSG("db_set_user_rank_fail");
//		}
//		return 0;
//		//error_log("[GetUserRank fail][ret=%d,uid=%u]", ret, uid);
//		//DB_ERROR_RETURN_MSG("db_get_user_rank_fail");
//	}
//	else if (ret == R_ERR_NO_DATA)
//	{
//		data.uid = uid;
//		data.passNums = num;
//		data.basePoints = 0;
//		data.gainRes = 0;
//		data.power = 0;
//		ret = AddUserRank(data);
//		if(ret != 0)
//		{
//			error_log("[add userRank fail from UpdateGate][ret=%d]", ret);
//			DB_ERROR_RETURN_MSG("db_add_user_rank_fail_updateGate");
//		}
//		return 0;
//	}
//	else
//	{
//		error_log("[GetUserRank fail][ret=%d,uid=%u]", ret, uid);
//		DB_ERROR_RETURN_MSG("db_get_user_rank_fail");
//	}
//
//}
//
//int CDataUserRank::UpdateLoots(unsigned uid, const int rAll_loots)
//{
//	DataUserRank data;
//	int ret = GetUserRank(uid, data);
//	if(ret == 0)
//	{
//		data.gainRes += rAll_loots;
//		ret = SetUserRank(uid, data);
//		if(ret != 0)
//		{
//			error_log("[SetUserRank fail] from UpdateLoots [ret=%d,uid=%u]", ret, uid);
//			DB_ERROR_RETURN_MSG("db_set_user_rank_fail");
//		}
//		return 0;
//		//error_log("[GetUserRank fail][ret=%d,uid=%u]", ret, uid);
//		//DB_ERROR_RETURN_MSG("db_get_user_rank_fail");
//	}
//	else if (ret == R_ERR_NO_DATA)
//	{
//		data.uid = uid;
//		data.passNums = 0;
//		data.basePoints = 0;
//		data.gainRes = rAll_loots;
//		data.power = 0;
//		ret = AddUserRank(data);
//		if(ret != 0)
//		{
//			error_log("[add userRank fail from UpdateLoots][ret=%d]", ret);
//			DB_ERROR_RETURN_MSG("db_add_user_rank_fail_updateLoots");
//		}
//		return 0;
//	}
//	else
//	{
//		error_log("[GetUserRank fail][ret=%d,uid=%u]", ret, uid);
//		DB_ERROR_RETURN_MSG("db_get_user_rank_fail");
//	}
//}
//
//int CDataUserRank::GetUserPowerMcity(unsigned uid, unsigned& power, unsigned& mcity)
//{
//	DataUser user;
//	CDataUser dataUser;
//	int ret = dataUser.GetUser(uid, user);
//	if (ret != 0)
//	{
//		return R_ERR_DB;
//	}
//	Json::Value value;
//	Json::Reader().parse(user.user_stat, value);
//	power = value["power"].asUInt();
//	mcity = value["mcity"].asUInt();
//	return 0;
//}
//
//int CDataUserRank::SetUserPowerMcity(unsigned uid, const unsigned power, const unsigned mcity)
//{
//	DataUserRank data;
//	int ret = GetUserRank(uid, data);
//	if(ret != 0)
//	{
//		error_log("[GetUserRank fail][ret=%d,uid=%u]", ret, uid);
//		DB_ERROR_RETURN_MSG("db_get_user_rank_fail");
//	}
//	data.power = power;
//	data.basePoints = mcity;
//	ret = SetUserRank(uid, data);
//	if(ret != 0)
//	{
//		error_log("[SetUserRank fail][ret=%d,uid=%u]", ret, uid);
//		DB_ERROR_RETURN_MSG("db_set_user_rank_fail");
//	}
//	return 0;
//}
///*
//int CDataUserRank::GetUserGate(unsigned uid, unsigned& gate)
//{
//	gate = 0;
//	vector<DataGate> vecDataGate;
//	CDataGate dataGate;
//	int ret = dataGate.GetGate(uid, vecDataGate);
//	if (ret != 0)
//	{
//		return R_ERR_DB;
//	}
//	Json::Value value;
//	for (vector<DataGate>::iterator itr = vecDataGate.begin(); itr != vecDataGate.end(); ++itr)
//	{
//		Json::Reader().parse(itr->data, value);
//		gate += value["w"].asUInt();
//	}
//	return 0;
//}
//
//int CDataUserRank::GetUserLoots(unsigned uid, unsigned& loots)
//{
//
//}
//*/
