/*
 * DataNoise.cpp
 *
 *  Created on: 2011-11-07
 *      Author: Ralf
 */

#include "DataNoise.h"


CDataNoise::CDataNoise() {
	m_init = false;
}

int CDataNoise::Init(const string &path, semdat sem) {
	if (m_init) {
		return 0;
	}

	int semgroup = 0;
	int semserver = 0;//全物理服务器用一个
	if (!m_sh.CreateOrOpen(path.c_str(), sizeof(DataNoise),
			SEM_ID(sem,semgroup,semserver))) {
		error_log("[init_map_data_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if (!m_sh.HasInit()) {
		DataNoise *pTable = (DataNoise *) m_sh.GetAddress();
		memset(pTable, 0, sizeof(*pTable));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}

int CDataNoise::Add(unsigned uid,unsigned uidby)
{
	if(!IsValidUid(uid) || !IsValidUid(uidby))
		return -1;

	DataNoise *pTable = (DataNoise *) m_sh.GetAddress();
	if (pTable == NULL) {
		error_log("[add_Noise_fail]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	unsigned now = Time::GetGlobalTime();
	for(int i=0;i<NOISE_NUM;++i)
	{
		if(uid == pTable->noise[i].uid)
		{
			if(pTable->noise[i].ts + NOISE_TIME > now)
			{
				for(int j=0;j<10;++j)
				{
					if(pTable->noise[i].count[j] == uidby)
						return -1;

					if(!pTable->noise[i].count[j])
					{
						pTable->noise[i].ts = now;
						pTable->noise[i].count[j] = uidby;

						if(j == NOISE_COUNT - 1)
						{
							memset(&pTable->noise[i],0,sizeof(Noise));
							return 0;
						}
						return -1;
					}
				}
				memset(&pTable->noise[i],0,sizeof(Noise));
				return 0;
			}
			else
			{
				memset(&pTable->noise[i],0,sizeof(Noise));
				pTable->noise[i].uid = uid;
				pTable->noise[i].ts = now;
				pTable->noise[i].count[0] = uidby;
			}
			return -1;
		}
		else if(pTable->noise[i].ts + NOISE_TIME < now)
		{
			memset(&pTable->noise[i],0,sizeof(Noise));
		}
	}
	for(int i=0;i<NOISE_NUM;++i)
	{
		if(0 == pTable->noise[i].uid)
		{
			pTable->noise[i].uid = uid;
			pTable->noise[i].ts = now;
			pTable->noise[i].count[0] = uidby;
			return -1;
		}
	}
	error_log("[add_Noise_full]");
	return R_ERR_DB;
}

