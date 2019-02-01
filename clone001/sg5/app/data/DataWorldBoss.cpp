#include "DataWorldBoss.h"

#define WORLDBOSS_ATTACKERS_MAX (WORLDBOSS_CHALLENGERS_MAX-1)
#define WORLDBOSS_LAST_ATTACK_INDEX (WORLDBOSS_CHALLENGERS_MAX-1)
#define STAND_TIME_LEVEL_1 1
#define STAND_TIME_LEVEL_2 2
#define STAND_TIME_LEVEL_3 3
#define STAND_TIME_LEVEL_4 4
#define STAND_TIME_LEVEL_5 5
#define STAND_TIME_LEVEL_6 6
CDataWorldBoss::CDataWorldBoss()
{
	m_init = false;
}
int CDataWorldBoss::Init(const string &path, const vector<int> &vMinBloods,const vector<int> &vMaxBlood, semdat sem)
{
	if(m_init)
	{
		return 0;
	}
	if (path.empty() || (vMinBloods.size() != WORLD_BOSS_ID_MAX - WORLD_BOSS_ID_MIN + 1) || (vMaxBlood.size() != WORLD_BOSS_ID_MAX - WORLD_BOSS_ID_MIN + 1))
	{
		error_log("[init_worldboss_fail][path=%s,minbloods=%d ,maxbloods=%d]", path.c_str(),vMinBloods.size(),vMaxBlood.size());
		return R_ERROR;
	}

	int semgroup = 0;
	int semserver = 0;
	Config::GetDomain(semserver);
	if(!semserver)
		Config::GetDB(semserver);
	int bossId = WORLD_BOSS_ID_MIN;
	for (; bossId <= WORLD_BOSS_ID_MAX; bossId++)
	{
		string mapfile = path;
		if (path[path.length() - 1] != '/')
			mapfile.append("/");
		mapfile.append(CTrans::ITOS(bossId)).append(".boss");
		int index = bossId - WORLD_BOSS_ID_MIN;
		m_maxWorldbossBlood[index] = vMaxBlood[index];
		m_minWorldbossBlood[index] = vMinBloods[index];

		if(!m_sh[index].CreateOrOpen(mapfile.c_str(), sizeof(WorldBossData), SEM_ID(sem,semgroup,semserver)))
		{
			error_log("[init_worldboss_fail][path=%s]", mapfile.c_str());
			return R_ERROR;
		}
		CAutoLock lock(&(m_sh[index]), true);
		if(!m_sh[index].HasInit())
		{
			WorldBossData *pdata = (WorldBossData *)m_sh[index].GetAddress();
			memset(pdata, 0, sizeof(WorldBossData));
			m_sh[index].SetInitDone();
		}
	}
	m_init = true;
	return 0;
}

int CDataWorldBoss::LoadWorldBoss(unsigned uid, unsigned bossId, unsigned &blood, unsigned &number, int &selfRank, WorldBossChallenger &self,
		vector<WorldBossChallenger> &top, WorldBossChallenger &last,unsigned &fullBlood)
{
	//add by aaron 20121220
	unsigned now = time(0);
	unsigned totalStandTs = 0;
	unsigned arvStandTs = 0;
	int fullBloodLevel = 1;
	int index = bossId - WORLD_BOSS_ID_MIN;
	unsigned ts = now;
	WorldBossData *pdata = NULL;
	int i = 0;

	for(i = 0; i < 4; i++)
	{
		pdata = (WorldBossData *)m_sh[i].GetAddress();
		if(pdata == NULL)
		{
			error_log("[GetAddress fail]");
			return R_ERR_DB;
		}
		CAutoLock lock(&(m_sh[i]), true);
		if (CTime::GetDayInterval(pdata->ts, now) != 0)
		{
			totalStandTs += pdata->standts;
			if(index == i)
			{
				ts = pdata->ts;
			}
		}
	}
	if(CTime::GetDayInterval(ts, now) != 0)
	{
		arvStandTs = totalStandTs / 4;
		if(arvStandTs < 180 && arvStandTs > 0)
		{
			fullBloodLevel = STAND_TIME_LEVEL_1;
		}else if(arvStandTs >= 180 && arvStandTs < 300) {
			fullBloodLevel = STAND_TIME_LEVEL_2;
		}else if(arvStandTs >= 300 && arvStandTs < 600 ){
			fullBloodLevel = STAND_TIME_LEVEL_3;
		}else if(arvStandTs >= 600 && arvStandTs < 900 ){
			fullBloodLevel = STAND_TIME_LEVEL_4;
		}else if(arvStandTs >= 1800){
			fullBloodLevel = STAND_TIME_LEVEL_5;
		}else{
			fullBloodLevel = STAND_TIME_LEVEL_6;
		}

		for(i = 0; i < 4; i++)
		{
			pdata = (WorldBossData *)m_sh[i].GetAddress();
			if(pdata == NULL)
			{
				error_log("[GetAddress fail]");
				return R_ERR_DB;
			}
			CAutoLock lock(&(m_sh[i]), true);
			switch(fullBloodLevel)
			{
			case STAND_TIME_LEVEL_1:
				pdata->fullBlood = pdata->fullBlood * 6;
				break;
			case STAND_TIME_LEVEL_2:
				pdata->fullBlood = pdata->fullBlood * 4;
				break;
			case STAND_TIME_LEVEL_3:
				pdata->fullBlood = pdata->fullBlood * 2;
				break;
			case STAND_TIME_LEVEL_4:
				pdata->fullBlood = pdata->fullBlood * 1.5;
				break;
			case STAND_TIME_LEVEL_5:
				pdata->fullBlood = pdata->fullBlood * 0.9;
				break;
			default:
				break;
			}
			if(pdata->fullBlood < m_minWorldbossBlood[i] )
			{
				pdata->fullBlood = m_minWorldbossBlood[i];
			}else if(pdata->fullBlood > m_maxWorldbossBlood[i] )
			{
				pdata->fullBlood = m_maxWorldbossBlood[i];
			}else
			{
				;
			}
			pdata->blood = pdata->fullBlood ;
			memset(pdata->challengers, 0, sizeof(pdata->challengers));
			pdata->challNum = 0;
			pdata->showNum = 0;
			pdata->ts = now;
			pdata->standts = 0;
		}
	}

	//add by aaron 20121220
	if (!IsValidWorldBossId(bossId))
		return R_ERR_NO_DATA;

	pdata = (WorldBossData *)m_sh[index].GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh[index]), true);

	selfRank = 0;
	fullBlood = pdata->fullBlood;
	blood = pdata->blood;
	//number = pdata->challNum;
	number = pdata->showNum;
	memset(&self, 0, sizeof(WorldBossChallenger));
	for (unsigned i = 0; i < pdata->challNum; i++)
	{
		if ((pdata->challengers[i]).uid == uid)
		{
			selfRank = i + 1;
			memcpy(&self, &(pdata->challengers[i]), sizeof(WorldBossChallenger));
			break;
		}
	}
	unsigned size = pdata->challNum >= 10?10 : pdata->challNum;
	for (unsigned i = 0; i < size; i++)
	{
		top.push_back((pdata->challengers)[i]);
	}
	last = pdata->challengers[WORLDBOSS_LAST_ATTACK_INDEX];
	return 0;
}

int CDataWorldBoss::AttackWorldBoss(unsigned uid, unsigned bossId, unsigned damage, const string &name,
		bool &dying, unsigned &blood, unsigned &number, int &selfRank, WorldBossChallenger &self,
		vector<WorldBossChallenger> &top, WorldBossChallenger &last, vector<WorldBossChallenger> &lucks
		, vector<WorldBossChallenger> &dam)
{
	if (!IsValidWorldBossId(bossId))
		return R_ERR_NO_DATA;
	int index = bossId - WORLD_BOSS_ID_MIN;
	WorldBossData *pdata = (WorldBossData *)m_sh[index].GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh[index]), true);
	dying = false;
	selfRank = 0;
	if (pdata->blood == 0 || damage == 0)
	{
		blood = pdata->blood;
		//number = pdata->challNum;
		number = pdata->showNum;
		memset(&self, 0, sizeof(WorldBossChallenger));
		for (unsigned i = 0; i < pdata->challNum; i++)
		{
			if ((pdata->challengers[i]).uid == uid)
			{
				selfRank = i + 1;
				memcpy(&self, &(pdata->challengers[i]), sizeof(WorldBossChallenger));
				break;
			}
		}
		unsigned size = pdata->challNum >= 10?10 : pdata->challNum;
		for (unsigned i = 0; i < size; i++)
		{
			top.push_back((pdata->challengers)[i]);
		}
		last = pdata->challengers[WORLDBOSS_LAST_ATTACK_INDEX];
		return 0;
	}
	//add by aaron 20121221
	unsigned now =time(0);
	if(pdata->fullBlood == pdata->blood)
	{

		pdata->standts = 0;
		pdata->ts = now;

	}else{
		pdata->standts = now - pdata->ts;
	}
	//add by aaron 20121220
	if (pdata->blood > damage)
	{
		pdata->blood -= damage;
	}
	else
	{
		damage = pdata->blood;
		pdata->blood = 0;
	}
	int pos = 0;
	for (; pos < (int)pdata->challNum; pos++)
	{
		if ((pdata->challengers[pos]).uid == uid)
			break;
	}
	memset(&self, 0, sizeof(WorldBossChallenger));
	if (pos < (int)pdata->challNum || pdata->challNum < WORLDBOSS_ATTACKERS_MAX)
	{
		unsigned newDamage = damage;
		if (pos < (int)pdata->challNum)
			 newDamage += (pdata->challengers[pos]).damage;
		int newpos = pos - 1;
		for (; newpos >= 0; newpos--)
		{
			if ((pdata->challengers[newpos]).damage >= newDamage)
				break;
		}
		newpos++;
		for (int i = pos - 1; i >= newpos; i--)
		{
			pdata->challengers[i+1] = pdata->challengers[i];
		}
		(pdata->challengers[newpos]).uid = uid;
		(pdata->challengers[newpos]).damage = newDamage;
		snprintf((pdata->challengers[newpos]).name, sizeof((pdata->challengers[newpos]).name), "%s", name.c_str());
		if (pos >= (int)pdata->challNum)
		{
			pdata->challNum++;
			if(pdata-> challNum >= 10)
			{
				int r = rand() % 2;
				switch(r)
				{
				case 0:
					pdata->showNum += 1;
					break;
				case 1:
					pdata->showNum += 3;
					break;
				default:
					pdata->showNum += 2;
					break;
				}
			}else{
				pdata->showNum = pdata->challNum;
			}

		}
		memcpy(&self, &(pdata->challengers[newpos]), sizeof(WorldBossChallenger));
		selfRank = newpos + 1;
		if (pdata->blood == 0)
		{
			pdata->challengers[WORLDBOSS_LAST_ATTACK_INDEX] = pdata->challengers[newpos];	//最后一击
		}
	}
	else
	{
		if (pdata->blood == 0)
		{
			(pdata->challengers[WORLDBOSS_LAST_ATTACK_INDEX]).uid = uid;
			(pdata->challengers[WORLDBOSS_LAST_ATTACK_INDEX]).damage = damage;
			snprintf((pdata->challengers[WORLDBOSS_LAST_ATTACK_INDEX]).name,
					sizeof((pdata->challengers[WORLDBOSS_LAST_ATTACK_INDEX]).name), "%s", name.c_str());
		}
	}
	blood = pdata->blood;

//	if(pdata->challNum > 10 && pdata->standts > 600)
//	{
//		if(pdata->showNum < 1000)
//		{
//			pdata->showNum += 1;
//		}else{
//			pdata->showNum = 1000;
//		}
//	}else{
//		pdata->showNum = pdata->challNum;
//	}
	number = pdata->showNum;
	if (pdata->blood == 0) dying = true;

	unsigned size = pdata->challNum >= 10?10 : pdata->challNum;
	for (unsigned i = 0; i < size; i++)
	{
		top.push_back((pdata->challengers)[i]);
	}
	last = pdata->challengers[WORLDBOSS_LAST_ATTACK_INDEX];
	if (pdata->blood == 0 && pdata->challNum > 10)
	{
		for (unsigned r = 20; r <= pdata->challNum; r += 10)
		{
			lucks.push_back((pdata->challengers)[r-1]);
		}
	}
	if (pdata->blood == 0)
	{
		for (unsigned j = 0; j < pdata->challNum; j++)
		{
			if((pdata->challengers)[j].damage * 100 >= pdata->fullBlood)
				dam.push_back((pdata->challengers)[j]);
		}
	}
	return 0;
}

int CDataWorldBoss::ViewWorldBoss(unsigned uid, unsigned bossId, unsigned &blood,unsigned &fullBlood, unsigned &number, int &selfRank, WorldBossChallenger &self,
		vector<WorldBossChallenger> &top, WorldBossChallenger &last)
{
	if (!IsValidWorldBossId(bossId))
		return R_ERR_NO_DATA;
	int index = bossId - WORLD_BOSS_ID_MIN;
	WorldBossData *pdata = (WorldBossData *)m_sh[index].GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh[index]), true);
	selfRank = 0;
	blood = pdata->blood;
	//number = pdata->challNum;
	number = pdata->showNum;
	fullBlood = pdata->fullBlood;
	memset(&self, 0, sizeof(WorldBossChallenger));
	for (unsigned i = 0; i < pdata->challNum; i++)
	{
		if ((pdata->challengers[i]).uid == uid)
		{
			selfRank = i + 1;
			memcpy(&self, &(pdata->challengers[i]), sizeof(WorldBossChallenger));
			break;
		}
	}
	unsigned size = pdata->challNum >= 10?10 : pdata->challNum;
	for (unsigned i = 0; i < size; i++)
	{
		top.push_back((pdata->challengers)[i]);
	}
	last = pdata->challengers[WORLDBOSS_LAST_ATTACK_INDEX];
	return 0;
}
int CDataWorldBoss::ViewWorldBoss( unsigned bossId, unsigned &blood, unsigned &fullblood, unsigned &standts, unsigned ts)
{
	if (!IsValidWorldBossId(bossId))
		return R_ERR_NO_DATA;
	int index = bossId - WORLD_BOSS_ID_MIN;
	WorldBossData *pdata = (WorldBossData *)m_sh[index].GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh[index]), true);
	pdata->blood = blood ;
	pdata->fullBlood =fullblood;
	pdata->ts = ts;
	pdata->standts = standts;
	return 0;
}
int CDataWorldBoss::SetWorldBoss( unsigned bossId, unsigned blood, unsigned fullblood, unsigned standts, unsigned ts)
{
	if (!IsValidWorldBossId(bossId))
		return R_ERR_NO_DATA;
	int index = bossId - WORLD_BOSS_ID_MIN;
	WorldBossData *pdata = (WorldBossData *)m_sh[index].GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh[index]), true);
	blood = pdata->blood  ;
	fullblood = pdata->fullBlood;
	ts = pdata->ts ;
	standts = pdata->standts;
	return 0;
}

