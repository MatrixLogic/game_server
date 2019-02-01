/*
 * DataNewUserRecList.cpp
 *
 *  Created on: 2013-8-9
 *      Author: Administrator
 */

#include "DataNewUserRecList.h"

CDataNewUserRecList::CDataNewUserRecList()
{
	m_init = false;
}

int CDataNewUserRecList::Init(const string &path, semdat sem)
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
	if(!m_sh.CreateOrOpen(path.c_str(), sizeof(DataNewUserRecList), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init_broadcast_data_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{
		DataNewUserRecList *pdata = (DataNewUserRecList *)m_sh.GetAddress();
		memset(pdata, 0, sizeof(*pdata));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}

int CDataNewUserRecList::GetRecList(unsigned *list, const int count)
{
	if (count < 0 || count > MAX_REC_LIST_COUNT)
	{
		return R_ERR_PARAM;
	}
	DataNewUserRecList *pdata = (DataNewUserRecList *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	memcpy(list, pdata->list, sizeof(unsigned) * count);
	return 0;
}

int CDataNewUserRecList::SetRecList(unsigned uid)
{
	DataNewUserRecList *pdata = (DataNewUserRecList *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	int item = uid % MAX_REC_LIST_COUNT;
	if (item < 0 || item > MAX_REC_LIST_COUNT)
	{
		return R_ERR_DATA;
	}
	pdata->list[item] = uid;
	return 0;
}
