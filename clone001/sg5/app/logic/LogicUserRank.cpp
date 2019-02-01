/*
 * LogicUserRank.cpp
 *
 *  Created on: 2013-2-4
 *      Author: colin
 */
#include "LogicUserRank.h"

int CLogicUserRank::UpdatePowerMcity(unsigned uid, const int power, const int mcity)
{
	DataUserRank data;
	CDataUserRank dbUserRank;
	int ret = dbUserRank.GetUserRank(uid, data);
	if(ret == 0)
	{
		data.power = power;
		data.basePoints = mcity;
		ret = dbUserRank.SetUserRank(uid, data);
		if(ret != 0)
		{
			error_log("[SetUserRank fail][ret=%d,uid=%u]", ret, uid);
			DB_ERROR_RETURN_MSG("db_set_user_rank_fail");
		}
		return 0;
	}
	else if (ret == R_ERR_NO_DATA)
	{
		data.uid = uid;
		data.power = power;
		data.basePoints = mcity;
		data.gainRes = 0;
		data.passNums = 0;
		ret = dbUserRank.AddUserRank(data);
		if(ret != 0)
		{
			error_log("[add userRank fail from UpdateLootsPowerMcity][ret=%d]", ret);
			DB_ERROR_RETURN_MSG("db_add_user_rank_fail_updateLPM");
		}
		return 0;
	}
	else
	{
		error_log("[GetUserRank fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("db_get_user_rank_fail");
	}
}

int CLogicUserRank::UpdateLootsPowerMcity(unsigned uid, int r1, int r2, int r3, int r4, int r5, int power, int mcity)
{
	int all = 0;
	all = r1 + r2 + r3 + r4 + r5;
	DataUserRank data;
	CDataUserRank dbUserRank;
	int ret = dbUserRank.GetUserRank(uid, data);
	if(ret == 0)
	{
		data.gainRes += all;
		data.power = power;
		data.basePoints = mcity;
		ret = dbUserRank.SetUserRank(uid, data);
		if(ret != 0)
		{
			error_log("[SetUserRank fail][ret=%d,uid=%u]", ret, uid);
			DB_ERROR_RETURN_MSG("db_set_user_rank_fail");
		}
		return 0;
		//error_log("[GetUserRank fail][ret=%d,uid=%u]", ret, uid);
		//DB_ERROR_RETURN_MSG("db_get_user_rank_fail");
	}
	else if (ret == R_ERR_NO_DATA)
	{
		data.uid = uid;
		data.gainRes = all;
		data.power = power;
		data.basePoints = mcity;
		ret = dbUserRank.AddUserRank(data);
		if(ret != 0)
		{
			error_log("[add userRank fail from UpdateLootsPowerMcity][ret=%d]", ret);
			DB_ERROR_RETURN_MSG("db_add_user_rank_fail_updateLPM");
		}
		return 0;
	}
	else
	{
		error_log("[GetUserRank fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("db_get_user_rank_fail");
	}


}

int CLogicUserRank::UpdatePassGateNums(unsigned uid, unsigned num)
{
	DataUserRank data;
	CDataUserRank dbUserRank;
	int ret = dbUserRank.GetUserRank(uid, data);
	if(ret == 0)
	{
		data.passNums += num;
		ret = dbUserRank.SetUserRank(uid, data);
		if(ret != 0)
		{
			error_log("[SetUserRank fail][ret=%d,uid=%u]", ret, uid);
			DB_ERROR_RETURN_MSG("db_set_user_rank_fail");
		}
		return 0;
		//error_log("[GetUserRank fail][ret=%d,uid=%u]", ret, uid);
		//DB_ERROR_RETURN_MSG("db_get_user_rank_fail");
	}
	else if (ret == R_ERR_NO_DATA)
	{
		data.uid = uid;
		data.passNums = num;
		data.basePoints = 0;
		data.gainRes = 0;
		data.power = 0;
		ret = dbUserRank.AddUserRank(data);
		if(ret != 0)
		{
			error_log("[add userRank fail from UpdateGate][ret=%d]", ret);
			DB_ERROR_RETURN_MSG("db_add_user_rank_fail_updateGate");
		}
		return 0;
	}
	else
	{
		error_log("[GetUserRank fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("db_get_user_rank_fail");
	}

}

int CLogicUserRank::UpdateLoots(unsigned uid, const int rAll_loots)
{
	DataUserRank data;
	CDataUserRank dbUserRank;
	int ret = dbUserRank.GetUserRank(uid, data);
	if(ret == 0)
	{
		data.gainRes += rAll_loots;
		ret = dbUserRank.SetUserRank(uid, data);
		if(ret != 0)
		{
			error_log("[SetUserRank fail] from UpdateLoots [ret=%d,uid=%u]", ret, uid);
			DB_ERROR_RETURN_MSG("db_set_user_rank_fail");
		}
		return 0;
		//error_log("[GetUserRank fail][ret=%d,uid=%u]", ret, uid);
		//DB_ERROR_RETURN_MSG("db_get_user_rank_fail");
	}
	else if (ret == R_ERR_NO_DATA)
	{
		data.uid = uid;
		data.passNums = 0;
		data.basePoints = 0;
		data.gainRes = rAll_loots;
		data.power = 0;
		ret = dbUserRank.AddUserRank(data);
		if(ret != 0)
		{
			error_log("[add userRank fail from UpdateLoots][ret=%d]", ret);
			DB_ERROR_RETURN_MSG("db_add_user_rank_fail_updateLoots");
		}
		return 0;
	}
	else
	{
		error_log("[GetUserRank fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("db_get_user_rank_fail");
	}
}

int CLogicUserRank::GetUserPowerMcity(unsigned uid, unsigned& power, unsigned& mcity)
{
	DataUser user;
	CDataUser dataUser;
	int ret = dataUser.GetUser(uid, user);
	if (ret != 0)
	{
		return R_ERR_DB;
	}
	Json::Value value;
	Json::Reader().parse(user.user_stat, value);
	power = value["power"].asUInt();
	mcity = value["mcity"].asUInt();
	return 0;
}

int CLogicUserRank::SetUserPowerMcity(unsigned uid, const unsigned power, const unsigned mcity)
{
	DataUserRank data;
	CDataUserRank dbUserRank;
	int ret = dbUserRank.GetUserRank(uid, data);
	if(ret != 0)
	{
		error_log("[GetUserRank fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("db_get_user_rank_fail");
	}
	data.power = power;
	data.basePoints = mcity;
	ret = dbUserRank.SetUserRank(uid, data);
	if(ret != 0)
	{
		error_log("[SetUserRank fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("db_set_user_rank_fail");
	}
	return 0;
}
/*
int CLogicUserRank::GetUserGate(unsigned uid, unsigned& gate)
{
	gate = 0;
	vector<DataGate> vecDataGate;
	CDataGate dataGate;
	int ret = dataGate.GetGate(uid, vecDataGate);
	if (ret != 0)
	{
		return R_ERR_DB;
	}
	Json::Value value;
	for (vector<DataGate>::iterator itr = vecDataGate.begin(); itr != vecDataGate.end(); ++itr)
	{
		Json::Reader().parse(itr->data, value);
		gate += value["w"].asUInt();
	}
	return 0;
}

int CLogicUserRank::GetUserLoots(unsigned uid, unsigned& loots)
{

}
*/


