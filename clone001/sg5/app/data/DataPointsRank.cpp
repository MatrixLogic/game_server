/*
 * CDataPointsRank.cpp
 *
 *  Created on: 2012-10-16
 *      Author: Administrator
 */

#include "DataPointsRank.h"


CDataPointsRank::CDataPointsRank(){
	m_init = false;
}
CDataPointsRank::~CDataPointsRank() {
	// TODO Auto-generated destructor stub
}

int  CDataPointsRank::Init(const std::string &path, semdat sem)
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
	if(!m_sh.CreateOrOpen(path.c_str(), sizeof(PointsRank), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init__joincard_data_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{
		PointsRank *pdata = (PointsRank *)m_sh.GetAddress();
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
int sortFunc(const DataPoints& left,const DataPoints& right){
	return left.points > right.points;
}
int CDataPointsRank::UpdateRankList(const DataPoints &points)
{
	int i= 0;
	bool inFlag = false;  // 用户是否在排行榜中
	PointsRank *pdata = (PointsRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}
	CAutoLock lock(&m_sh, true);
	for(i = RANK_SIZE -1;i >= 0; i--)
	{
		if(pdata->pointRank[i].uid == points.uid  )
		{
			pdata->pointRank[i].points = points.points;
			// 如果用户已在排行榜，inFlag = true
			inFlag = true;
			break;
		}
	}
	if(!inFlag){
		pdata->pointRank[RANK_SIZE] = points;
	}
	else{
		memset(&(pdata->pointRank[RANK_SIZE]),0,sizeof(DataPoints));
	}

	vector<DataPoints> vecPoint;
	for(i = 0; i<=RANK_SIZE;++i){
		vecPoint.push_back(pdata->pointRank[i]);
	}

	sort(vecPoint.begin(),vecPoint.end(),sortFunc);

	for(i = 0; i<=RANK_SIZE;++i){
		pdata->pointRank[i] = vecPoint[i];
	}
	return 0;
}

int CDataPointsRank::GetRankList( DataPoints vecPoints[])
{
	PointsRank *pdata = (PointsRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}
	CAutoLock lock(&m_sh, true);
	memcpy(vecPoints,pdata->pointRank,sizeof(DataPoints) * RANK_SIZE);
	return 0;
}

int CDataPointsRank::SetReward(unsigned uid, byte reward)
{
	PointsRank *pdata = (PointsRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}
	CAutoLock lock(&m_sh, true);
	for (int i=0; i<RANK_SIZE; i++)
	{
		if (pdata->pointRank[i].uid == uid)
		{
			pdata->pointRank[i].reward = reward;
			return 0;
		}
	}
	return R_ERROR;
}

int CDataPointsRank::CleanRankList(void)
{
	PointsRank *pdata = (PointsRank *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_pointsrank_fail");
	}
	CAutoLock lock(&m_sh, true);
	memset(pdata, 0, sizeof(*pdata));
	return 0;
}

