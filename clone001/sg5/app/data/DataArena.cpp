#include "DataArena.h"

CDataArena::CDataArena()
{
	m_init = false;
}

int CDataArena::Init(const string &path, semdat sem)
{
	if(m_init)
	{
		return 0;
	}
	if (path.empty())
	{
		error_log("[init_arena_data_fail][path=%s]", path.c_str());
		return R_ERROR;
	}

	int semgroup = 0;
	int semserver = 0;
	Config::GetDomain(semserver);
	if(!semserver)
		Config::GetDB(semserver);
	int arenaId = ARENA_ID_MIN;
	for (; arenaId <= ARENA_ID_MAX; arenaId++)
	{
		string mapfile = path;
		if (path[path.length() - 1] != '/')
			mapfile.append("/");
		mapfile.append(CTrans::ITOS(arenaId)).append(".arena");
		int index = arenaId - ARENA_ID_MIN;
		if(!m_sh[index].CreateOrOpen(mapfile.c_str(), sizeof(DataArena), SEM_ID(sem,semgroup,semserver)))
		{
			error_log("[init_arena_data_fail][path=%s]", mapfile.c_str());
			return R_ERROR;
		}
		CAutoLock lock(&(m_sh[index]), true);
		if(!m_sh[index].HasInit())
		{
			DataArena *pdata = (DataArena *)m_sh[index].GetAddress();
			memset(pdata, 0, sizeof(*pdata));
			m_sh[index].SetInitDone();
		}
	}
	m_init = true;
	return 0;
}

int CDataArena::GetArena(int arenaId, DataArena &data)
{
	if (!IsValidArenaId(arenaId))
		return R_ERR_NO_DATA;
	int index = arenaId - ARENA_ID_MIN;
	DataArena *pdata = (DataArena *)m_sh[index].GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh[index]), true);
	if (pdata->challenger != 0 && !IsBeingAttacked(pdata->breathTs))
	{
		pdata->winCount++;
		pdata->preChallenger = pdata->challenger;
		pdata->challenger = 0;
	}
	memcpy(&data, pdata, sizeof(DataArena));
	return 0;
}

int CDataArena::GetArenaLimit(int arenaId, DataArenaLimit &data)
{
	if (!IsValidArenaId(arenaId))
		return R_ERR_NO_DATA;
	int index = arenaId - ARENA_ID_MIN;
	DataArena *pdata = (DataArena *)m_sh[index].GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh[index]), true);
	if (pdata->challenger != 0 && !IsBeingAttacked(pdata->breathTs))
	{
		pdata->winCount++;
		pdata->preChallenger = pdata->challenger;
		pdata->challenger = 0;
	}
	memcpy(&data, pdata, sizeof(DataArenaLimit));
	return 0;
}

int CDataArena::SetArena(int arenaId, const DataArena &data)
{
	if (!IsValidArenaId(arenaId))
		return R_ERR_DB;
	int index = arenaId - ARENA_ID_MIN;
	DataArena *pdata = (DataArena *)m_sh[index].GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh[index]), true);
	memcpy(pdata, &data, sizeof(DataArena));
	return 0;
}

int CDataArena::SetArenaLimit(int arenaId, const DataArenaLimit &data)
{
	if (!IsValidArenaId(arenaId))
		return R_ERR_DB;
	int index = arenaId - ARENA_ID_MIN;
	DataArena *pdata = (DataArena *)m_sh[index].GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh[index]), true);
	memcpy(pdata, &data, sizeof(DataArenaLimit));
	return 0;
}

bool CDataArena::EnableChallenge(int arenaId, unsigned challenger, unsigned addPrize, DataArena &data)
{
	if (!IsValidArenaId(arenaId))
		return false;
	int index = arenaId - ARENA_ID_MIN;
	DataArena *pdata = (DataArena *)m_sh[index].GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return false;
	}
	CAutoLock lock(&(m_sh[index]), true);
	if (pdata->challenger != 0 && !IsBeingAttacked(pdata->breathTs))
	{
		pdata->winCount++;
		pdata->preChallenger = pdata->challenger;
		pdata->challenger = 0;
	}
	if (pdata->challenger == 0)
	{
		pdata->challenger = challenger;
		pdata->challengeTs = Time::GetGlobalTime();
		pdata->breathTs = Time::GetGlobalTime();
		pdata->prizePool += addPrize;
		memcpy(&data, pdata, sizeof(DataArena));
		return true;
	}
	return false;
}

int CDataArena::UpdateWinArena(int arenaId, unsigned host, unsigned level, const string &name,
		const string &pic, const string &archive, DataArenaLimit &data)
{
	if (!IsValidArenaId(arenaId))
		return R_ERR_DB;
	int index = arenaId - ARENA_ID_MIN;
	DataArena *pdata = (DataArena *)m_sh[index].GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh[index]), true);
	if (host != pdata->challenger)
		return R_ERR_REFUSE;
	memcpy(&data, pdata, sizeof(DataArenaLimit));
	pdata->preHost = pdata->host;
	pdata->preChallenger = pdata->challenger;
	pdata->prizePool = 0;
	pdata->challenger = 0;
	pdata->winCount = 0;
	pdata->host = host;
	pdata->level = level;
	snprintf(pdata->name, sizeof(pdata->name), "%s", name.c_str());
	snprintf(pdata->pic, sizeof(pdata->pic), "%s", pic.c_str());
	snprintf(pdata->archive, sizeof(pdata->archive), "%s", archive.c_str());
	return 0;
}

int CDataArena::BreathArena(int arenaId, unsigned challenger)
{
	if (!IsValidArenaId(arenaId))
		return R_ERR_DB;
	int index = arenaId - ARENA_ID_MIN;
	DataArena *pdata = (DataArena *)m_sh[index].GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh[index]), true);
	if (pdata->challenger == challenger)
		pdata->breathTs = Time::GetGlobalTime();
	return 0;
}
