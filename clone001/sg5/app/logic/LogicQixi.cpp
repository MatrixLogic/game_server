
/*
 * LogicQixi.h
 *
 *  Created on: 2013-08-06
 *      Author: Administrator
 */

#include "LogicQixi.h"


CLogicQixijie::CLogicQixijie() {
	// TODO Auto-generated constructor stub

}

CLogicQixijie::~CLogicQixijie() {
	// TODO Auto-generated destructor stub
}

CDataQixijie* CLogicQixijie::GetQixijieData()
{
	GET_MEM_DATA_SEM(CDataQixijie, QIXIJIE_PAY_RANK, sem_qixijie)

}

int CLogicQixijie::GetQixiActivityRank(unsigned uid, DataQixiRank vecPoints[])
{
	CDataQixijie *pdata= GetQixijieData();
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
int CLogicQixijie::GetQixiActivityRankJson(unsigned uid, Json::Value &result)
{
	CDataQixijie *pdata= GetQixijieData();

	if( Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_PAY_OF_QIXIJIE_BEGIN_TS)
	&& Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAY_OF_QIXIJIE_END_TS) + 48*60*60)
	{

		DataQixiRank vecPoints[RANK_SIZE];
		DataGetWinners vecPrize[4];

		int ret = GetQixiActivityRank(uid,vecPoints);
		if(ret != 0)
		{
				return ret;
		}
		int Lotterynumber;
		ret = pdata->GetRecordLotterynumber(Lotterynumber);
		if(ret != 0)
		{
				return ret;
		}
		int markreward;
		ret = pdata->GetMarkreward(markreward);
		if(ret != 0)
		{
				return ret;
		}
		ret = pdata->GetPrizeNum(vecPrize);
		if(ret != 0)
		{
				return ret;
		}
		int prizenum[4];
		ret = pdata->GetPrizeNum11(prizenum);
		if(ret != 0)
		{
				return ret;
		}

		int iCount = RANK_SIZE;

		//前十名积分
		for(int i = 0; i < iCount; i++)
		{

			result[i]["uid"] = vecPoints[i].uid;
			result[i]["pay"] = vecPoints[i].pay;
			result[i]["name"] = string(vecPoints[i].name);
		}
		result[iCount]["number"] = Lotterynumber;
		for(int i = iCount+1; i < iCount +5 ; i++)              //大奖
		{
//			result[i]["mark"] = "Prizes issued";
			result[i]["uid"] = vecPrize[i- iCount-1].uid;
			result[i]["rewards"] = vecPrize[i-iCount-1].reward;
			result[i]["name"] = string(vecPrize[i-iCount-1].name);
		}
		result[iCount +5]["PrizesResults"] = markreward;

		result[iCount +6]["Prizesnum_1"] = prizenum[0];
		result[iCount +7]["Prizesnum_2"] = prizenum[1];
		result[iCount +8]["Prizesnum_3"] = prizenum[2];
		result[iCount +9]["Prizesnum_4"] = prizenum[3];

		//个人积分
		DataUser user;
		CLogicUser logicUser;
		ret = logicUser.GetUser(uid,user);
		if(ret != 0)
			return ret;
		int payTotal = 0;
		string user_stat = user.user_stat;
		Json::Reader reader;
		Json::Value userStat;
		if (!reader.parse(user_stat, userStat))
		{
			error_log("[paser user_flag fail][uid=%u]", uid);
			DB_ERROR_RETURN_MSG("user_flag_error");
		}
		if(!userStat.isMember("choujiangNum"))
		{
			payTotal = 0;

		}
		else
		{
			payTotal = (userStat["choujiangNum"].asUInt()+1) * 10;
		}

		result[iCount +10]["Self"] = payTotal;

		if( Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_PAY_OF_QIXIJIE_END_TS)
		&& Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAY_OF_QIXIJIE_END_TS) + 48*60*60)
		{
			int flag = 0;
			ret = pdata->Setrewardflag();
			if(ret != 0)
			{
					return ret;
			}
			int ret = pdata->Getrewardflag(flag);
			if(1 == flag)
			{
				RewardQixiActivityRank();
				ret = pdata->SetRecordLotterynumber();    //最后一天消费统计数据
				if(ret != 0)
				{
						return ret;
				}
//				CleanRankList();
				error_log("rewadrs----------------");
			}
		}
	}
	return 0;
}

int CLogicQixijie::UpdateQixiActivityRank(int unsigned uid, Json::Value &input)
{
	if( Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_PAY_OF_QIXIJIE_BEGIN_TS)
	&& Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAY_OF_QIXIJIE_END_TS))
	{
		unsigned now = Time::GetGlobalTime();
		CDataQixijie *pdata= GetQixijieData();
		if(pdata == NULL)
		{
			DB_ERROR_RETURN_MSG("init_qixiactivityrank_fail");
		}
		int ret = 0;
		DataUser user;
		CLogicUser logicUser;
		ret = logicUser.GetUser(uid,user);
		if(ret != 0)
			return ret;
		unsigned lastpay_ts = 0;
		int payTotal = 0;
		string user_stat = user.user_stat;
		Json::Reader reader;
		Json::Value userStat;
		if (!reader.parse(user_stat, userStat))
		{
			error_log("[paser user_flag fail][uid=%u]", uid);
			DB_ERROR_RETURN_MSG("user_flag_error");
		}
		if(!userStat.isMember("choujiangNum"))
		{
			return R_ERR_NO_DATA;

		}
		else
		{
			payTotal = (userStat["choujiangNum"].asUInt()+1) * 10;
		}


		DataQixiRank points;
		CLogicUserBasic logicUserBasic;
		DataUserBasic dataUserBasic;
		logicUserBasic.GetUserBasicLimit(uid,OpenPlatform::GetType(),dataUserBasic);
		points.uid = uid;
		points.pay = payTotal;
		memcpy(points.name,dataUserBasic.name.c_str(),sizeof(points.name) - 1);

		ret = pdata->SetRecordLotterynumber();
		if(ret != 0)
		{
			return ret;
		}

///////////////////////////////////随机数产生
		int s1 = 0;
		int s2 = 0;
		for(int i = 0; i < 10; i++)
		{
			s1 = s1 +  input["qixifirstlayer"]["rate"][i].asInt();
		}
		for(int i = 0; i < 24; i++)
		{
			s2 = s2 +  input["qixisecondlayer"]["rate"][i].asInt();
		}
		int Platform = OpenPlatform::GetType();
		ret = pdata->LotteryResults(uid,input,s1,s2,Platform);
		error_log("ssssssssssss: %d | %d",s1,s2);
		if(ret != 0)
		{
			return ret;
		}


		ret = pdata->UpdateRankList(points);
		if(ret != 0)
		{
			return ret;
		}
	}
	else
	{
		return R_ERR_REFUSE;
	}
//给当前服所有在线人员发updates
//	if(num > 200)
//	{
//		int flag;
//		int serverId = 0;
//		Config::GetDomain(serverId);
//		if (0 == serverId)
//			return NULL;
//		uint64_t uidEnd;
//		unsigned userid = UID_MIN + 500000 * (serverId - 1);
//		CLogicIdCtrl logicIdCtrl;
//		ret = logicIdCtrl.GetCurrentId(KEY_UID_CTRL, uidEnd, serverId);
//		if (ret != 0) {
//			error_log(
//					"get current user id fail! from AllChangeCash ---start!");
//			return ret;
//		}
//		for(int i = userid; i <= uidEnd; i++)
//		{
//			DataUser user;
//			CLogicUser logicUser;
//			int ret = logicUser.GetUser(i, user);
//			if(0 != ret)
//			{
//				continue;
//			}
//			flag = (Time::GetGlobalTime() - ONLINE_TIMEOUT < user.last_active_time) ? 1 : 0;
//			if(1 == flag)
//			{
//				Json::Value updateJson;
//				updateJson.resize(1);
//				CLogicUpdates logiceUpdates;
//				updateJson[(unsigned)0]["s"] = "OnthesecondLayer";
//				updateJson[(unsigned)0]["uid"] = i;
//				updateJson[(unsigned)0]["ts"] = Time::GetGlobalTime();
//				ret = logiceUpdates.AddUpdates(i, updateJson);
//				if (0 != ret)
//				{
//					error_log("[Add updates fail][ret=%d,uid=%u]", ret, i);
//				}
//				Json::FastWriter writer;
//				debug_log("hundreddays reward,updates=%s", writer.write(updateJson).c_str());
//			}
//		}
//	}
	return 0;
}

int CLogicQixijie::RewardQixiActivityRank(void)
{
	int ret = 0;
	DataQixiRank pointList[RANK_SIZE];
	CDataQixijie *pdata= GetQixijieData();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_qixijie_avtivityrank_fail");
	}

	ret = pdata->GetRankList(pointList);
	if(ret != 0)
	{
		return ret;
	}
	Json::Value updateJson;
	updateJson.resize(1);
	CLogicUpdates logiceUpdates;
	for (int i=0; i<5; i++)
	{
		if(pointList[i].pay >= 2000)
		{
			if (!IsValidUid(pointList[i].uid))
			{
				continue;
			}

			updateJson[(unsigned)0]["uid"] = pointList[i].uid;
			updateJson[(unsigned)0]["s"] = "QixiJieRank";
			updateJson[(unsigned)0]["rank"] = i+1;
			updateJson[(unsigned)0]["ts"] = Time::GetGlobalTime();
			updateJson[(unsigned)0]["pay"] = pointList[i].pay;

			ret = logiceUpdates.AddUpdates(pointList[i].uid, updateJson);
			if (0 != ret)
			{
				error_log("[Add updates fail][ret=%d,uid=%u]", ret, pointList[i].uid);
			}
			Json::FastWriter writer;
			//debug_log("qixijie_reward,updates=%s", writer.write(updateJson).c_str());
		}
	}
	return 0;
}

int CLogicQixijie::CleanRankList(void)
{
	CDataQixijie *pdata= GetQixijieData();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("clean_qixiactivity_fail");
	}
	return pdata->CleanRankList();
}
