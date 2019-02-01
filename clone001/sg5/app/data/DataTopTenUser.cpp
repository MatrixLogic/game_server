/*
 * DataTopTenUser.cpp
 *
 *  Created on: 2013-3-6
 *      Author: Administrator
 */

#include "DataTopTenUser.h"

CDataTopTenUser::CDataTopTenUser() {
	m_init = false;
}

CDataTopTenUser::~CDataTopTenUser() {}


static int sortLevel(const DataUserLevel& left,const DataUserLevel& right){
	return left.level > right.level;
}

static int sortPower(const DataHeroPower& left,const DataHeroPower& right){
	return left.power > right.power;
}

static int sortLoot(const DataUserPlunder& left,const DataUserPlunder& right){
	return left.plunder > right.plunder;
}

int CDataTopTenUser::Init(const std::string &path, semdat sem)
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
	if(!m_sh.CreateOrOpen(path.c_str(), sizeof(DataTopTenUser), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init_DataTopTenUser_data_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{
		DataTopTenUser *pdata = (DataTopTenUser *)m_sh.GetAddress();
		if(pdata == NULL)
		{
			DB_ERROR_RETURN_MSG("init_BarbarianInvasion_fail");
		}
		memset(pdata, 0x00, sizeof(*pdata));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}

int CDataTopTenUser::GetInfo(DataTopTenUser &data)
{
	DataTopTenUser *pdata = (DataTopTenUser *)m_sh.GetAddress();
	if (NULL == pdata)
	{
		error_log("GetAddress fail!");
		return R_ERR_DATA;
	}
	CAutoLock lock(&m_sh, true);
	for (int i=0; i<MAX_LIST_SIZE; i++)
	{
		data.userLevel[i] = pdata->userLevel[i];
		data.heroPower[i] = pdata->heroPower[i];
		data.userPlunder[i] = pdata->userPlunder[i];
	}
	return R_SUCCESS;
}

int CDataTopTenUser::SetUserLevel(const DataUserLevel &data)
{
	int ret = 0;
	bool inFlag = false;
	DataTopTenUser *pdata = (DataTopTenUser *)m_sh.GetAddress();
	if (NULL == pdata)
	{
		error_log("GetAddress fail!");
		return R_ERR_DATA;
	}
	CAutoLock lock(&m_sh, true);
	int i = 0;
	for(i = MAX_LIST_SIZE -1;i >= 0; i--)
	{
		if(pdata->userLevel[i].uid == data.uid  )
		{
			pdata->userLevel[i].level = data.level;
			inFlag = true;
			break;
		}
	}
	if(!inFlag){
		pdata->userLevel[MAX_LIST_SIZE] = data;
	}
	else{
		memset(&(pdata->userLevel[MAX_LIST_SIZE]),0,sizeof(DataUserLevel));
	}

	vector<DataUserLevel> vecPoint;
	for(i = 0; i<=MAX_LIST_SIZE;++i){
		vecPoint.push_back(pdata->userLevel[i]);
	}

	sort(vecPoint.begin(),vecPoint.end(),sortLevel);

	for(i = 0; i<=MAX_LIST_SIZE;++i){
		pdata->userLevel[i] = vecPoint[i];
	}
	return 0;
}

int CDataTopTenUser::SetHeroPower(const DataHeroPower &data)
{
	int ret = 0;
	bool inFlag = false;
	DataTopTenUser *pdata = (DataTopTenUser *)m_sh.GetAddress();
	if (NULL == pdata)
	{
		error_log("GetAddress fail!");
		return R_ERR_DATA;
	}
	CAutoLock lock(&m_sh, true);
	int i = 0;
	for(i = MAX_LIST_SIZE -1;i >= 0; i--)
	{
		if(pdata->heroPower[i].uid == data.uid  )
		{
			pdata->heroPower[i].power = data.power;
			inFlag = true;
			break;
		}
	}
	if(!inFlag){
		pdata->heroPower[MAX_LIST_SIZE] = data;
	}
	else{
		memset(&(pdata->heroPower[MAX_LIST_SIZE]),0,sizeof(DataHeroPower));
	}

	vector<DataHeroPower> vecPoint;
	for(i = 0; i<=MAX_LIST_SIZE;++i){
		vecPoint.push_back(pdata->heroPower[i]);
	}

	sort(vecPoint.begin(),vecPoint.end(),sortPower);

	for(i = 0; i<=MAX_LIST_SIZE;++i){
		pdata->heroPower[i] = vecPoint[i];
	}
	return 0;
}

int CDataTopTenUser::SetUserPlunder(const DataUserPlunder &data)
{
	int ret = 0;
	bool inFlag = false;
	DataTopTenUser *pdata = (DataTopTenUser *)m_sh.GetAddress();
	if (NULL == pdata)
	{
		error_log("GetAddress fail!");
		return R_ERR_DATA;
	}
	CAutoLock lock(&m_sh, true);
	int i = 0;
	for(i = MAX_LIST_SIZE -1;i >= 0; i--)
	{
		if(pdata->userPlunder[i].uid == data.uid  )
		{
			pdata->userPlunder[i].plunder = data.plunder;
			inFlag = true;
			break;
		}
	}
	if(!inFlag){
		pdata->userPlunder[MAX_LIST_SIZE] = data;
	}
	else{
		memset(&(pdata->userPlunder[MAX_LIST_SIZE]),0,sizeof(DataUserPlunder));
	}

	vector<DataUserPlunder> vecPoint;
	for(i = 0; i<=MAX_LIST_SIZE;++i){
		vecPoint.push_back(pdata->userPlunder[i]);
	}

	sort(vecPoint.begin(),vecPoint.end(),sortLoot);

	for(i = 0; i<=MAX_LIST_SIZE;++i){
		pdata->userPlunder[i] = vecPoint[i];
	}
	return 0;
}

int CDataTopTenUser::SetRewardFlage(int type)
{
	DataTopTenUser *pdata = (DataTopTenUser *)m_sh.GetAddress();
	if (NULL == pdata)
	{
		error_log("GetAddress fail!");
		return R_ERR_DATA;
	}
	CAutoLock lock(&m_sh, true);
	pdata->reward_flag = type;
	return 0;
}

int CDataTopTenUser::GetRewardFlage(int &flag)
{
	DataTopTenUser *pdata = (DataTopTenUser *)m_sh.GetAddress();
	if (NULL == pdata)
	{
		error_log("GetAddress fail!");
		return R_ERR_DATA;
	}
	CAutoLock lock(&m_sh, true);
	flag = pdata->reward_flag;
	return 0;
}
