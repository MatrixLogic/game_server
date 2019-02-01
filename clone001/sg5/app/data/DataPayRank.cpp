/*
 * DataPayRank.cpp
 *
 *  Created on: 2012-11-7
 *      Author: Administrator
 */

#include "DataPayRank.h"

CDataPayRank::CDataPayRank() {
	// TODO Auto-generated constructor stub
	m_init =false;
}

CDataPayRank::~CDataPayRank() {
	// TODO Auto-generated destructor stub
}



int  CDataPayRank::Init(const std::string &path, semdat sem)
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
	if(!m_sh.CreateOrOpen(path.c_str(), sizeof(DataPayRank), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init__joincard_data_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{
		DataPayRank *pdata = (DataPayRank *)m_sh.GetAddress();
		if(pdata == NULL)
		{
			DB_ERROR_RETURN_MSG("init_pointsrank_fail");
		}
		memset(pdata, 0, sizeof(*pdata));
		pdata->last_ts = Time::GetGlobalTime();
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}
int sortFunc(const PayItem& left,const PayItem& right){
	return left.payTotal > right.payTotal;
}
int CDataPayRank::UpdateRank(const PayItem &payItem)
{
	int i= 0;
	bool inFlag = false;  // 用户是否在排行榜中
	DataPayRank *pdata = (DataPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}
	CAutoLock lock(&m_sh, true);
	for(i = PAY_RANK_SIZE -1;i >= 0; i--)
	{
		if(pdata->payRank[i].uid == payItem.uid  )
		{
			pdata->payRank[i] = payItem;
			// 如果用户已在排行榜，inFlag = true
			inFlag = true;
			break;
		}
	}
	if(!inFlag){
		pdata->payRank[PAY_RANK_SIZE] = payItem;
	}
	else{
		memset(&(pdata->payRank[PAY_RANK_SIZE]),0,sizeof(PayItem));
	}
	vector<PayItem> vecPoint;
	for(i = 0; i<=PAY_RANK_SIZE;++i){
		vecPoint.push_back(pdata->payRank[i]);
	}

	sort(vecPoint.begin(),vecPoint.end(),sortFunc);

	for(i = 0; i<=PAY_RANK_SIZE;++i){
		pdata->payRank[i] = vecPoint[i];
	}
	return 0;
}

int CDataPayRank::GetRank(PayItem vecPayRank[])
{
	DataPayRank *pdata = (DataPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}
	CAutoLock lock(&m_sh, true);

	memcpy(vecPayRank,pdata->payRank,sizeof(PayItem) * PAY_RANK_SIZE);

	return 0;
}
int CDataPayRank::ViewRank()
{
	DataPayRank *pdata = (DataPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}
	CAutoLock lock(&m_sh, true);
	for(int i = 0; i < PAY_RANK_SIZE;i++)
	{
		cout<<"name: "<<pdata->payRank[i].name<<" uid: "<<pdata->payRank[i].uid<<" payTotal: "<<pdata->payRank[i].payTotal<<endl;
	}
	return 0;
}

int CDataPayRank::SetReward(unsigned uid, byte flag)
{
	DataPayRank *pdata = (DataPayRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}
	CAutoLock lock(&m_sh, true);
	for(int i = 0; i < PAY_RANK_SIZE;i++)
	{
		if (pdata->payRank[i].uid == uid)
		{
			pdata->payRank[i].reward_flag = flag;
			return 0;
		}
	}
	return R_ERR_DATA;
}
