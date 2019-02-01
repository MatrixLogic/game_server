#include "DataAllianceBoss.h"


#define ALLIANCEBOSS_ATTACKERS_MAX (ALLIANCEBOSS_CHALLENGERS_MAX-1)
#define ALLIANCEBOSS_LAST_ATTACK_INDEX (ALLIANCEBOSS_CHALLENGERS_MAX-1)
#define STAND_TIME_LEVEL_1 1
#define STAND_TIME_LEVEL_2 2
#define STAND_TIME_LEVEL_3 3
#define STAND_TIME_LEVEL_4 4
#define STAND_TIME_LEVEL_5 5
#define STAND_TIME_LEVEL_6 6
CDataAllianceBoss::CDataAllianceBoss()
{
	m_init = false;
}

CDataAllianceBoss::~CDataAllianceBoss()
{
	// TODO Auto-generated destructor stub
}
int CDataAllianceBoss::Init(const string &path, const vector<int> &vMinBloods,const vector<int> &vMaxBlood, semdat sem)
{
	if(m_init)
	{
		return 0;
	}
	if (path.empty())
	{
		error_log("[init_Allianceboss_fail][path=%s", path.c_str());
		return R_ERROR;
	}
	int semgroup = 0;
	int semserver = 0;
	Config::GetDomain(semserver);
	if(!semserver)
		Config::GetDB(semserver);               //获取server id

	int bossId = ALLIANCE_BOSS_ID;
	string mapfile = path;
	if (path[path.length() - 1] != '/')
		mapfile.append("/");
	mapfile.append("Alliance").append(".boss");
	m_maxWorldbossBlood[0] = vMaxBlood[0];
	m_minWorldbossBlood[0] = vMinBloods[0];

	if(!m_sh.CreateOrOpen(mapfile.c_str(), sizeof(AllianceAttackNumber), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init_worldboss_fail2][path=%s]", mapfile.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&(m_sh), true);
	if(!m_sh.HasInit())         //初始化共享内存
	{
		AllianceAttackNumber *pdata = (AllianceAttackNumber *)m_sh.GetAddress();
		memset(pdata, 0, sizeof(AllianceAttackNumber));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}

//开启屠魔的人
int CDataAllianceBoss::LoadAllianceBossOpener(unsigned uid, unsigned bossId, unsigned &blood,
						unsigned &fullBlood,unsigned &vip_grade,unsigned &alliance_id,unsigned &ts)
{
	//add by jimmy 2013
	if (!IsValidAllianceBossId(bossId))
	return R_ERR_NO_DATA;
	unsigned now = Time::GetGlobalTime();
	unsigned ts1 = now;
	unsigned totalStandTs = 0;
	unsigned arvStandTs = 0;
	int fullBloodLevel = 1;
	int index =0;
	bool flag = false;

	AllianceAttackNumber *pdata = (AllianceAttackNumber *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh), true);
	for(int i = 0; i < ALLIANCEBOSS_ALLIANCE_MAX; i++)
	{
		if((pdata->Alliancebossdata[i].alliance_id == alliance_id)&&(CTime::GetDayInterval(pdata->Alliancebossdata[i].ts_start, now) == 0))
		{
			return R_ERR_DATA_LIMIT;
		}
		else if(((pdata->Alliancebossdata[i].alliance_id == alliance_id)
	    &&(CTime::GetDayInterval(pdata->Alliancebossdata[i].ts_start, now) != 0)))
		{
			index = i;
			flag = true;
			debug_log("allianceboss_already_opener : | last_blood=%u | curr_open=%u | alliance_id=%u | %d"
								,pdata->Alliancebossdata[i].fullBlood,now,pdata->Alliancebossdata[i].alliance_id,i);
			break;
		}
	    else if( (pdata->Alliancebossdata[i].alliance_id == 0))
	    {
			index = i;
			flag = true;
			debug_log("allianceboss_first_opener : | last_blood=%u | curr_open=%u | alliance_id=%u | %d"
								,pdata->Alliancebossdata[i].fullBlood,now,pdata->Alliancebossdata[i].alliance_id,i);
			break;
	    }
	}

	if(flag == true )
	{
		CDataAlliancePay alliance_coins;                                   //开启活动需减帮会30金币
		DataAlliancePay alliancecoin;
		int ret = alliance_coins.GetPay(alliance_id,alliancecoin);
		if(ret != 0 )
		{
			return ret;
		}
		if(alliancecoin.coins >= 30)
		{
			alliancecoin.coins = alliancecoin.coins - 30;
			ret = alliance_coins.SetPay(alliance_id,alliancecoin);
			if(ret != 0)
			{
				return ret;
			}
		}
		else
		{
			DB_ERROR_RETURN_MSG("Alliance_coin_lacking");
//			return R_ERR_REFUSE;
		}

		pdata->Alliancebossdata[index].alliance_id = alliance_id;
		pdata->Alliancebossdata[index].ts_start = now;
		pdata->Alliancebossdata[index].vip_grade = vip_grade;

		if (CTime::GetDayInterval(pdata->Alliancebossdata[index].ts,now) != 0)
		{
			totalStandTs +=pdata->Alliancebossdata[index].standts;
			ts1= pdata->Alliancebossdata[index].ts;
		}
		if(CTime::GetDayInterval(ts1, now) != 0)
		{
			arvStandTs = totalStandTs / 4;
			if(arvStandTs < 180 && arvStandTs > 0)
			{
				fullBloodLevel = STAND_TIME_LEVEL_1;
			}
			else if(arvStandTs >= 180 && arvStandTs < 300)
			{
				fullBloodLevel = STAND_TIME_LEVEL_2;
			}
			else
			{
				fullBloodLevel = STAND_TIME_LEVEL_3;
			}
			switch(fullBloodLevel)
			{
			case STAND_TIME_LEVEL_1:
				pdata->Alliancebossdata[index].fullBlood = pdata->Alliancebossdata[index].fullBlood * 2;
				break;
			case STAND_TIME_LEVEL_2:
				pdata->Alliancebossdata[index].fullBlood =pdata->Alliancebossdata[index].fullBlood * 1.5;
				break;
			case STAND_TIME_LEVEL_3:
				pdata->Alliancebossdata[index].fullBlood = pdata->Alliancebossdata[index].fullBlood * 0.8;
				break;
			default:
				break;
			}
			if(pdata->Alliancebossdata[index].fullBlood < m_minWorldbossBlood[0] )
			{
				pdata->Alliancebossdata[index].fullBlood = m_minWorldbossBlood[0];
			}
			else if(pdata->Alliancebossdata[index].fullBlood > m_maxWorldbossBlood[0] )
			{
				pdata->Alliancebossdata[index].fullBlood = m_maxWorldbossBlood[0];
			}
			pdata->Alliancebossdata[index].blood = pdata->Alliancebossdata[index].fullBlood;
			memset(&(pdata->Alliancebossdata[index].challengers), 0, sizeof(pdata->Alliancebossdata[0].challengers));
			pdata->Alliancebossdata[index].challNum = 0;
			pdata->Alliancebossdata[index].showNum = 0;
			pdata->Alliancebossdata[index].ts = now;
			pdata->Alliancebossdata[index].standts = 0;

			fullBlood = pdata->Alliancebossdata[index].fullBlood;
			blood = pdata->Alliancebossdata[index].blood;
			debug_log("Allianceboss_blood :fullBlood=%u |  alliance_id=%u | %d"
					,pdata->Alliancebossdata[index].fullBlood,pdata->Alliancebossdata[index].alliance_id,index);
		}
	}
	else                             //空间已经满了
	{
		debug_log("opener : %s  |open_uid= %u ","no place to allianceboss opener!!",uid);
		return R_ERR_DATA_LIMIT;
	}
	return 0;
}

int CDataAllianceBoss::LoadAllianceBoss(unsigned uid, unsigned bossId, unsigned &blood, unsigned &number, int &selfRank, AllianceBossChallenger &self,
		vector<AllianceBossChallenger> &top, AllianceBossChallenger &last,unsigned &fullBlood,unsigned &alliance_id)
{

	if (!IsValidAllianceBossId(bossId))
	return R_ERR_NO_DATA;
	int index = 999;
	bool flag = false;
	unsigned now = Time::GetGlobalTime();
	AllianceAttackNumber *pdata = (AllianceAttackNumber *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[Get AllianceBoss Address fail]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh), true);
	for(int i = 0; i < ALLIANCEBOSS_ALLIANCE_MAX; i++)
	{
		if((pdata->Alliancebossdata[i].alliance_id == alliance_id)&&(CTime::GetDayInterval(pdata->Alliancebossdata[i].ts_start, now) == 0))
		{
			debug_log("jimmyloadsuccess : ts_start=%u |alliance_id=%u  | blood=%u | full_blood=%u",
					pdata->Alliancebossdata[i].ts_start,pdata->Alliancebossdata[i].alliance_id,pdata->Alliancebossdata[i].blood,pdata->Alliancebossdata[i].fullBlood);
			index = i;
			flag = true;
			break;
		}
	}

	if(flag == true)
	{
		debug_log("jimmyload_flag=ture : ts_start=%u |alliance_id=%u  | blood=%u | full_blood=%u",
				pdata->Alliancebossdata[index].ts_start,pdata->Alliancebossdata[index].alliance_id
				,pdata->Alliancebossdata[index].blood,pdata->Alliancebossdata[index].fullBlood);
		selfRank = 0;
		fullBlood = pdata->Alliancebossdata[index].fullBlood;
		blood = pdata->Alliancebossdata[index].blood;
		number = pdata->Alliancebossdata[index].showNum;
		memset(&self, 0, sizeof(AllianceBossChallenger));
		for (unsigned i = 0; i < pdata->Alliancebossdata[index].challNum; i++)
		{
			if ((pdata->Alliancebossdata[index].challengers[i]).uid == uid)
			{
				selfRank = i + 1;
				memcpy(&self, &(pdata->Alliancebossdata[index].challengers[i]), sizeof(AllianceBossChallenger));
				break;
			}
		}
		unsigned size = pdata->Alliancebossdata[index].challNum;

		for (unsigned i = 0; i < size; i++)
		{
			top.push_back((pdata->Alliancebossdata[index].challengers)[i]);
		}
		last = pdata->Alliancebossdata[index].challengers[ALLIANCEBOSS_LAST_ATTACK_INDEX];
	}
	else
	{
		selfRank = 0;
		fullBlood = 0;
		blood = 0;
		number = 0;
		memset(&self, 0, sizeof(AllianceBossChallenger));
		memset(&last, 0, sizeof(AllianceBossChallenger));
		top.clear();
		debug_log("load_allianceboss  uid =%u",uid);
//		return R_ERR_NO_DATA;
	}
	return 0;
}

int CDataAllianceBoss::AttackAllianceBoss(unsigned uid, unsigned bossId, unsigned damage, const string &name,
			bool &dying, unsigned &blood, unsigned &number, int &selfRank, AllianceBossChallenger &self,
			vector<AllianceBossChallenger> &top, AllianceBossChallenger &last, vector<AllianceBossChallenger> &lucks
			, vector<AllianceBossChallenger> &dam,unsigned &alliance_id,unsigned &vip)
{
	if (!IsValidAllianceBossId(bossId))
		return R_ERR_NO_DATA;
	AllianceAttackNumber *pdata = NULL;
	unsigned now = time(0);
	pdata = (AllianceAttackNumber *)m_sh.GetAddress();
	int index;
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh), true);
	int i = 0;
	for(; i < ALLIANCEBOSS_ALLIANCE_MAX; i++)
	{
		if((pdata->Alliancebossdata[i].alliance_id == alliance_id))
		{
			index = i;
			break;
		}
	}
	if(i == ALLIANCEBOSS_ALLIANCE_MAX)
		return R_ERR_LOGIC;

	dying = false;
	selfRank = 0;

	vip = pdata->Alliancebossdata[index].vip_grade;

	if (pdata->Alliancebossdata[index].blood == 0 || damage == 0)
	{
		blood = pdata->Alliancebossdata[index].blood;
		//number = pdata->challNum;
		number = pdata->Alliancebossdata[index].showNum;
		memset(&self, 0, sizeof(AllianceBossChallenger));
		for (unsigned i = 0; i < pdata->Alliancebossdata[index].challNum; i++)
		{
			if ((pdata->Alliancebossdata[index].challengers[i]).uid == uid)
			{
				selfRank = i + 1;
				memcpy(&self, &(pdata->Alliancebossdata[index].challengers[i]), sizeof(AllianceBossChallenger));
				break;
			}
		}
		unsigned size = pdata->Alliancebossdata[index].challNum>= 10?10 : pdata->Alliancebossdata[index].challNum;
		for (unsigned i = 0; i < size; i++)
		{
			top.push_back((pdata->Alliancebossdata[index].challengers)[i]);
		}
		last = pdata->Alliancebossdata[index].challengers[ALLIANCEBOSS_LAST_ATTACK_INDEX];
		return 0;
	}

	//add by aaron 20121221
	if(pdata->Alliancebossdata[index].fullBlood == pdata->Alliancebossdata[index].blood)
	{

		pdata->Alliancebossdata[index].standts = 0;
		pdata->Alliancebossdata[index].ts = now;

	}else{
		pdata->Alliancebossdata[index].standts = now - pdata->Alliancebossdata[index].ts;
	}
	//add by aaron 20121220
	if (pdata->Alliancebossdata[index].blood > damage)
	{
		pdata->Alliancebossdata[index].blood -= damage;
	}
	else
	{
		damage = pdata->Alliancebossdata[index].blood;
		pdata->Alliancebossdata[index].blood = 0;
	}
	int pos = 0;
	for (; pos < (int)pdata->Alliancebossdata[index].challNum; pos++)
	{
		if ((pdata->Alliancebossdata[index].challengers[pos]).uid == uid)
			break;
	}
	memset(&self, 0, sizeof(AllianceBossChallenger));
	if (pos < (int)pdata->Alliancebossdata[index].challNum || pdata->Alliancebossdata[index].challNum < ALLIANCEBOSS_ATTACKERS_MAX)
	{
		unsigned newDamage = damage;
		if (pos < (int)pdata->Alliancebossdata[index].challNum)
			 newDamage += (pdata->Alliancebossdata[index].challengers[pos]).damage;
		int newpos = pos - 1;
		for (; newpos >= 0; newpos--)
		{
			if ((pdata->Alliancebossdata[index].challengers[newpos]).damage >= newDamage)
				break;
		}
		newpos++;
		for (int i = pos - 1; i >= newpos; i--)
		{
			pdata->Alliancebossdata[index].challengers[i+1] = pdata->Alliancebossdata[index].challengers[i];
		}
		(pdata->Alliancebossdata[index].challengers[newpos]).uid = uid;
		(pdata->Alliancebossdata[index].challengers[newpos]).damage = newDamage;
		snprintf((pdata->Alliancebossdata[index].challengers[newpos]).name, sizeof((pdata->Alliancebossdata[index].challengers[newpos]).name), "%s", name.c_str());
		if (pos >= (int)pdata->Alliancebossdata[index].challNum)
		{
			pdata->Alliancebossdata[index].challNum++;
			if(pdata->Alliancebossdata[index].challNum >= 10)
			{
				int r = rand() % 2;
				switch(r)
				{
				case 0:
					pdata->Alliancebossdata[index].showNum += 1;
					break;
				case 1:
					pdata->Alliancebossdata[index].showNum += 3;
					break;
				default:
					pdata->Alliancebossdata[index].showNum += 2;
					break;
				}
			}else{
				pdata->Alliancebossdata[index].showNum = pdata->Alliancebossdata[index].challNum;
			}

		}
		memcpy(&self, &(pdata->Alliancebossdata[index].challengers[newpos]), sizeof(AllianceBossChallenger));
		selfRank = newpos + 1;
		if (pdata->Alliancebossdata[index].blood == 0)
		{
			pdata->Alliancebossdata[index].challengers[ALLIANCEBOSS_LAST_ATTACK_INDEX] = pdata->Alliancebossdata[index].challengers[newpos];	//最后一击
		}
	}
	else
	{
		if (pdata->Alliancebossdata[index].blood == 0)
		{
			(pdata->Alliancebossdata[index].challengers[ALLIANCEBOSS_LAST_ATTACK_INDEX]).uid = uid;
			(pdata->Alliancebossdata[index].challengers[ALLIANCEBOSS_LAST_ATTACK_INDEX]).damage = damage;
			snprintf((pdata->Alliancebossdata[index].challengers[ALLIANCEBOSS_LAST_ATTACK_INDEX]).name,
					sizeof((pdata->Alliancebossdata[index].challengers[ALLIANCEBOSS_LAST_ATTACK_INDEX]).name), "%s", name.c_str());
		}
	}
	blood = pdata->Alliancebossdata[index].blood;

	number = pdata->Alliancebossdata[index].showNum;
	if (pdata->Alliancebossdata[index].blood == 0)
	{
		dying = true;
	}

	unsigned size = pdata->Alliancebossdata[index].challNum >= 10?10 : pdata->Alliancebossdata[index].challNum;
	for (unsigned i = 0; i < size; i++)
	{
		top.push_back((pdata->Alliancebossdata[index].challengers)[i]);
	}
	last = pdata->Alliancebossdata[index].challengers[ALLIANCEBOSS_LAST_ATTACK_INDEX];
	if (pdata->Alliancebossdata[index].blood == 0 && pdata->Alliancebossdata[index].challNum > 10)
	{
		for (unsigned r = 20; r <= pdata->Alliancebossdata[index].challNum; r += 10)
		{
			lucks.push_back((pdata->Alliancebossdata[index].challengers)[r-1]);
		}
	}
	if (pdata->Alliancebossdata[index].blood == 0)
	{
		for (unsigned j = 0; j < pdata->Alliancebossdata[index].challNum; j++)
		{
			if((pdata->Alliancebossdata[index].challengers)[j].damage * 100 >= pdata->Alliancebossdata[index].fullBlood)
				dam.push_back((pdata->Alliancebossdata[index].challengers)[j]);
		}
	}
	return 0;
}

int CDataAllianceBoss::ViewAllianceBoss(unsigned uid, unsigned bossId, unsigned &blood,unsigned &fullBlood, unsigned &number, int &selfRank, AllianceBossChallenger &self,
		vector<AllianceBossChallenger> &top, AllianceBossChallenger &last,unsigned &alliance_id)
{
	AllianceAttackNumber *pdata = NULL;
	unsigned now = time(0);
	pdata = (AllianceAttackNumber *)m_sh.GetAddress();
	int index;
	bool flag = false;
	for(int i = 0; i < ALLIANCEBOSS_ALLIANCE_MAX; i++)
	{
		if((pdata->Alliancebossdata[i].alliance_id == alliance_id))
		{
			index = i;
			flag = true;
			break;
		}
	}
	if(pdata == NULL)
	{
		error_log("[GetAddress_fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh), true);
	if(flag == true)
	{
		selfRank = 0;
		blood = pdata->Alliancebossdata[index].blood;
		number = pdata->Alliancebossdata[index].showNum;
		fullBlood = pdata->Alliancebossdata[index].fullBlood;
		memset(&self, 0, sizeof(AllianceBossChallenger));
		for (unsigned i = 0; i < pdata->Alliancebossdata[index].challNum; i++)
		{
			if ((pdata->Alliancebossdata[index].challengers[i]).uid == uid)
			{
				selfRank = i + 1;
				memcpy(&self, &(pdata->Alliancebossdata[index].challengers[i]), sizeof(AllianceBossChallenger));
				break;
			}
		}
		unsigned size = pdata->Alliancebossdata[index].challNum;
		for (unsigned i = 0; i < size; i++)
		{
			top.push_back((pdata->Alliancebossdata[index].challengers)[i]);
		}
		last = pdata->Alliancebossdata[index].challengers[ALLIANCEBOSS_LAST_ATTACK_INDEX];
	}
	else
	{
		selfRank = 0;
		fullBlood = 0;
		blood = 0;
		number = 0;
		memset(&self, 0, sizeof(AllianceBossChallenger));
		memset(&last, 0, sizeof(AllianceBossChallenger));
		top.clear();

	}
	return 0;
}


int CDataAllianceBoss::LoadLastAllianceBoss(unsigned uid, unsigned bossId, unsigned &blood, unsigned &number, int &selfRank, AllianceBossChallenger &self,
		vector<AllianceBossChallenger> &top, AllianceBossChallenger &last,unsigned &fullBlood,unsigned &alliance_id)
{
	//add by aaron 20121220
	if (!IsValidAllianceBossId(bossId))
	return R_ERR_NO_DATA;

	AllianceAttackNumber *pdata = (AllianceAttackNumber *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail]");
		return R_ERR_DB;
	}
	CAutoLock lock(&(m_sh), true);

	int index;
	bool flag = false;
	unsigned now = time(0);
	for(int i = 0; i < ALLIANCEBOSS_ALLIANCE_MAX; i++)
	{
		if(((pdata->Alliancebossdata[i].alliance_id == alliance_id)&&(pdata->Alliancebossdata[i].blood == 0))
		|| (((pdata->Alliancebossdata[i].alliance_id == alliance_id))&&(now - pdata->Alliancebossdata[i].ts_start > 60*60)))
		{
			index = i;
			flag = true;
			debug_log("jimmy_last %u | %u",pdata->Alliancebossdata[index].ts_start,now);
			break;
		}
	}
	//add by aaron 20121220
	if(flag == true)
	{
		selfRank = 0;
		fullBlood = pdata->Alliancebossdata[index].fullBlood;
		blood = pdata->Alliancebossdata[index].blood;
		//number = pdata->challNum;
		number = pdata->Alliancebossdata[index].showNum;
		memset(&self, 0, sizeof(AllianceBossChallenger));
		for (unsigned i = 0; i < pdata->Alliancebossdata[index].challNum; i++)
		{
			if ((pdata->Alliancebossdata[index].challengers[i]).uid == uid)
			{
				selfRank = i + 1;
				memcpy(&self, &(pdata->Alliancebossdata[index].challengers[i]), sizeof(AllianceBossChallenger));
				break;
			}
		}
		unsigned size = pdata->Alliancebossdata[index].challNum;

		for (unsigned i = 0; i < size; i++)
		{
			top.push_back((pdata->Alliancebossdata[index].challengers)[i]);
		}
		last = pdata->Alliancebossdata[index].challengers[ALLIANCEBOSS_LAST_ATTACK_INDEX];
	}
	else                             //空间已经满了
	{
		selfRank = 0;
		fullBlood = 0;
		blood = 0;
		number = 0;
		memset(&self, 0, sizeof(AllianceBossChallenger));
		memset(&last, 0, sizeof(AllianceBossChallenger));
		top.clear();
	}
	return 0;
}
