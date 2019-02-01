/*
 * LogicPointRank.cpp
 *
 *  Created on: 2012-10-17
 *      Author: Administrator
 */

#include "LogicPointsRank.h"

CLogicPointsRank::CLogicPointsRank() {
	// TODO Auto-generated constructor stub

}

CLogicPointsRank::~CLogicPointsRank() {
	// TODO Auto-generated destructor stub
}

CDataPointsRank* CLogicPointsRank::GetPointsRankData()
{
	GET_MEM_DATA_SEM(CDataPointsRank, POINTS_RANK, sem_pointsrank)
	/*static CDataPointsRank *pdata = NULL;
	if(pdata == NULL)
	{
		pdata = new CDataPointsRank();
	}
	if(pdata->Init(Config::GetValue(POINTS_RANK)))
	{
		delete pdata;
		pdata = NULL;
	}
	return pdata;*/
}
int CLogicPointsRank::GetPointsRank(int unsigned uid, DataPoints vecPoints[])
{
	CDataPointsRank *pdata= GetPointsRankData();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}
	int ret = pdata->GetRankList(vecPoints);
	if(ret != 0)
	{
		return ret;
	}
	return 0;
}
int CLogicPointsRank::GetPointsRankJson(int unsigned uid, Json::Value &result)
{
	int ret = 0;
	// vector<DataPoints> vecPoints;
	DataPoints vecPoints[RANK_SIZE];
	ret = GetPointsRank(uid,vecPoints);
	if(ret != 0)
	{
			return ret;
	}
	int iCount = RANK_SIZE;
	//int iCount = vecPoints.size();
	for(int i = 0; i < iCount; i++)
	{
		result[i]["uid"] = vecPoints[i].uid;
		result[i]["points"] = vecPoints[i].points;
		result[i]["name"] = string(vecPoints[i].name);
	}
	RewardPointRank();
	return 0;
}

int CLogicPointsRank::UpdatePointsRank(int unsigned uid, const DataPoints &points)
{
	// 活动到期，不更新排行榜
	info_log("points record uid=%u,points = %d",points.uid, points.points);
	if(!(  Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_POINTS_BEGIN_TS)
		&& Time::GetGlobalTime() + 4*60*60 <= Config::GetIntValue(CONFIG_POINTS_END_TS)))
	{
		return R_ERR_REFUSE;
	}
	CDataPointsRank *pdata= GetPointsRankData();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}

	int ret = pdata->UpdateRankList(points);
	if(ret != 0)
	{
		return ret;
	}
	return 0;
}

int CLogicPointsRank::RewardPointRank(void)
{
	//活动未结束，不需发奖励
	if(    Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_POINTS_BEGIN_TS)
		&& Time::GetGlobalTime()+4*60*60 <= Config::GetIntValue(CONFIG_POINTS_END_TS))
	{
		return R_ERR_REFUSE;
	}

	int ret = 0;
	DataPoints pointList[RANK_SIZE];
	CDataPointsRank *pdata= GetPointsRankData();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}

	ret = pdata->GetRankList(pointList);
	if(ret != 0)
	{
		return ret;
	}

	Json::Value updateJson = Json::Value(Json::arrayValue);
	updateJson.resize(1);
	CLogicUpdates logiceUpdates;
	for (int i=0; i<5; i++)
	{
		if (!IsValidUid(pointList[i].uid))
		{
			debug_log("[invalid uid][uid=%u]", pointList[i].uid);
			continue;
		}
		if (pointList[i].reward == 1)
		{
			debug_log("[allready had reward][uid=%u]", pointList[i].uid);
			continue;
		}
		ret = pdata->SetReward(pointList[i].uid, 1);
		if (0 != ret)
		{
			debug_log("[set reward flag fail][ret=%d,uid=%u]", ret, pointList[i].uid);
			error_log("[set reward flag fail][ret=%d,uid=%u]", ret, pointList[i].uid);
			continue;
		}

		updateJson[(unsigned)0]["uid"] = pointList[i].uid;
		updateJson[(unsigned)0]["s"] = "activitypointrank";
		updateJson[(unsigned)0]["rank"] = i+1;
		updateJson[(unsigned)0]["ts"] = Time::GetGlobalTime();
		updateJson[(unsigned)0]["points"] = pointList[i].points;
		ret = logiceUpdates.AddUpdates(pointList[i].uid, updateJson);
		if (0 != ret)
		{
			debug_log("[Add updates fail][ret=%d,uid=%u]", ret, pointList[i].uid);
			error_log("[Add updates fail][ret=%d,uid=%u]", ret, pointList[i].uid);
		}
		Json::FastWriter writer;
		debug_log("pointrnak rewar,updates=%s", writer.write(updateJson).c_str());
	}
	return 0;
}

int CLogicPointsRank::CleanRankList(void)
{
	CDataPointsRank *pdata= GetPointsRankData();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}
	return pdata->CleanRankList();
}
