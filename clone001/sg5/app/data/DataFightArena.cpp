#include "DataFightArena.h"

CDataFightArena::CDataFightArena()
{
	m_init = false;
}

int CDataFightArena::Init(const string &path, semdat sem=sem_fightarena)
{
	if(m_init)
	{
		return 0;
	}
	if (path.empty())
	{
		error_log("[init_fight_arena_fail][path=%s]", path.c_str());
		return R_ERROR;
	}

	int semgroup = 0;
	int semserver = 0;
	Config::GetDomain(semserver);
	if(!semserver)
		Config::GetDB(semserver);
	int arenaId = FIGHT_ARENA_ID_MIN;
	for (; arenaId <= FIGHT_ARENA_ID_MAX; arenaId++)
	{
		string mapfile = path;
		if (path[path.length() - 1] != '/')
			mapfile.append("/");
		mapfile.append(CTrans::ITOS(arenaId)).append(".fight");

		int index = arenaId - FIGHT_ARENA_ID_MIN;
		if(!m_sh[index].CreateOrOpen(mapfile.c_str(), sizeof(DataFightArena), SEM_ID(sem,semgroup,semserver)))
		{
			error_log("[init_fight_arena_fail][path=%s]", mapfile.c_str());
			return R_ERROR;
		}
		CAutoLock lock(&(m_sh[index]), true);
		if(!m_sh[index].HasInit())
		{
			DataFightArena *pdata = (DataFightArena *)m_sh[index].GetAddress();
			memset(pdata, 0, sizeof(*pdata));
			m_sh[index].SetInitDone();
		}
	}
	m_init = true;
	return 0;
}

int CDataFightArena::GetFightArena(int arenaId, DataFightArena &data)
{
	if (!IsValidFightArenaId(arenaId))
		return R_ERR_NO_DATA;
	int index = arenaId - FIGHT_ARENA_ID_MIN;
	DataFightArena *pdata = (DataFightArena *)m_sh[index].GetAddress();
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
	memcpy(&data, pdata, sizeof(DataFightArena));
	return 0;
}

int CDataFightArena::GetFightArenaLimit(int arenaId, DataFightArenaLimit &data)
{
	if (!IsValidFightArenaId(arenaId))
		return R_ERR_NO_DATA;
	int index = arenaId - FIGHT_ARENA_ID_MIN;
	DataFightArena *pdata = (DataFightArena *)m_sh[index].GetAddress();
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
	memcpy(&data, pdata, sizeof(DataFightArenaLimit));
	return 0;
}

int CDataFightArena::SetFightArena(int arenaId, const DataFightArena &data)
{
	if (!IsValidFightArenaId(arenaId))
		return R_ERR_DB;
	int index = arenaId - FIGHT_ARENA_ID_MIN;
	DataFightArena *pdata = (DataFightArena *)m_sh[index].GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh[index]), true);
	memcpy(pdata, &data, sizeof(DataFightArena));
	return 0;
}

int CDataFightArena::SetFightArenaLimit(int arenaId, const DataFightArenaLimit &data)
{
	if (!IsValidFightArenaId(arenaId))
		return R_ERR_DB;
	int index = arenaId - FIGHT_ARENA_ID_MIN;
	DataFightArena *pdata = (DataFightArena *)m_sh[index].GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh[index]), true);
	memcpy(pdata, &data, sizeof(DataFightArenaLimit));
	return 0;
}

bool CDataFightArena::EnableChallenge(int arenaId, unsigned challenger, unsigned addPrize, DataFightArenaLimit &data)
{
	if (!IsValidFightArenaId(arenaId))
		return false;
	int index = arenaId - FIGHT_ARENA_ID_MIN;
	DataFightArena *pdata = (DataFightArena *)m_sh[index].GetAddress();
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
		memcpy(&data, pdata, sizeof(DataFightArenaLimit));
		return true;
	}
	return false;
}

int CDataFightArena::UpdateWinFightArena(int arenaId, unsigned host, unsigned level, const string &name,
		const string &pic, const string &archive, DataFightArenaLimit &data)
{
	if (!IsValidFightArenaId(arenaId))
		return R_ERR_DB;
	int index = arenaId - FIGHT_ARENA_ID_MIN;
	DataFightArena *pdata = (DataFightArena *)m_sh[index].GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh[index]), true);
	if (host != pdata->challenger)
		return R_ERR_REFUSE;
	memcpy(&data, pdata, sizeof(DataFightArenaLimit));
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

int CDataFightArena::BreathFightArena(int arenaId, unsigned challenger)
{
	if (!IsValidFightArenaId(arenaId))
		return R_ERR_DB;
	int index = arenaId - FIGHT_ARENA_ID_MIN;
	DataFightArena *pdata = (DataFightArena *)m_sh[index].GetAddress();
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

int CDataFightArena::UpdateLoseFightArena(int arenaId, unsigned challenger, DataFightArenaLimit &data)
{
	if (!IsValidFightArenaId(arenaId))
		return R_ERR_DB;
	int index = arenaId - FIGHT_ARENA_ID_MIN;
	DataFightArena *pdata = (DataFightArena *)m_sh[index].GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh[index]), true);
	if (challenger != pdata->challenger)
		return R_ERR_REFUSE;
	memcpy(&data, pdata, sizeof(DataFightArenaLimit));
	pdata->preChallenger = pdata->challenger;
	pdata->challenger = 0;
	pdata->winCount++;
	return 0;
}
