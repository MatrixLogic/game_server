/*
 * LogicPayRank.cpp
 *
 *  Created on: 2012-11-7
 *      Author: Administrator
 */

#include "LogicPayRank.h"

CLogicPayRank::CLogicPayRank() {
	// TODO Auto-generated constructor stub

}

CLogicPayRank::~CLogicPayRank() {
	// TODO Auto-generated destructor stub
}

CDataPayRank* CLogicPayRank::GetPayRankData()
{
	GET_MEM_DATA_SEM(CDataPayRank, CONFIG_PAYRANK, sem_payrank)
	/*static CDataPayRank *pdata = NULL;
	if(pdata == NULL)
	{
		pdata = new CDataPayRank();
	}
	if(pdata->Init(Config::GetValue(CONFIG_PAYRANK)))
	{
		delete pdata;
		pdata = NULL;
	}
	return pdata;*/
}
int CLogicPayRank::GetPayRank(int unsigned uid, PayItem vecPayRank[])
{
	CDataPayRank *pdata= GetPayRankData();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}
	int ret = pdata->GetRank(vecPayRank);
	if(ret != 0)
	{
		return ret;
	}
	return 0;
}
int CLogicPayRank::GetPayRankJson(int unsigned uid, Json::Value &result)
{
	int ret = 0;
	PayItem vecPayRank[RANK_SIZE];
	ret = GetPayRank(uid,vecPayRank);
	if(ret != 0)
	{
		return ret;
	}
	int iCount = PAY_RANK_SIZE;
	for(int i = 0; i < iCount; i++)
	{
		result[i]["uid"] = vecPayRank[i].uid;
		result[i]["payTotal"] = vecPayRank[i].payTotal;
		result[i]["name"] = string(vecPayRank[i].name);
	}
	PayRankReward();
	return 0;
}

int CLogicPayRank::UpdatePayRank(int unsigned uid, const PayItem &pay)
{
	if(    Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_PAYRANK_BEGIN_TS)
		&& Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAYRANK_END_TS))
	{
		CDataPayRank *pdata= GetPayRankData();
		if(pdata == NULL)
		{
			DB_ERROR_RETURN_MSG("init_pointsrank_fail");
		}
		int ret = pdata->UpdateRank(pay);
		if(ret != 0)
		{
			return ret;
		}
		return 0;
	}
	return R_ERR_REFUSE;
}

int CLogicPayRank::PayRankReward(void)
{
	//活动未结束，不需发奖励
	if(    Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_PAYRANK_BEGIN_TS)
		&& Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAYRANK_END_TS))
	{
		return R_ERR_REFUSE;
	}

	int ret = 0;
	PayItem payRankList[PAY_RANK_SIZE];
	CDataPayRank *pdata= GetPayRankData();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}

	ret = pdata->GetRank(payRankList);
	if(ret != 0)
	{
		return ret;
	}

	Json::Value updateJson = Json::Value(Json::arrayValue);
	updateJson.resize(1);
	CLogicUpdates logiceUpdates;
	for (int i=0; i<5; i++)
	{
		if (!IsValidUid(payRankList[i].uid))
		{
			continue;
		}
		if (payRankList[i].reward_flag == 1)
		{
			continue;
		}
		ret = pdata->SetReward(payRankList[i].uid, 1);
		if (0 != ret)
		{
			error_log("[set reward flag fail][ret=%d,uid=%u]", ret, payRankList[i].uid);
			continue;
		}
		updateJson[(unsigned)0]["uid"] = payRankList[i].uid;
		updateJson[(unsigned)0]["s"] = "activitypayrank";
		updateJson[(unsigned)0]["rank"] = i+1;
		updateJson[(unsigned)0]["ts"] = Time::GetGlobalTime();
		updateJson[(unsigned)0]["total"] = payRankList[i].payTotal;
		ret = logiceUpdates.AddUpdates(payRankList[i].uid, updateJson);
		if (0 != ret)
		{
			error_log("[Add updates fail][ret=%d,uid=%u]", ret, payRankList[i].uid);
		}
		Json::FastWriter writer;
		//debug_log("payrnak rewar,updates=%s", writer.write(updateJson).c_str());
	}
	return 0;
}
