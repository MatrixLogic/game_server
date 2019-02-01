/*
 * DataQixijie.cpp
 *
 *  Created on: 2013-08-06
 *      Author: Administrator
 */

#include "DataQixi.h"
#include "DataUserBasic.h"


CDataQixijie::CDataQixijie(){
	m_init = false;
}
CDataQixijie::~CDataQixijie() {
	// TODO Auto-generated destructor stub
}

int  CDataQixijie::Init(const std::string &path, semdat sem)
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
	if(!m_sh.CreateOrOpen(path.c_str(), sizeof(QixiAcvitityPayRank), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init__qixi_data_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{
		QixiAcvitityPayRank *pdata = (QixiAcvitityPayRank *)m_sh.GetAddress();
		if(pdata == NULL)
		{
			DB_ERROR_RETURN_MSG("init_qixi_fail");
		}
		memset(pdata, 0, sizeof(*pdata));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}
int sortFunc(const DataQixiRank& left,const DataQixiRank& right){
	return left.pay > right.pay;
}
int CDataQixijie::UpdateRankList(const DataQixiRank &points)
{
	int i= 0;
	bool inFlag = false;  // 用户是否在排行榜中
	QixiAcvitityPayRank *pdata = (QixiAcvitityPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_qixiactivity_fail");
	}
	CAutoLock lock(&m_sh, true);
	for(i = RANK_SIZE -1;i >= 0; i--)
	{
		if(pdata->QixiAcvitityRank[i].uid == points.uid  )
		{
			pdata->QixiAcvitityRank[i].pay = points.pay;
			// 如果用户已在排行榜，inFlag = true
			inFlag = true;
			break;
		}
	}
	if(!inFlag)
	{
		pdata->QixiAcvitityRank[RANK_SIZE] = points;
	}
	else
	{
		memset(&(pdata->QixiAcvitityRank[RANK_SIZE]),0,sizeof(DataQixiRank));
	}

	vector<DataQixiRank> vecQixi;
	for(i = 0; i<=RANK_SIZE;++i)
	{
		vecQixi.push_back(pdata->QixiAcvitityRank[i]);
	}

	sort(vecQixi.begin(),vecQixi.end(),sortFunc);

	for(i = 0; i<=RANK_SIZE;++i){
		pdata->QixiAcvitityRank[i] = vecQixi[i];
	}
	return 0;
}

int CDataQixijie::GetRankList( DataQixiRank vecPoints[])
{
	QixiAcvitityPayRank *pdata = (QixiAcvitityPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_qixiactivity_fail");
	}
	CAutoLock lock(&m_sh, true);
	memcpy(vecPoints,pdata->QixiAcvitityRank,sizeof(DataQixiRank) * RANK_SIZE);
	return 0;
}

int CDataQixijie::SetPrizeNum(const  DataGetWinners &prize)
{
	unsigned now = Time::GetGlobalTime();
	QixiAcvitityPayRank *pdata = (QixiAcvitityPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_qixiactivity_fail");
	}
	CAutoLock lock(&m_sh, true);

	for(int i = 0; i < 4; i++ )
	{
		if(CTime::GetDayInterval(pdata->GetRewards[i].ts, now) != 0)
		{
			memset(pdata->GetRewards, 0, sizeof(pdata->GetRewards));
			break;
		}

	}
	for(int i = 0; i < 4; i++ )
	{
		if(pdata->GetRewards[i].uid == 0)
		{

			pdata->GetRewards[i].reward = prize.reward;
			pdata->GetRewards[i].uid = prize.uid;
			pdata->GetRewards[i].ts = now;
			memcpy(pdata->GetRewards[i].name,prize.name,sizeof(prize.name) - 1);
//			error_log("aaaaaaaaaa:%u | %d",pdata->GetRewards[i].uid,pdata->GetRewards[i].reward);
			break;
		}
	}

	return 0;

}

int CDataQixijie::GetPrizeNum(DataGetWinners vecPrize[])
{
	QixiAcvitityPayRank *pdata = (QixiAcvitityPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_qixiactivity_fail");
	}
	CAutoLock lock(&m_sh, true);

	memcpy(vecPrize,pdata->GetRewards,sizeof(pdata->GetRewards));
	return 0;
}

int CDataQixijie::SetRecordLotterynumber()
{
	unsigned now = Time::GetGlobalTime();

	QixiAcvitityPayRank *pdata = (QixiAcvitityPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_qixiactivity_fail");
	}
	CAutoLock lock(&m_sh, true);
	if(CTime::GetDayInterval(pdata->ts, now) != 0)
	{
		pdata->ts = now;
		//EQUIPMENT_LOG("uid=%u,moneyTotal=%u,ts=%d,act=%s,code=%s,data=%s",0,pdata->Lotterynumber*20,now,"qixijie","ConsumptionData","money");
		pdata->Lotterynumber = 1;
	}
	else
	{
		pdata->ts = now;
		pdata->Lotterynumber = pdata->Lotterynumber + 1;
	}
	return 0;
}

int CDataQixijie::GetRecordLotterynumber(int &num)
{
	QixiAcvitityPayRank *pdata = (QixiAcvitityPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_qixiactivity_fail");
	}
	CAutoLock lock(&m_sh, true);
	num = pdata->Lotterynumber;

	return 0;
}
int CDataQixijie::GetMarkreward(int &markReward)
{
	QixiAcvitityPayRank *pdata = (QixiAcvitityPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_qixiactivity_fail");
	}
	CAutoLock lock(&m_sh, true);
	markReward = pdata->markreward;

	return 0;

}

int CDataQixijie::LotteryResults(unsigned uid,Json::Value input,int s1, int s2,int platform)
{
	QixiAcvitityPayRank *pdata = (QixiAcvitityPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("clean_qixiactivity_fail");
	}
	CAutoLock lock(&m_sh, true);
	int Lotterynumber = pdata->Lotterynumber;

	if(Lotterynumber <= 288 )
	{
		int mark = rand()%s1;
		int s0 = 0;

		if(mark >= 0 && mark < input["qixifirstlayer"]["rate"][0u].asInt())
		{
			pdata->markreward = input["qixifirstlayer"]["grid"][0u].asInt();
		}
		for(int i = 0; i < 9; i++)
		{

			s0 += input["qixifirstlayer"]["rate"][i].asInt();

			if(mark >= s0
			&& mark < s0 + input["qixifirstlayer"]["rate"][i+1].asInt())
			{
				pdata->markreward = input["qixifirstlayer"]["grid"][i+1].asInt();
				break;
			}
		}
	}

	else if(Lotterynumber > 288 )
	{
		int mark = rand()%s2;
		int s0 = 0;

		if(mark >= 0 && mark < input["qixisecondlayer"]["rate"][0u].asInt())
		{
			pdata->markreward = input["qixisecondlayer"]["grid"][0u].asInt();
		}
		for(int i = 1; i < 24; i++)
		{
			s0 += input["qixisecondlayer"]["rate"][i].asInt() ;
			if(mark >= s0
					&& mark < s0 + input["qixisecondlayer"]["rate"][i+1].asInt())
			{
				pdata->markreward = input["qixisecondlayer"]["grid"][i+1].asInt();
				break;
			}
		}
	}

	if(pdata->markreward == 11 || pdata->markreward == 14 || pdata->markreward == 18 || pdata->markreward == 21
		|| pdata->markreward == 9 || pdata->markreward == 10 || pdata->markreward == 19 || pdata->markreward == 24)
	{

		if(pdata->markreward == 11 )
		{
			pdata->prizenum[0] = pdata->prizenum[0] +1;
			if(pdata->prizenum[0] >= 5)
			{
				pdata->prizenum[0] = 5;
				pdata->markreward = pdata->markreward + 1;

			}

		}
		if(pdata->markreward == 14 )
		{
			pdata->prizenum[1] = pdata->prizenum[1] +1;
			if(pdata->prizenum[1] >= 5)
			{
				pdata->prizenum[1] = 5;
				pdata->markreward = pdata->markreward + 1;
			}

		}
		if(pdata->markreward == 18 )
		{
			pdata->prizenum[2] = pdata->prizenum[2] +1;
			if(pdata->prizenum[2] >= 5)
			{
				pdata->prizenum[2] = 5;
				pdata->markreward = pdata->markreward + 1;
			}

		}
		if(pdata->markreward == 21 )
		{
			pdata->prizenum[3] = pdata->prizenum[3] +1;
			if(pdata->prizenum[3] >= 5)
			{
				pdata->prizenum[3] = 5;
				pdata->markreward = pdata->markreward + 1;
			}

		}

	}
	if(pdata->markreward == 11 || pdata->markreward == 14 || pdata->markreward == 18 || pdata->markreward == 21
		|| pdata->markreward == 9 || pdata->markreward == 10 || pdata->markreward == 19 || pdata->markreward == 24)
	{
		DataGetWinners points;
		DataUserBasic dataUserBasic;
		CDataUserBasic dbUserBasic;
		int ret = dbUserBasic.GetUserBasicLimit(uid, platform, dataUserBasic);
		if (ret != 0)
		{
			error_log("[GetUserBasicLimit fail][ret=%d,uid=%u,platform=%d]", ret, uid, platform);
			DB_ERROR_RETURN_MSG("db_get_uesr_basic_fail");
		}
		points.uid = uid;
		points.reward = pdata->markreward;
		points.ts = Time::GetGlobalTime();
		memcpy(points.name,dataUserBasic.name.c_str(),sizeof(points.name) - 1);


		if( 0 == pdata->GetRewards[0].ts || 0 == pdata->GetRewards[1].ts
		|| 0 == pdata->GetRewards[2].ts || 0 == pdata->GetRewards[3].ts)
		{
			for(int i = 0; i < 4; i++ )
			{
				if(pdata->GetRewards[i].uid == 0 && 0 == pdata->GetRewards[i].ts)
				{
					pdata->GetRewards[i].reward = points.reward;
					pdata->GetRewards[i].uid = points.uid;
					pdata->GetRewards[i].ts = now;
					memcpy(pdata->GetRewards[i].name,points.name,sizeof(points.name) - 1);
					error_log("aaaaaaaaaa:%u | %d",pdata->GetRewards[i].uid,pdata->GetRewards[i].reward);
					break;
				}
			}
		}
		else if( 0 != pdata->GetRewards[0].ts && 0 != pdata->GetRewards[1].ts
				&& 0 != pdata->GetRewards[2].ts && 0 != pdata->GetRewards[3].ts)
		{
			for(int i = 0; i < 3; i++)
			{
				pdata->GetRewards[i].reward = pdata->GetRewards[i+1].reward;
				pdata->GetRewards[i].uid = pdata->GetRewards[i+1].uid;
				pdata->GetRewards[i].ts = pdata->GetRewards[i+1].ts;
				memcpy(pdata->GetRewards[i].name,pdata->GetRewards[i+1].name,sizeof(pdata->GetRewards[i+1].name) - 1);

			}
			pdata->GetRewards[3].reward = points.reward;
			pdata->GetRewards[3].uid = points.uid;
			pdata->GetRewards[3].ts = now;
			memcpy(pdata->GetRewards[3].name,points.name,sizeof(points.name) - 1);
		}
	}

	return 0;
}

int CDataQixijie::GetPrizeNum11(int *num)
{
	QixiAcvitityPayRank *pdata = (QixiAcvitityPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("clean_qixiactivity_fail");
	}
	CAutoLock lock(&m_sh, true);
	for(int i =0; i < 4; i++)
	{
		num[i] = pdata->prizenum[i];
	}
	return 0;
}

int CDataQixijie::Setrewardflag()
{
	QixiAcvitityPayRank *pdata = (QixiAcvitityPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("clean_qixiactivity_fail");
	}
	CAutoLock lock(&m_sh, true);
	pdata->rewardflag = pdata->rewardflag + 1;
	return 0;
}


int CDataQixijie::Getrewardflag(int &rewardflag)
{
	QixiAcvitityPayRank *pdata = (QixiAcvitityPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("clean_qixiactivity_fail");
	}
	CAutoLock lock(&m_sh, true);
	rewardflag = pdata->rewardflag;
	return 0;
}

int CDataQixijie::CleanRankList(void)
{
	QixiAcvitityPayRank *pdata = (QixiAcvitityPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("clean_qixiactivity_fail");
	}
	CAutoLock lock(&m_sh, true);
	memset(pdata, 0, sizeof(*pdata));
	return 0;
}
