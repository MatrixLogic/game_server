/*
 * DataGM.cpp
 *
 *  Created on: 2011-6-21
 *      Author: dada
 */

#include "DataGM.h"


CDataGM::CDataGM() {
	m_init = false;
}

int CDataGM::Init(const string &path, semdat sem) {
	if (m_init) {
		return 0;
	}

	int semgroup = 0;
	int semserver = 0;//全物理服务器用一个
	if (!m_sh.CreateOrOpen(path.c_str(), sizeof(DataGM),
			SEM_ID(sem,semgroup,semserver))) {
		error_log("[init_map_data_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if (!m_sh.HasInit()) {
		DataGM *pTable = (DataGM *) m_sh.GetAddress();
		memset(pTable, 0, sizeof(*pTable));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}

int CDataGM::Query(Json::Value &buff, unsigned flag)
{
	DataGM *pTable = (DataGM *) m_sh.GetAddress();
	if (pTable == NULL) {
		error_log("[quer_gm_fail]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	for(int i=0;i<GM_NUM;++i)
	{
		if(flag & pTable->gm[i].flag)
		{
			string temp = pTable->gm[i].openid;
			if(!temp.empty())
				buff[buff.size()] = temp;
		}
	}
	return 0;
}

int CDataGM::Add(string& buff, unsigned flag)
{
	DataGM *pTable = (DataGM *) m_sh.GetAddress();
	if (pTable == NULL) {
		error_log("[add_gm_fail]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	for(int i=0;i<GM_NUM;++i)
	{
		string temp = pTable->gm[i].openid;
		if(temp == buff)
		{
			pTable->gm[i].flag |= flag;
			return 0;
		}
	}
	for(int i=0;i<GM_NUM;++i)
	{
		string temp = pTable->gm[i].openid;
		if(temp.empty())
		{
			pTable->gm[i].flag |= flag;
			strcpy(pTable->gm[i].openid , buff.c_str());
			return 0;
		}
	}
	error_log("[add_gm_full]");
	return R_ERR_DB;
}

int CDataGM::Del(string& buff, unsigned flag)
{
	DataGM *pTable = (DataGM *) m_sh.GetAddress();
	if (pTable == NULL) {
		error_log("[del_gm_fail]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	for(int i=0;i<GM_NUM;++i)
	{
		string temp = pTable->gm[i].openid;
		if(temp == buff)
		{
			pTable->gm[i].flag &= ~flag;
			if(!pTable->gm[i].flag)
				memset(pTable->gm[i].openid,0,sizeof(pTable->gm[i].openid));
			return 0;
		}
	}
	return 0;
}

int CDataGM::Check(string& buff, unsigned &flag)
{
	DataGM *pTable = (DataGM *) m_sh.GetAddress();
	if (pTable == NULL) {
		error_log("[check_gm_fail]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	for(int i=0;i<GM_NUM;++i)
	{
		string temp = pTable->gm[i].openid;
		if(temp == buff)
		{
			flag = pTable->gm[i].flag;
			return 0;
		}
	}
	return 0;
}

