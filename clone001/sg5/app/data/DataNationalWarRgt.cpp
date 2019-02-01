/*
 * DataNationalWarRgt.cpp
 *
 *  Created on: 2013-08-06
 *      Author: Administrator
 */

#include "DataNationalWarRgt.h"



CDataNationalWarRgt::CDataNationalWarRgt(){
	m_init = false;
}
CDataNationalWarRgt::~CDataNationalWarRgt() {
	// TODO Auto-generated destructor stub
}

int  CDataNationalWarRgt::Init(const std::string &path, semdat sem)
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
	if(!m_sh.CreateOrOpen(path.c_str(), sizeof(CDataAllCityState), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init__NationalWarRgt_data_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{
		CDataAllCityState *pdata = (CDataAllCityState *)m_sh.GetAddress();
		if(pdata == NULL)
		{
			DB_ERROR_RETURN_MSG("init_NationalWarRgt_fail");
		}
		memset(pdata, 0, sizeof(*pdata));
		for(int i = 0 ;i < CITY_NUM; i++)
		{
			pdata->Cityofnumber[i].city_id = i;
		}
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}

int CDataNationalWarRgt::GetCityState( CDataCitiesInState vecCityState[])
{
	CDataAllCityState *pdata = (CDataAllCityState *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_NationalWarRgt_fail");
	}
	CAutoLock lock(&m_sh, true);
	memcpy(vecCityState,pdata->Cityofnumber,sizeof(CDataCitiesInState) * 18);
	return 0;
}

int CDataNationalWarRgt::UpdateCityState(const CDataCitiesInState &CityState,int &state)
{

	CDataAllCityState *pdata = (CDataAllCityState *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_NationalWarRgt_fail");
	}
	CAutoLock lock(&m_sh, true);

	if(pdata->Cityofnumber[CityState.city_id].chall_state != 0 )
	{
		state = 1;
	}
	else if(pdata->Cityofnumber[CityState.city_id].chall_state == 0 )
	{

		if(pdata->Cityofnumber[CityState.city_id].city_id == CityState.city_id )
		{
			pdata->Cityofnumber[CityState.city_id].ChallAlliance_id = CityState.ChallAlliance_id;
//			pdata->Cityofnumber[CityState.city_id].Chall_InviteAlliance_id[0] = CityState.Chall_InviteAlliance_id[0];
//			pdata->Cityofnumber[CityState.city_id].Chall_InviteAlliance_id[1] = CityState.Chall_InviteAlliance_id[1];
			pdata->Cityofnumber[CityState.city_id].OccAlliance_id = CityState.OccAlliance_id;
//			pdata->Cityofnumber[CityState.city_id].Occ_InviteAlliance_id[0] = CityState.Occ_InviteAlliance_id[0];
//			pdata->Cityofnumber[CityState.city_id].Occ_InviteAlliance_id[1] = CityState.Occ_InviteAlliance_id[1];
			pdata->Cityofnumber[CityState.city_id].Occ_ts = CityState.Occ_ts;
			pdata->Cityofnumber[CityState.city_id].chall_state = CityState.chall_state;
			pdata->Cityofnumber[CityState.city_id].chall_ts = CityState.chall_ts;
			pdata->Cityofnumber[CityState.city_id].chall_uid = CityState.chall_uid;
			pdata->Cityofnumber[CityState.city_id].state = CityState.state;
			pdata->Cityofnumber[CityState.city_id].city_id = CityState.city_id;
			if(CityState.ChallAlliance_id != 0)
			{
				memcpy(pdata->Cityofnumber[CityState.city_id].Challname,CityState.Challname,sizeof(CityState.Challname) - 1);
			}
//			if(CityState.Chall_InviteAlliance_id[0] != 0)
//			{
//				memcpy(pdata->Cityofnumber[CityState.city_id].Challname0,CityState.Challname0,sizeof(CityState.Challname0) - 1);
//			}
//			if(CityState.Chall_InviteAlliance_id[1] != 0)
//			{
//				memcpy(pdata->Cityofnumber[CityState.city_id].Challname1,CityState.Challname1,sizeof(CityState.Challname1) - 1);
//			}
			if(CityState.OccAlliance_id != 0)
			{
				memcpy(pdata->Cityofnumber[CityState.city_id].Occname,CityState.Occname,sizeof(CityState.Occname) - 1);
			}
//			if(CityState.Chall_InviteAlliance_id[0] != 0)
//			{
//				memcpy(pdata->Cityofnumber[CityState.city_id].Occname0,CityState.Occname0,sizeof(CityState.Occname0) - 1);
//			}
//			if(CityState.Chall_InviteAlliance_id[1] != 0)
//			{
//				memcpy(pdata->Cityofnumber[CityState.city_id].Occname1,CityState.Occname1,sizeof(CityState.Occname1) - 1);
//			}

		}
	}
	return 0;
}


int CDataNationalWarRgt::InviteSuccess(const CDataCitiesInState &CityState,int &state)
{
	CDataAllCityState *pdata = (CDataAllCityState *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("clean_NationalWarRgt_fail");
	}
	CAutoLock lock(&m_sh, true);
	if(pdata->Cityofnumber[CityState.city_id].Chall_InviteAlliance_id[0] != 0
	 &&pdata->Cityofnumber[CityState.city_id].Chall_InviteAlliance_id[1] != 0)
	{
		state = 1;
	}
	else if(pdata->Cityofnumber[CityState.city_id].Occ_InviteAlliance_id[0] != 0
	 &&pdata->Cityofnumber[CityState.city_id].Occ_InviteAlliance_id[1] != 0)
	{
		state = 1;
	}
	else
	{

		if(pdata->Cityofnumber[CityState.city_id].city_id == CityState.city_id )
		{
			pdata->Cityofnumber[CityState.city_id].Chall_InviteAlliance_id[0] = CityState.Chall_InviteAlliance_id[0];
			pdata->Cityofnumber[CityState.city_id].Chall_InviteAlliance_id[1] = CityState.Chall_InviteAlliance_id[1];
			pdata->Cityofnumber[CityState.city_id].Occ_InviteAlliance_id[0] = CityState.Occ_InviteAlliance_id[0];
			pdata->Cityofnumber[CityState.city_id].Occ_InviteAlliance_id[1] = CityState.Occ_InviteAlliance_id[1];

			if(CityState.Chall_InviteAlliance_id[0] != 0)
			{
				memcpy(pdata->Cityofnumber[CityState.city_id].Challname0,CityState.Challname0,sizeof(CityState.Challname0) - 1);
			}
			if(CityState.Chall_InviteAlliance_id[1] != 0)
			{
				memcpy(pdata->Cityofnumber[CityState.city_id].Challname1,CityState.Challname1,sizeof(CityState.Challname1) - 1);
			}

			if(CityState.Chall_InviteAlliance_id[0] != 0)
			{
				memcpy(pdata->Cityofnumber[CityState.city_id].Occname0,CityState.Occname0,sizeof(CityState.Occname0) - 1);
			}
			if(CityState.Chall_InviteAlliance_id[1] != 0)
			{
				memcpy(pdata->Cityofnumber[CityState.city_id].Occname1,CityState.Occname1,sizeof(CityState.Occname1) - 1);
			}

		}
	}
	return 0;
}


int CDataNationalWarRgt::CleanNationalWarRgt(void)
{
	CDataAllCityState *pdata = (CDataAllCityState *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("clean_NationalWarRgt_fail");
	}
	CAutoLock lock(&m_sh, true);
	memset(pdata, 0, sizeof(*pdata));
	return 0;
}
