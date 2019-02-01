/*
 * DataGroupsFighting.cpp
 *
 *  Created on: 2013-08-06
 *      Author: Administrator
 */

#include "DataGroupsFighting.h"



CDataGroupsFigthing::CDataGroupsFigthing(){
	m_init = false;
}
CDataGroupsFigthing::~CDataGroupsFigthing() {
	// TODO Auto-generated destructor stub
}

int  CDataGroupsFigthing::Init(const std::string &path,int cityid, semdat sem)
{
	if(m_init)
	{
		return 0;
	}

	int semserver = 0;
	Config::GetDomain(semserver);
	if(!semserver)
		Config::GetDB(semserver);

	int semgroup = cityid;
	int index = cityid;
	if(!m_sh[index].CreateOrOpen(path.c_str(), sizeof(AllCombatData), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init__GroupsFighting_data_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh[index], true);
	if(!m_sh[index].HasInit())
	{
		AllCombatData *pdata = (AllCombatData *)m_sh[index].GetAddress();
		if(pdata == NULL)
		{
			DB_ERROR_RETURN_MSG("init_GroupsFighting_fail");
		}
		memset(pdata, 0, sizeof(*pdata));
		m_sh[index].SetInitDone();
	}
	m_init = true;
	return 0;
}


int CDataGroupsFigthing::GetAllCombatData(vector <EveryoneFormation> &All_data,int cityindex,unsigned &Data_id,unsigned &Senduid,vector <EveryoneFormation> &Self_data)
{
	AllCombatData *pdata = (AllCombatData *)m_sh[cityindex].GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_GroupsFighting_fail");
	}
	CAutoLock lock(&m_sh[cityindex], true);
	Data_id = pdata->Data_id;
	Senduid = pdata->Senduid;

	int exist = 0;
	for(int i = 0; i < MAXNUM_OF_PEOPLE; i++)
	{
		if(pdata->All_people[i].uid == Self_data[0].uid)
		{
			exist++;
		}
		if(pdata->All_people[i].uid != 0)
		{
			All_data.push_back(pdata->All_people[i]);
		}
	}

	error_log("exist=%d",exist);
	if(exist == 0)
	{
		for(int i = 0; i < MAXNUM_OF_PEOPLE; i++)
		{
//			if(pdata->All_people[i].uid != 0)
//			{
//				error_log("alldata%d|%u",i,pdata->All_people[i].uid);
//			}
			if(pdata->All_people[i].uid == 0)
			{
				pdata->All_people[i].alliance_id = Self_data[0].alliance_id;
				pdata->All_people[i].camp = Self_data[0].camp;
				pdata->All_people[i].fullmorale = Self_data[0].fullmorale;
				pdata->All_people[i].morale = Self_data[0].morale;
				pdata->All_people[i].position = Self_data[0].position;
				pdata->All_people[i].role = Self_data[0].role;
				pdata->All_people[i].state = Self_data[0].state;
				pdata->All_people[i].target = Self_data[0].target;
				pdata->All_people[i].ts = Self_data[0].ts;
				pdata->All_people[i].uid = Self_data[0].uid;
				pdata->All_people[i].personalWarData.attack = Self_data[0].personalWarData.attack;
				pdata->All_people[i].personalWarData.attackRate = Self_data[0].personalWarData.attackRate;
				pdata->All_people[i].personalWarData.critRate = Self_data[0].personalWarData.critRate;
				pdata->All_people[i].personalWarData.defence = Self_data[0].personalWarData.defence;
				pdata->All_people[i].personalWarData.dodgeRate = Self_data[0].personalWarData.dodgeRate;
				pdata->All_people[i].personalWarData.level = Self_data[0].personalWarData.level;
				pdata->All_people[i].personalWarData.maxHp = Self_data[0].personalWarData.maxHp;
				pdata->All_people[i].personalWarData.type = Self_data[0].personalWarData.type;
				pdata->All_people[i].personalWarData.typeLevels[0] = Self_data[0].personalWarData.typeLevels[0];
				pdata->All_people[i].personalWarData.typeLevels[1] = Self_data[0].personalWarData.typeLevels[1];
				pdata->All_people[i].personalWarData.typeLevels[2] = Self_data[0].personalWarData.typeLevels[2];
				error_log("first_time_enter_uid=%u|%u |%d",pdata->All_people[i].uid,pdata->All_people[i].uid,i);
				break;
			}
		}
	}
	return 0;
}

int CDataGroupsFigthing::UpdatasFightingData(vector <EveryoneFormation> &All_data,int cityindex,unsigned &Data_id,unsigned &Senduid,int &map_id)
{
	AllCombatData *pdata = (AllCombatData *)m_sh[cityindex].GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("clean_GroupsFighting_fail");
	}
	CAutoLock lock(&m_sh[cityindex], true);

	if((Data_id > pdata->Data_id)
	||(Data_id == pdata->Data_id && Senduid <= pdata->Senduid))
	{
		memset(pdata, 0, sizeof(*pdata));

		pdata->Data_id = Data_id;
		pdata->Senduid = Senduid;
		pdata->Map_id = map_id;

		for(int i = 0; i < MAXNUM_OF_PEOPLE; i++)
		{
			pdata->All_people[i].alliance_id = All_data[i].alliance_id;
			pdata->All_people[i].camp = All_data[i].camp;
			pdata->All_people[i].fullmorale = All_data[i].fullmorale;
			pdata->All_people[i].morale = All_data[i].morale;
			pdata->All_people[i].position = All_data[i].position;
			pdata->All_people[i].role = All_data[i].role;
			pdata->All_people[i].state = All_data[i].state;
			pdata->All_people[i].target = All_data[i].target;
			pdata->All_people[i].ts = All_data[i].ts;
			pdata->All_people[i].uid = All_data[i].uid;
			pdata->All_people[i].personalWarData.attack = All_data[i].personalWarData.attack;
			pdata->All_people[i].personalWarData.attackRate = All_data[i].personalWarData.attackRate;
			pdata->All_people[i].personalWarData.critRate = All_data[i].personalWarData.critRate;
			pdata->All_people[i].personalWarData.defence = All_data[i].personalWarData.defence;
			pdata->All_people[i].personalWarData.dodgeRate = All_data[i].personalWarData.dodgeRate;
			pdata->All_people[i].personalWarData.level = All_data[i].personalWarData.level;
			pdata->All_people[i].personalWarData.maxHp = All_data[i].personalWarData.maxHp;
			pdata->All_people[i].personalWarData.type = All_data[i].personalWarData.type;
			pdata->All_people[i].personalWarData.typeLevels[0] = All_data[i].personalWarData.typeLevels[0];
			pdata->All_people[i].personalWarData.typeLevels[1] = All_data[i].personalWarData.typeLevels[1];
			pdata->All_people[i].personalWarData.typeLevels[2] = All_data[i].personalWarData.typeLevels[2];
		}

	}
	else
	{

	}

	return 0;
}







int CDataGroupsFigthing::AddFullmorale(int cityindex,unsigned uid)
{
	AllCombatData *pdata = (AllCombatData *)m_sh[cityindex].GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("clean_GroupsFighting_fail");
	}
	CAutoLock lock(&m_sh[cityindex], true);
	for(int i = 0; i < MAXNUM_OF_PEOPLE; i++)
	{
		if(pdata->All_people[i].uid == uid)
		{
			pdata->All_people[i].morale = pdata->All_people[i].fullmorale;
			error_log();
		}
	}
	return 0;
}


int CDataGroupsFigthing::CleanGroupsfigthingData(int cityindex)
{
	AllCombatData *pdata = (AllCombatData *)m_sh[cityindex].GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("clean_GroupsFighting_fail");
	}
	CAutoLock lock(&m_sh[cityindex], true);
	memset(pdata, 0, sizeof(*pdata));
	return 0;
}

