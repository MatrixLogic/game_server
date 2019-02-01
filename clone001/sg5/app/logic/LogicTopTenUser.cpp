/*
 * LogicTopTenUser.cpp
 *
 *  Created on: 2013-3-6
 *      Author: Administrator
 */

#include "LogicTopTenUser.h"

CLogicTopTenUser::CLogicTopTenUser() {}

CLogicTopTenUser::~CLogicTopTenUser() {}

CDataTopTenUser* CLogicTopTenUser::GetCDataTopTenUser()
{
	GET_MEM_DATA_SEM(CDataTopTenUser, CONFIG_TOP_TEN_USER_PATH, sem_toptenuser)
	/*static CDataTopTenUser* pdata = NULL;
	if (NULL == pdata)
	{
		pdata = new CDataTopTenUser();
		if (!pdata->Init(Config::GetValue(CONFIG_TOP_TEN_USER_PATH)))
		{
			delete pdata;
			pdata = NULL;
		}
	}
	return pdata;*/
}


int CLogicTopTenUser::GetInfo(DataTopTenUser &data)
{
	CDataTopTenUser* pData = GetCDataTopTenUser();
	if (NULL == pData)
	{
		error_log("GetCDataTopTenUser return NULL");
		return R_ERR_DATA;
	}
	int ret = pData->GetInfo(data);
	if (0 != ret)
	{
		error_log("pData->GetInfo fail, ret=%d", ret);
		return ret;
	}
	return 0;
}

int CLogicTopTenUser::SetUserLevel(unsigned uid, int level)
{
	if(!CheckTime(USER_LEVEL_RANK))
	{
		return R_ERR_DATA;
	}
	CDataTopTenUser* pData = GetCDataTopTenUser();
	if (NULL == pData)
	{
		error_log("GetCDataTopTenUser return NULL");
		return R_ERR_DATA;
	}
	DataUserLevel data;
	data.uid = uid;
	data.level = level;
	return pData->SetUserLevel(data);
}

int CLogicTopTenUser::SetHeroPower(unsigned uid, int power, string& stat)
{
	if(!CheckTime(HERO_POWER_RANK))
	{
		return R_ERR_DATA;
	}

	Json::Value OldUserStat;
	Json::Reader tempReader;
	tempReader.parse(stat, OldUserStat);
	int dehp = 0;
	Json::GetInt(OldUserStat, "dehp", dehp);
	if(power == dehp)
		return 0;

	CDataTopTenUser* pData = GetCDataTopTenUser();
	if (NULL == pData)
	{
		error_log("GetCDataTopTenUser return NULL");
		return R_ERR_DATA;
	}
	DataHeroPower data;
	data.uid = uid;
	data.power = power;
	return pData->SetHeroPower(data);
}

int CLogicTopTenUser::SetUserPlunder(unsigned uid)
{
	if(!CheckTime(USER_LOOT_RANK))
	{
		return R_ERR_DATA;
	}
	CLogicAttack logicAttack;
	vector<DataAttack> attacks;
	int ret = logicAttack.GetAllAttackHistory(uid, attacks);
	if (0 != ret)
	{
		return ret;
	}
	unsigned plunder = 0;
	vector<DataAttack>::iterator itr = attacks.begin();
	for (; itr!=attacks.end(); itr++)
	{
		if (itr->attack_uid == uid)
		{
			plunder += itr->r1_loot+itr->r2_loot+itr->r3_loot+itr->r4_loot+itr->r5_loot;
		}
	}
	CDataTopTenUser* pData = GetCDataTopTenUser();
	if (NULL == pData)
	{
		error_log("GetCDataTopTenUser return NULL");
		return R_ERR_DATA;
	}
	DataUserPlunder data;
	data.uid = uid;
	data.plunder = plunder;
	return pData->SetUserPlunder(data);
}

bool CLogicTopTenUser::CheckTime(int type)
{
	unsigned openTs = 0;
	if (!Config::GetUIntValue(openTs, CONFIG_OPEN_TS))
	{
		error_log("get CONFIG_OPEN_TS fail");
		return false;
	}
	unsigned activityTs = 0;
	const unsigned seven_data = 7 * 24 * 60 * 60;
	const unsigned five_data = 5 * 24 * 60 * 60;
	const unsigned one_data = 1 * 24 * 60 * 60;
	switch(type)
	{
	case USER_LEVEL_RANK:
		activityTs = openTs + seven_data;
		break;
	case HERO_POWER_RANK:
		activityTs = openTs + five_data;
		break;
	case USER_LOOT_RANK:
		activityTs = openTs + seven_data;
		break;
	default:
		return false;
	}

	if ( activityTs < Time::GetGlobalTime())
	{
		if (activityTs + one_data > Time::GetGlobalTime())
		{
			Reward(type);
		}
		return false;
	}
	return true;
}

int CLogicTopTenUser::Reward(int type)
{
	int ret = 0;
	DataTopTenUser data;
	CDataTopTenUser* pData = GetCDataTopTenUser();
	if (NULL == pData)
	{
		error_log("GetCDataTopTenUser return NULL");
		return R_ERR_DATA;
	}
	int flag = 0;
	ret = pData->GetRewardFlage(flag);
	if (0 != ret)
	{
		return R_ERR_DATA;
	}

	int control = 1 << type;
	int result = flag & control;
	if (result != 0)
	{
		return 0;
	}

	flag |= control;
	ret = pData->SetRewardFlage(flag);
	if (0 != ret)
	{
		error_log("[SetRewardFlage fail][type=%d,ret=%d]",type,ret);
		return ret;
	}

	ret = pData->GetInfo(data);
	if (0 != ret)
	{
		error_log("[get top_ten list fail] ret=%d", ret);
		return ret;
	}

	CLogicUpdates logicUpdates;
	Json::Value updates;
	updates.resize(1);
	updates[(unsigned)0]["ts"] = Time::GetGlobalTime();

	switch(type)
	{
	case USER_LEVEL_RANK:
		for (int i=0; i<10; i++)
		{
			updates[(unsigned)0]["rank"] = i + 1;
			if (IsValidUid(data.userLevel[i].uid))
			{
				updates[(unsigned)0]["s"] = "activityplayertop10";
				ret = logicUpdates.AddUpdates(data.userLevel[i].uid, updates);
				if (0 != ret)
				{
					error_log("[add updates faile][uid=%u,ret=%d]", data.userLevel[i].uid, ret);
				}
			}
		}
		break;
	case HERO_POWER_RANK:
		if (IsValidUid(data.heroPower[0].uid))
		{
			updates[(unsigned)0]["s"] = "activityherotop";
			ret = logicUpdates.AddUpdates(data.heroPower[0].uid, updates);
			if (0 != ret)
			{
				error_log("[add updates faile][uid=%u,ret=%d]", data.heroPower[0].uid, ret);
			}
		}
		break;
	case USER_LOOT_RANK:
		for (int i=0; i<10; i++)
		{
			updates[(unsigned)0]["rank"] = i + 1;
			if (IsValidUid(data.userPlunder[i].uid))
			{
				updates[(unsigned)0]["s"] = "activityroottop10";
				ret = logicUpdates.AddUpdates(data.userPlunder[i].uid, updates);
				if (0 != ret)
				{
					error_log("[add updates faile][uid=%u,ret=%d]", data.userPlunder[i].uid, ret);
				}
			}
		}
		break;
	default:
		error_log("[reward type error][type=%d]", type);
		return R_ERR_PARAM;
	}
	return 0;
}
