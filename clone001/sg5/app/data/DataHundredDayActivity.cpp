/*
 * DataHundredDayActivity.cpp
 *
 *  Created on: 2013-07-17
 *      Author: Administrator
 */

#include "DataHundredDayActivity.h"



CDataHDaysAcvitityRank::CDataHDaysAcvitityRank(){
	m_init = false;
}
CDataHDaysAcvitityRank::~CDataHDaysAcvitityRank() {
	// TODO Auto-generated destructor stub
}

int  CDataHDaysAcvitityRank::Init(const std::string &path, semdat sem)
{
	if(m_init)
	{
		return 0;
	}
	int semgroup = 0;
	int semserver = 0;
	Config::GetDomain(semserver);
	if(!semserver)
		Config::GetDB(semserver);
	if(!m_sh.CreateOrOpen(path.c_str(), sizeof(AcvitityPayRank), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init__joincard_data_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{
		AcvitityPayRank *pdata = (AcvitityPayRank *)m_sh.GetAddress();
		if(pdata == NULL)
		{
			DB_ERROR_RETURN_MSG("init_pointsrank_fail");
		}
		memset(pdata, 0, sizeof(*pdata));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}
int sortFunc(const DataHDaysAcvitityPay& left,const DataHDaysAcvitityPay& right){
	return left.pay > right.pay;
}
int CDataHDaysAcvitityRank::UpdateRankList(const DataHDaysAcvitityPay &points)
{
	int i= 0;
	bool inFlag = false;  // 用户是否在排行榜中
	AcvitityPayRank *pdata = (AcvitityPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_hundreddaysavtivityrank_fail");
	}
	CAutoLock lock(&m_sh, true);
	for(i = RANK_SIZE -1;i >= 0; i--)
	{
		if(pdata->HDAcvitityRank[i].uid == points.uid  )
		{
			pdata->HDAcvitityRank[i].pay = points.pay;
			// 如果用户已在排行榜，inFlag = true
			inFlag = true;
			break;
		}
	}
	if(!inFlag)
	{
		pdata->HDAcvitityRank[RANK_SIZE] = points;
	}
	else
	{
		memset(&(pdata->HDAcvitityRank[RANK_SIZE]),0,sizeof(DataHDaysAcvitityPay));
	}

	vector<DataHDaysAcvitityPay> vecPoint;
	for(i = 0; i<=RANK_SIZE;++i)
	{
		vecPoint.push_back(pdata->HDAcvitityRank[i]);
	}

	sort(vecPoint.begin(),vecPoint.end(),sortFunc);

	for(i = 0; i<=RANK_SIZE;++i){
		pdata->HDAcvitityRank[i] = vecPoint[i];
	}
	return 0;
}

int CDataHDaysAcvitityRank::GetRankList( DataHDaysAcvitityPay vecPoints[])
{
	AcvitityPayRank *pdata = (AcvitityPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_hundreddaysavtivityrank_fail");
	}
	CAutoLock lock(&m_sh, true);
	memcpy(vecPoints,pdata->HDAcvitityRank,sizeof(DataHDaysAcvitityPay) * RANK_SIZE);
	return 0;
}

int CDataHDaysAcvitityRank::RewardHundredDaysActivityRank(vector <UpdatesHundDays> &data)
{
	AcvitityPayRank *pdata = (AcvitityPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_hundreddaysavtivityrank_fail");
	}
	CAutoLock lock(&m_sh, true);
	int ret = 0;
//	error_log("lastyiji=%d | %d | %d |%d",pdata->awardflag[0],pdata->awardflag[1],pdata->awardflag[2],pdata->flag);
	//清理排行榜
	if((Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS) + 24*60*60)
		&& (Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS) + 46*60*60)
		&& pdata->flag == 1)
	{
		memset(pdata, 0, sizeof(*pdata));
		pdata->awardflag[0] = 1;
	}
	else if((Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS) + 48*60*60)
		&& (Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_END_TS))
		&& pdata->flag == 2)
	{
		memset(pdata, 0, sizeof(*pdata));
		pdata->awardflag[1] = 2;
	}
	else if((Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_END_TS) + 60*60 ) //第三天
		&& pdata->flag == 3)
	{
		memset(pdata, 0, sizeof(*pdata));
//		error_log("hundreddays_is_clean");
	}

	//第一天、
	if((Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS) + 22*60*60)
	&& (Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS) + 46*60*60)
	&& pdata->awardflag[0] == 0)
	{
		pdata->flag = 1;
		pdata->awardflag[0] = 1;

		for(int i=0; i<5; i++)
		{
			UpdatesHundDays temp;

			temp.uid = pdata->HDAcvitityRank[i].uid;
			temp.s = "IntegralRank";
			temp.rank = i+1;
			temp.ts = Time::GetGlobalTime();
			temp.pay = pdata->HDAcvitityRank[i].pay;
			data.push_back(temp);
//			error_log("test111111111:%u | %u | %d",data[i].uid,data[i].pay,data[i].rank);
		}

	}
	else if((Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS) + 46*60*60)  //第二天
			&& (Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_END_TS))
			&& pdata->awardflag[1] == 0)
	{
		pdata->flag = 2;
		pdata->awardflag[1] = 2;

		for(int i=0; i<5; i++)
		{
			UpdatesHundDays temp;

			temp.uid = pdata->HDAcvitityRank[i].uid;
			temp.s = "IntegralRank";
			temp.rank = i+1;
			temp.ts = Time::GetGlobalTime();
			temp.pay = pdata->HDAcvitityRank[i].pay;
			data.push_back(temp);
//			error_log("test222222222222:%u | %u | %d",data[i].uid,data[i].pay,data[i].rank);
		}
	}
	else if(Time::GetGlobalTime() >=Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_END_TS)  //第三天
		&& (Time::GetGlobalTime() <= Config::GetIntValue(CONFIG_PAY_OF_HUNDRED_DAYS_END_TS) + 24*60*60)
		&& pdata->awardflag[2] == 0)
	{
		pdata->flag = 3;
		pdata->awardflag[2] = 3;

		for(int i=0; i<5; i++)
		{
			UpdatesHundDays temp;

			temp.uid = pdata->HDAcvitityRank[i].uid;
			temp.s = "IntegralRank";
			temp.rank = i+1;
			temp.ts = Time::GetGlobalTime();
			temp.pay = pdata->HDAcvitityRank[i].pay;
			data.push_back(temp);
//			error_log("test333333333:%u | %u | %d",data[i].uid,data[i].pay,data[i].rank);
		}
	}
	return 0;
}


int CDataHDaysAcvitityRank::CleanRankList(void)
{
	AcvitityPayRank *pdata = (AcvitityPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_hundreddaysavtivityrank_fail");
	}
	CAutoLock lock(&m_sh, true);
	memset(pdata, 0, sizeof(*pdata));
	return 0;
}
