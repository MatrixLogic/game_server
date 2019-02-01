
/*
 * LogicHundredDaysActivity.h
 *
 *  Created on: 2013-07-17
 *      Author: Administrator
 */

#include "LogicHundredDaysActivity.h"


CLogicHundredDaysActivityRank::CLogicHundredDaysActivityRank() {
	// TODO Auto-generated constructor stub

}

CLogicHundredDaysActivityRank::~CLogicHundredDaysActivityRank() {
	// TODO Auto-generated destructor stub
}

CDataHDaysAcvitityRank* CLogicHundredDaysActivityRank::GetHundredDaysActivityRankData()
{
	GET_MEM_DATA_SEM(CDataHDaysAcvitityRank, HUNDREDDAYSACTIVITY_RANK, sem_hundreddaysactivity)
}

int CLogicHundredDaysActivityRank::GetHundredDaysActivityRank(int unsigned uid, DataHDaysAcvitityPay vecPoints[])
{
	CDataHDaysAcvitityRank *pdata= GetHundredDaysActivityRankData();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_hundreddaysactivityrank_fail");
	}
	int ret = pdata->GetRankList(vecPoints);
	if(ret != 0)
	{
		return ret;
	}
	return 0;
}
int CLogicHundredDaysActivityRank::GetHundredDaysActivityRankJson(unsigned uid, Json::Value &result)
{

	CDataHDaysAcvitityRank *pdata= GetHundredDaysActivityRankData();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_hundreddaysactivityrank_fail");
	}

	DataHDaysAcvitityPay vecPoints[RANK_SIZE];
	int ret = GetHundredDaysActivityRank(uid,vecPoints);
	if(ret != 0)
	{
		return ret;
	}
	int iCount = RANK_SIZE;
	//int iCount = vecPoints.size();
	//前十名积分
	for(int i = 0; i < iCount; i++)
	{

		result[i]["uid"] = vecPoints[i].uid;
		result[i]["pay"] = vecPoints[i].pay;
		result[i]["name"] = string(vecPoints[i].name);
	}

	//自己积分
	Json::Value userFlag;
	CLogicUser logicUser;
	userFlag.clear();
	ret = logicUser.GetUserFlag(uid,userFlag);
	if(ret)
		return ret;

	result[10]["uid"] = uid;
	if(!userFlag.isMember("hundredDaysPay"))
	{
		userFlag["hundredDaysPay"][0u]= 0;
		userFlag["hundredDaysPay"][1u]= Time::GetGlobalTime();
		ret = logicUser.SetUserFlag(uid, userFlag);
		if (ret != 0)
		{
			error_log("[set user flag fail][uid=%u,cash=%u]",uid,0);
			return ret;
		}
	}
	if(CTime::GetDayInterval(Time::GetGlobalTime(),userFlag["hundredDaysPay"][1u].asUInt()) == 0 )
	{
		result[10]["pay"] = userFlag["hundredDaysPay"][0u].asUInt();
	}
	else
	{
		result[10]["pay"] = 0;
	}

    //发奖
	vector <UpdatesHundDays> data;
	Json::Value updateJson;
	updateJson.resize(1);
	ret = pdata->RewardHundredDaysActivityRank(data);
	if(ret != 0)
	{
		return ret;
	}

	if(data.size() > 0)
	{
		for(int i = 0; i < 5; i++)
		{
			CLogicUpdates logiceUpdates;
			updateJson[(unsigned)0]["uid"] = data[i].uid;
			updateJson[(unsigned)0]["s"] = "IntegralRank";
			updateJson[(unsigned)0]["rank"] = i+1;
			updateJson[(unsigned)0]["ts"] = Time::GetGlobalTime();
			updateJson[(unsigned)0]["pay"] = data[i].pay;

			if(data[i].uid != 0)
			{
				ret = logiceUpdates.AddUpdates(data[i].uid, updateJson);
				if (0 != ret)
				{
					error_log("[Add updates fail][ret=%d,uid=%u]", ret, data[i].uid);
				}
				Json::FastWriter writer;
				debug_log("hundreddays reward,updates=%s", writer.write(updateJson).c_str());

				string texT = "感谢您参加乐不思蜀活动，您共获得";
				unsigned int rrr = i +1;
				texT.append(CTrans::ITOS(data[i].pay)).append("积分，排名第");
				texT.append(CTrans::ITOS(rrr)).append(",本次乐不思蜀活动奖励已发放，请查收！");

				DataEmail email;
				CLogicEmail logicEmail;
				vector<uint64_t> toUidList;
				toUidList.push_back(data[i].uid);
				email.attach_flag = 0;
				email.attachments = "";
				email.uid = ADMIN_UID;
				email.post_ts = Time::GetGlobalTime();
				email.title = "乐不思蜀";
				email.text = texT;
				logicEmail.AddEmail(email,toUidList,OpenPlatform::GetType());
				//debug_log("hundreddays_email=%s", texT.c_str());
			}
		}
	}


	return 0;
}

int CLogicHundredDaysActivityRank::UpdateHundredDaysActivityRank(int unsigned uid, const DataHDaysAcvitityPay &points)
{

	CDataHDaysAcvitityRank *pdata= GetHundredDaysActivityRankData();

	//更新排行榜
	if( (Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS)&& Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS) + 22*60*60)
		|| (Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS) + 24*60*60 && Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS) + 46*60*60)
		|| (Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS) + 48*60*60 && Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_END_TS)) )
	{

		if(pdata == NULL)
		{
			DB_ERROR_RETURN_MSG("init_hundreddaysactivityrank_fail");
		}
		int ret = pdata->UpdateRankList(points);
		if(ret != 0)
		{
			return ret;
		}
	}

	return 0;
}

//int CLogicHundredDaysActivityRank::RewardHundredDaysActivityRank(void)
//{
//	//特定时间发奖励
////	if( (Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS) + 22*60*60 && Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS) + 24*60*60)
////		|| (Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS) + 46*60*60 && Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS) + 48*60*60)
////		|| (Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_END_TS) && Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_END_TS) + 2*60*60 ))
////	{
//	int ret = 0;
//	DataHDaysAcvitityPay pointList[RANK_SIZE];
//	CDataHDaysAcvitityRank *pdata= GetHundredDaysActivityRankData();
//	if(pdata == NULL)
//	{
//		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
//	}
//
//	ret = pdata->GetRankList(pointList);
//	if(ret != 0)
//	{
//		return ret;
//	}
//	Json::Value updateJson;
//	updateJson.resize(1);
//	CLogicUpdates logiceUpdates;
//	for (int i=0; i<5; i++)
//	{
//		if (!IsValidUid(pointList[i].uid))
//		{
//			continue;
//		}
////		if (pointList[i].reward == 1)
////		{
////			continue;
////		}
////		ret = pdata->SetReward(pointList[i].uid, 1);
////		if (0 != ret)
////		{
////			error_log("[set reward flag fail][ret=%d,uid=%u]", ret, pointList[i].uid);
////			continue;
////		}
//
//		updateJson[(unsigned)0]["uid"] = pointList[i].uid;
//		updateJson[(unsigned)0]["s"] = "IntegralRank";
//		updateJson[(unsigned)0]["rank"] = i+1;
//		updateJson[(unsigned)0]["ts"] = Time::GetGlobalTime();
//		updateJson[(unsigned)0]["pay"] = pointList[i].pay;
//
//		ret = logiceUpdates.AddUpdates(pointList[i].uid, updateJson);
//		if (0 != ret)
//		{
//			error_log("[Add updates fail][ret=%d,uid=%u]", ret, pointList[i].uid);
//		}
//		Json::FastWriter writer;
//		debug_log("hundreddays reward,updates=%s", writer.write(updateJson).c_str());
//
//		string texT = "感谢您参加百日庆典活动，您共获得";
//		unsigned int rrr = i +1;
//		texT.append(CTrans::ITOS(pointList[i].pay)).append("积分，排名第");
//		texT.append(CTrans::ITOS(rrr)).append(",本次百日庆典活动奖励已发放，请查收！");
//
//		DataEmail email;
//		CLogicEmail logicEmail;
//		vector<uint64_t> toUidList;
//		toUidList.push_back(pointList[i].uid);
//		email.attach_flag = 0;
//		email.attachments = "";
//		email.uid = ADMIN_UID;
//		email.post_ts = Time::GetGlobalTime();
//		email.title = "百日庆典";
//		email.text = texT;
//		logicEmail.AddEmail(email,toUidList,OpenPlatform::GetType());
//	}
//
//
//
//
//	return 0;
//}

int CLogicHundredDaysActivityRank::CleanRankList(void)
{
	CDataHDaysAcvitityRank *pdata= GetHundredDaysActivityRankData();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}
	return pdata->CleanRankList();
}
