/*
 * DataNominateAlliance.cpp
 *
 * Created on: 2013-01-22
 * Author: Administrator
 */

#include "DataNominateAlliance.h"

struct DataAllianecList
{
	DataNominateAlliance alliance_list[MAX_NOMINATE_ALLIANCE_NUM];
};

CDataNominateAlliance::CDataNominateAlliance() {
	m_init = false;
}

int CDataNominateAlliance::Init(const std::string &path, semdat sem)
{
	if(m_init)
	{
			return 0;
	}
	if (path.empty())
	{
		error_log("[init_match_fail][path=%s]", path.c_str());
		return R_ERROR;
	}

	int semgroup = 0;
	int semserver = 0;
	Config::GetDomain(semserver);
	if(!semserver)
		Config::GetDB(semserver);
	if(!m_sh.CreateOrOpen(path.c_str(), sizeof(DataAllianecList), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init_match_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{
		DataAllianecList *pData = (DataAllianecList *)m_sh.GetAddress();
		memset(pData, 0, sizeof(*pData));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}

int CDataNominateAlliance::GetNominateAlliance(vector<DataNominateAlliance> &allianceList)
{
	DataAllianecList *pdata = (DataAllianecList *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	allianceList.clear();
	for (int i=0; i<MAX_NOMINATE_ALLIANCE_NUM; ++i)
	{
		if (pdata->alliance_list[i].alliance_id != 0)
		{
			allianceList.push_back(pdata->alliance_list[i]);
		}
	}
	return 0;
}

int CDataNominateAlliance::SetNominateAlliance(const DataNominateAlliance &data)
{
	DataAllianecList *pdata = (DataAllianecList *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	int insertPoit = -1;
	int i = 0;
	for (int i=0; i<MAX_NOMINATE_ALLIANCE_NUM; ++i)
	{
		if (pdata->alliance_list[i].alliance_id == 0 && insertPoit == -1)
		{
			insertPoit = i;
		}
		if (pdata->alliance_list[i].alliance_id == data.alliance_id)
		{
			if (pdata->alliance_list[i].sign != data.sign)
			{
				pdata->alliance_list[i].sign = data.sign;
			}
			return 0;
		}
	}

	if (-1 == insertPoit)
	{
		error_log("alliance list is full!");
		return R_ERR_DATA;
	}

	pdata->alliance_list[insertPoit].alliance_id = data.alliance_id;
	pdata->alliance_list[insertPoit].sign = data.sign;

	return 0;
}

int CDataNominateAlliance::RemoveData(unsigned alliance_id)
{
	DataAllianecList *pdata = (DataAllianecList *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	int i = 0;
	for (i=0; i<MAX_NOMINATE_ALLIANCE_NUM; i++)
	{
		if (alliance_id == pdata->alliance_list[i].alliance_id)
		{
			pdata->alliance_list[i].sign = 0;
			pdata->alliance_list[i].alliance_id = 0;
			break;
		}
	}
	return 0;
}
