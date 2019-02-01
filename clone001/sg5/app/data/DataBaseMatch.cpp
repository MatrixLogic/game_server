#include "DataInc.h"

CDataBaseMatch::CDataBaseMatch()
{
	m_init = false;
}

int CDataBaseMatch::Init(const string &path, semdat sem)
{
	if(m_init)
	{
		return 0;
	}
	if (path.empty())
	{
		error_log("[init_basematch_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	int semgroup = 0;
	int semserver = 0;
	Config::GetDomain(semserver);
	if(!semserver)
		Config::GetDB(semserver);
	if(!m_sh.CreateOrOpen(path.c_str(), sizeof(BaseMatchData), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init_basematch_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{
		BaseMatchData *pData = (BaseMatchData *)m_sh.GetAddress();
		memset(pData, 0, sizeof(*pData));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}

int CDataBaseMatch::View(unsigned aid, unsigned uid, int &stage, BaseMatchTeam &team, BaseMatchRegularRival &rival,
		vector<BaseMatchPlayoffTeam> &playoff)
{
	BaseMatchData *pdata = (BaseMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	stage = pdata->stage;
	playoff.push_back(pdata->champion);
	playoff.push_back(pdata->top2[0]);
	playoff.push_back(pdata->top2[1]);
	playoff.push_back(pdata->top4[0]);
	playoff.push_back(pdata->top4[1]);
	playoff.push_back(pdata->top4[2]);
	playoff.push_back(pdata->top4[3]);
	playoff.push_back(pdata->top8[0]);
	playoff.push_back(pdata->top8[1]);
	playoff.push_back(pdata->top8[2]);
	playoff.push_back(pdata->top8[3]);
	playoff.push_back(pdata->top8[4]);
	playoff.push_back(pdata->top8[5]);
	playoff.push_back(pdata->top8[6]);
	playoff.push_back(pdata->top8[7]);

	memset(&team, 0, sizeof(BaseMatchTeam));
	for (unsigned i = 0; i < pdata->numOfApply; i++)
	{
		if ((pdata->teams[i]).aid == aid)
		{
			team = pdata->teams[i];
			break;
		}
	}

	//常规赛的对手
	memset(&rival, 0, sizeof(BaseMatchRegularRival));
	for (unsigned i = 0; i < pdata->numOfReg; i++)
	{
		for (unsigned j = 0; j < 5; j++)
		{
			if ((pdata->rteams[i]).rivals[j].player.uid == uid)
			{
				rival = (pdata->rteams[i]).rivals[j];
				return 0;
			}
		}
	}
	return 0;
}

int CDataBaseMatch::Apply(unsigned aid, const string &aname,
		int flag, unsigned uid, const string &name, unsigned &instanceid,bool allserver)
{
	unsigned min_match_instance_id = BMATCH_INST_ID_MIN;
	if (allserver)
	{
		min_match_instance_id = ALL_SERVER_BMATCH_INST_ID_MIN;
	}
	BaseMatchData *pdata = (BaseMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	if (pdata->stage != BMS_APPLY)
		return R_ERR_LOGIC;
	if (allserver)
	{
		if (!IsValidAllServerBMatchInstId(pdata->instance_ndx + min_match_instance_id))
			return R_ERR_LOGIC;
	}
	else
	{
		if (!IsValidBMatchInstId(pdata->instance_ndx + min_match_instance_id))
			return R_ERR_LOGIC;
	}
	unsigned apos = 0;
	for (; apos < pdata->numOfApply; apos++)
	{
		if ((pdata->teams[apos]).aid == aid)
			break;
	}
	if (apos < pdata->numOfApply)
	{
		unsigned upos = 0;
		for (; upos < (pdata->teams[apos]).number; upos++)
		{
			if ((pdata->teams[apos]).players[upos].uid == uid)
				break;
		}
		if (upos < (pdata->teams[apos]).number)
			return R_ERR_LOGIC;
		if ((pdata->teams[apos]).number >= BASEMATCH_PLAYERS_MAX)
			return R_ERR_LOGIC;
		instanceid = pdata->instance_ndx++ + min_match_instance_id;
		(pdata->teams[apos]).players[upos].uid = uid;
		snprintf((pdata->teams[apos]).players[upos].name, sizeof((pdata->teams[apos]).players[upos].name),
				"%s", name.c_str());
		(pdata->teams[apos]).players[upos].instanceid = instanceid;
		(pdata->teams[apos]).number++;
	}
	else
	{
		if (pdata->numOfApply >= BASEMATCH_TEAMS_MAX)
			return R_ERR_LOGIC;
		(pdata->teams[pdata->numOfApply]).aid = aid;
		snprintf((pdata->teams[pdata->numOfApply]).aname, sizeof((pdata->teams[pdata->numOfApply]).aname),
				"%s", aname.c_str());
		(pdata->teams[pdata->numOfApply]).flag = flag;
		(pdata->teams[pdata->numOfApply]).number = 1;
		instanceid = pdata->instance_ndx++ + min_match_instance_id;
		(pdata->teams[pdata->numOfApply]).players[0].uid = uid;
		snprintf((pdata->teams[pdata->numOfApply]).players[0].name, sizeof((pdata->teams[pdata->numOfApply]).players[0].name),
				"%s", name.c_str());
		(pdata->teams[pdata->numOfApply]).players[0].instanceid = instanceid;
		pdata->numOfApply++;
	}
	return 0;
}

int CDataBaseMatch::ReportResult(unsigned uid, int order, int damage)
{
	if (order < 1 || order > 6 || damage < 0 || damage > 100)
		return R_ERR_LOGIC;
	BaseMatchData *pdata = (BaseMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	if (pdata->stage == BMS_REGULAR && order >= 1 && order <= 3)
	{
		for (unsigned i = 0; i < pdata->numOfReg; i++)
		{
			for (unsigned j = 0; j < 5; j++)
			{
				if ((pdata->rteams[i]).rivals[j].player.uid == uid)
				{
					if ((pdata->rteams[i]).rivals[j].damages[order-1] == 0)
					{
						(pdata->rteams[i]).rivals[j].damages[order-1] = damage;
						(pdata->rteams[i]).rivals[j].damage += damage;
						(pdata->rteams[i]).damage += damage;
					}
					return 0;
				}
			}
		}
	}
	else if (pdata->stage == BMS_PLAYOFF8 && order == 4)
	{
		for (unsigned i = 0; i < 8; i++)
		{
			for (unsigned j = 0; j < 5; j++)
			{
				if ((pdata->top8[i]).players[j].uid == uid)
				{
					if ((pdata->top8[i]).damages[j] == 0)
					{
						(pdata->top8[i]).damages[j] = damage;
						(pdata->top8[i]).damage += damage;
					}
					return 0;
				}
			}
		}
	}
	else if (pdata->stage == BMS_PLAYOFF4 && order == 5)
	{
		for (unsigned i = 0; i < 4; i++)
		{
			for (unsigned j = 0; j < 5; j++)
			{
				if ((pdata->top4[i]).players[j].uid == uid)
				{
					if ((pdata->top4[i]).damages[j] == 0)
					{
						(pdata->top4[i]).damages[j] = damage;
						(pdata->top4[i]).damage += damage;
					}
					return 0;
				}
			}
		}
	}
	else if (pdata->stage == BMS_PLAYOFF2 && order == 6)
	{
		for (unsigned i = 0; i < 2; i++)
		{
			for (unsigned j = 0; j < 5; j++)
			{
				if ((pdata->top2[i]).players[j].uid == uid)
				{
					if ((pdata->top2[i]).damages[j] == 0)
					{
						(pdata->top2[i]).damages[j] = damage;
						(pdata->top2[i]).damage += damage;
					}
					return 0;
				}
			}
		}
	}
	return 0;
}

int CDataBaseMatch::StartRegular()
{
	BaseMatchData *pdata = (BaseMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	if (pdata->stage != BMS_APPLY)
		return R_ERR_LOGIC;
	pdata->stage = BMS_REGULAR;
	pdata->numOfReg = 0;
	memset(pdata->rteams, 0, sizeof(pdata->rteams));
	memset(pdata->top8, 0, sizeof(pdata->top8));
	memset(pdata->top4, 0, sizeof(pdata->top4));
	memset(pdata->top2, 0, sizeof(pdata->top2));
	memset(&(pdata->champion), 0, sizeof(pdata->champion));
	bool mark[60];
	for (unsigned i = 0; i < pdata->numOfApply; i++)
	{
		if ((pdata->teams[i]).number < 7)
			continue;
		memset(mark, 0, sizeof(mark));
		int rest = (pdata->teams[i]).number > 60 ? 60 : (pdata->teams[i]).number;
        for (int k = 0; k < 5; k++,rest--)
        {
			int r = ((unsigned)rand()) % rest + 1;
			int j = 0;
			while (r > 0 && j < 60)
			{
				if (mark[j++]) continue;
				r--;
			}
			mark[j - 1] = true;
        }
        int ndx = 0;
		for (unsigned k = 0; k < 60 && ndx < 5; k++)
		{
			if (k >= (pdata->teams[i]).number) break;
			if (!mark[k]) continue;
			(pdata->rteams[pdata->numOfReg]).rivals[ndx++].player = (pdata->teams[i]).players[k];
		}
		if (ndx != 5) continue;
		(pdata->rteams[pdata->numOfReg]).aid = (pdata->teams[i]).aid;
		strcpy((pdata->rteams[pdata->numOfReg]).aname, (pdata->teams[i]).aname);
		(pdata->rteams[pdata->numOfReg]).flag = (pdata->teams[i]).flag;
		pdata->numOfReg++;
	}
	if (pdata->numOfReg == 2)
	{
		for (int i = 0; i < 5; i++)
		{
			(pdata->rteams[0]).rivals[i].rplayers[0] = (pdata->rteams[1]).rivals[i].player;
			(pdata->rteams[0]).rivals[i].rplayers[1] = (pdata->rteams[1]).rivals[(i+1)%5].player;
			(pdata->rteams[0]).rivals[i].rplayers[2] = (pdata->rteams[1]).rivals[(i+2)%5].player;
			(pdata->rteams[1]).rivals[i].rplayers[0] = (pdata->rteams[0]).rivals[i].player;
			(pdata->rteams[1]).rivals[i].rplayers[1] = (pdata->rteams[0]).rivals[(i+1)%5].player;
			(pdata->rteams[1]).rivals[i].rplayers[2] = (pdata->rteams[0]).rivals[(i+2)%5].player;
		}
	}
	else if (pdata->numOfReg == 3)
	{
		for (int i = 0; i < 5; i++)
		{
			(pdata->rteams[0]).rivals[i].rplayers[0] = (pdata->rteams[1]).rivals[i].player;
			(pdata->rteams[0]).rivals[i].rplayers[1] = (pdata->rteams[1]).rivals[(i+1)%5].player;
			(pdata->rteams[0]).rivals[i].rplayers[2] = (pdata->rteams[2]).rivals[i].player;
			(pdata->rteams[1]).rivals[i].rplayers[0] = (pdata->rteams[2]).rivals[i].player;
			(pdata->rteams[1]).rivals[i].rplayers[1] = (pdata->rteams[2]).rivals[(i+1)%5].player;
			(pdata->rteams[1]).rivals[i].rplayers[2] = (pdata->rteams[0]).rivals[i].player;
			(pdata->rteams[2]).rivals[i].rplayers[0] = (pdata->rteams[0]).rivals[i].player;
			(pdata->rteams[2]).rivals[i].rplayers[1] = (pdata->rteams[0]).rivals[(i+1)%5].player;
			(pdata->rteams[2]).rivals[i].rplayers[2] = (pdata->rteams[1]).rivals[i].player;
		}
	}
	else if (pdata->numOfReg > 3)
	{
		int rnum = pdata->numOfReg;
		int r, an, r2;
		for (unsigned i = 0; i < pdata->numOfReg; i++)
		{
			for (unsigned j = 0; j < 5; j++)
			{
				r = ((unsigned)rand()) % (rnum - 1) + 1;
				r2 = ((unsigned)rand()) % 2;
				an = (i + r) % rnum;
				(pdata->rteams[i]).rivals[j].rplayers[0] = (pdata->rteams[an]).rivals[r2].player;
				r = ((unsigned)rand()) % (rnum - 1) + 1;
				r2 = ((unsigned)rand()) % 2 + 1;
				an = (i + r) % rnum;
				(pdata->rteams[i]).rivals[j].rplayers[1] = (pdata->rteams[an]).rivals[r2].player;
				r = ((unsigned)rand()) % (rnum - 1) + 1;
				r2 = ((unsigned)rand()) % 2 + 2;
				an = (i + r) % rnum;
				(pdata->rteams[i]).rivals[j].rplayers[2] = (pdata->rteams[an]).rivals[r2].player;
			}
		}
	}
	return 0;
}

int CDataBaseMatch::StartGuess(bool allserver)
{
	BaseMatchData *pdata = (BaseMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	if (pdata->stage != BMS_REGULAR)
		return R_ERR_LOGIC;
	pdata->stage = BMS_GUESS;
	int rank[8];
	for (int i = 0; i < 8; i++)
		rank[i] = -1;
	for (unsigned i = 0; i < pdata->numOfReg; i++)
	{
		int inpos = 7;
		for (; inpos >= 0; inpos--)
		{
			if (rank[inpos] < 0)
				continue;
			if ((pdata->rteams[i]).damage <= (pdata->rteams[rank[inpos]]).damage)
				break;
		}
		inpos++;
		if (inpos < 8)
		{
			for (int j = 7; j > inpos; j--)
				rank[j] = rank[j-1];
			rank[inpos] = i;
		}
	}
	int itemp = rank[0];
	if (itemp != -1)
	{
		(pdata->top8[0]).aid = (pdata->rteams[itemp]).aid;
		strcpy((pdata->top8[0]).aname, (pdata->rteams[itemp]).aname);
		(pdata->top8[0]).flag = (pdata->rteams[itemp]).flag;
		for (int i = 0; i < 5; i++)
		{
			(pdata->top8[0]).players[i] = (pdata->rteams[itemp]).rivals[i].player;
		}
	}
	itemp = rank[7];
	if (itemp != -1)
	{
		(pdata->top8[1]).aid = (pdata->rteams[itemp]).aid;
		strcpy((pdata->top8[1]).aname, (pdata->rteams[itemp]).aname);
		(pdata->top8[1]).flag = (pdata->rteams[itemp]).flag;
		for (int i = 0; i < 5; i++)
		{
			(pdata->top8[1]).players[i] = (pdata->rteams[itemp]).rivals[i].player;
		}
	}
	itemp = rank[3];
	if (itemp != -1)
	{
		(pdata->top8[2]).aid = (pdata->rteams[itemp]).aid;
		strcpy((pdata->top8[2]).aname, (pdata->rteams[itemp]).aname);
		(pdata->top8[2]).flag = (pdata->rteams[itemp]).flag;
		for (int i = 0; i < 5; i++)
		{
			(pdata->top8[2]).players[i] = (pdata->rteams[itemp]).rivals[i].player;
		}
	}
	itemp = rank[4];
	if (itemp != -1)
	{
		(pdata->top8[3]).aid = (pdata->rteams[itemp]).aid;
		strcpy((pdata->top8[3]).aname, (pdata->rteams[itemp]).aname);
		(pdata->top8[3]).flag = (pdata->rteams[itemp]).flag;
		for (int i = 0; i < 5; i++)
		{
			(pdata->top8[3]).players[i] = (pdata->rteams[itemp]).rivals[i].player;
		}
	}
	itemp = rank[1];
	if (itemp != -1)
	{
		(pdata->top8[4]).aid = (pdata->rteams[itemp]).aid;
		strcpy((pdata->top8[4]).aname, (pdata->rteams[itemp]).aname);
		(pdata->top8[4]).flag = (pdata->rteams[itemp]).flag;
		for (int i = 0; i < 5; i++)
		{
			(pdata->top8[4]).players[i] = (pdata->rteams[itemp]).rivals[i].player;
		}
	}
	itemp = rank[6];
	if (itemp != -1)
	{
		(pdata->top8[5]).aid = (pdata->rteams[itemp]).aid;
		strcpy((pdata->top8[5]).aname, (pdata->rteams[itemp]).aname);
		(pdata->top8[5]).flag = (pdata->rteams[itemp]).flag;
		for (int i = 0; i < 5; i++)
		{
			(pdata->top8[5]).players[i] = (pdata->rteams[itemp]).rivals[i].player;
		}
	}
	itemp = rank[2];
	if (itemp != -1)
	{
		(pdata->top8[6]).aid = (pdata->rteams[itemp]).aid;
		strcpy((pdata->top8[6]).aname, (pdata->rteams[itemp]).aname);
		(pdata->top8[6]).flag = (pdata->rteams[itemp]).flag;
		for (int i = 0; i < 5; i++)
		{
			(pdata->top8[6]).players[i] = (pdata->rteams[itemp]).rivals[i].player;
		}
	}
	itemp = rank[5];
	if (itemp != -1)
	{
		(pdata->top8[7]).aid = (pdata->rteams[itemp]).aid;
		strcpy((pdata->top8[7]).aname, (pdata->rteams[itemp]).aname);
		(pdata->top8[7]).flag = (pdata->rteams[itemp]).flag;
		for (int i = 0; i < 5; i++)
		{
			(pdata->top8[7]).players[i] = (pdata->rteams[itemp]).rivals[i].player;
		}
	}

	CGuessData guessData;
	int ret = 0;
	if (allserver)
	{
		ret = guessData.Init(Config::GetValue(ALL_SERVER_GUESS_DATA_PATH),1,sem_guessallserver);
	}
	else
	{
		ret = guessData.Init(Config::GetValue(GUESS_DATA_PATH),1,sem_guess);
	}
	if(ret != 0)
	{
		error_log("guess data init fail");
		return R_ERR_LOGIC;
	}
	GuessViewData tmpData;
	vector<GuessViewData> dataVec;
	for(int i = 0; i < 8; ++i){
		tmpData.gid = pdata->top8[i].aid;
		snprintf(tmpData.gname,sizeof(tmpData.gname),"%s",pdata->top8[i].aname);
		dataVec.push_back(tmpData);
	}
	guessData.SetInitValues(dataVec);

	return 0;
}

int CDataBaseMatch::StartPlayoff8()
{
	BaseMatchData *pdata = (BaseMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	if (pdata->stage != BMS_GUESS)
		return R_ERR_LOGIC;
	pdata->stage = BMS_PLAYOFF8;

	return 0;

	int rank[8];
	for (int i = 0; i < 8; i++)
		rank[i] = -1;
	for (unsigned i = 0; i < pdata->numOfReg; i++)
	{
		int inpos = 7;
		for (; inpos >= 0; inpos--)
		{
			if (rank[inpos] < 0)
				continue;
			if ((pdata->rteams[i]).damage <= (pdata->rteams[rank[inpos]]).damage)
				break;
		}
		inpos++;
		if (inpos < 8)
		{
			for (int j = 7; j > inpos; j--)
				rank[j] = rank[j-1];
			rank[inpos] = i;
		}
	}
	int itemp = rank[0];
	if (itemp != -1)
	{
		(pdata->top8[0]).aid = (pdata->rteams[itemp]).aid;
		strcpy((pdata->top8[0]).aname, (pdata->rteams[itemp]).aname);
		(pdata->top8[0]).flag = (pdata->rteams[itemp]).flag;
		for (int i = 0; i < 5; i++)
		{
			(pdata->top8[0]).players[i] = (pdata->rteams[itemp]).rivals[i].player;
		}
	}
	itemp = rank[7];
	if (itemp != -1)
	{
		(pdata->top8[1]).aid = (pdata->rteams[itemp]).aid;
		strcpy((pdata->top8[1]).aname, (pdata->rteams[itemp]).aname);
		(pdata->top8[1]).flag = (pdata->rteams[itemp]).flag;
		for (int i = 0; i < 5; i++)
		{
			(pdata->top8[1]).players[i] = (pdata->rteams[itemp]).rivals[i].player;
		}
	}
	itemp = rank[3];
	if (itemp != -1)
	{
		(pdata->top8[2]).aid = (pdata->rteams[itemp]).aid;
		strcpy((pdata->top8[2]).aname, (pdata->rteams[itemp]).aname);
		(pdata->top8[2]).flag = (pdata->rteams[itemp]).flag;
		for (int i = 0; i < 5; i++)
		{
			(pdata->top8[2]).players[i] = (pdata->rteams[itemp]).rivals[i].player;
		}
	}
	itemp = rank[4];
	if (itemp != -1)
	{
		(pdata->top8[3]).aid = (pdata->rteams[itemp]).aid;
		strcpy((pdata->top8[3]).aname, (pdata->rteams[itemp]).aname);
		(pdata->top8[3]).flag = (pdata->rteams[itemp]).flag;
		for (int i = 0; i < 5; i++)
		{
			(pdata->top8[3]).players[i] = (pdata->rteams[itemp]).rivals[i].player;
		}
	}
	itemp = rank[1];
	if (itemp != -1)
	{
		(pdata->top8[4]).aid = (pdata->rteams[itemp]).aid;
		strcpy((pdata->top8[4]).aname, (pdata->rteams[itemp]).aname);
		(pdata->top8[4]).flag = (pdata->rteams[itemp]).flag;
		for (int i = 0; i < 5; i++)
		{
			(pdata->top8[4]).players[i] = (pdata->rteams[itemp]).rivals[i].player;
		}
	}
	itemp = rank[6];
	if (itemp != -1)
	{
		(pdata->top8[5]).aid = (pdata->rteams[itemp]).aid;
		strcpy((pdata->top8[5]).aname, (pdata->rteams[itemp]).aname);
		(pdata->top8[5]).flag = (pdata->rteams[itemp]).flag;
		for (int i = 0; i < 5; i++)
		{
			(pdata->top8[5]).players[i] = (pdata->rteams[itemp]).rivals[i].player;
		}
	}
	itemp = rank[2];
	if (itemp != -1)
	{
		(pdata->top8[6]).aid = (pdata->rteams[itemp]).aid;
		strcpy((pdata->top8[6]).aname, (pdata->rteams[itemp]).aname);
		(pdata->top8[6]).flag = (pdata->rteams[itemp]).flag;
		for (int i = 0; i < 5; i++)
		{
			(pdata->top8[6]).players[i] = (pdata->rteams[itemp]).rivals[i].player;
		}
	}
	itemp = rank[5];
	if (itemp != -1)
	{
		(pdata->top8[7]).aid = (pdata->rteams[itemp]).aid;
		strcpy((pdata->top8[7]).aname, (pdata->rteams[itemp]).aname);
		(pdata->top8[7]).flag = (pdata->rteams[itemp]).flag;
		for (int i = 0; i < 5; i++)
		{
			(pdata->top8[7]).players[i] = (pdata->rteams[itemp]).rivals[i].player;
		}
	}

	return 0;
}
int CDataBaseMatch::StartPlayoff4(bool allserver)
{
	BaseMatchData *pdata = (BaseMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	if (pdata->stage != BMS_PLAYOFF8)
		return R_ERR_LOGIC;
	pdata->stage = BMS_PLAYOFF4;
	for (unsigned i = 0; i < 4; i++)
	{
		unsigned score1 = 0;
		unsigned score2 = 0;
		for (unsigned j = 0; j < 5; j++)
		{
			if ((pdata->top8[2*i]).damages[j] > (pdata->top8[2*i+1]).damages[j])
			{
				score1 += 3;
			}
			else if ((pdata->top8[2*i]).damages[j] < (pdata->top8[2*i+1]).damages[j])
			{
				score2 += 3;
			}
			else
			{
				score1++;
				score2++;
			}
		}
		unsigned itemp = 0;	//晋级的队
		if (score1 > score2)
		{
			itemp = 2*i;
		}
		else if (score1 < score2)
		{
			itemp = 2*i+1;
		}
		else
		{
			CGuessData guessData;
			int ret = 0;
			if (allserver)
			{
				ret = guessData.Init(Config::GetValue(ALL_SERVER_GUESS_DATA_PATH),1,sem_guessallserver);
			}
			else
			{
				ret = guessData.Init(Config::GetValue(GUESS_DATA_PATH),1,sem_guess);
			}
			if(0 == ret)
			{
				unsigned s1=0,s2=0;
				guessData.GetSupportCount(pdata->top8[2*i].aid,s1);
				guessData.GetSupportCount(pdata->top8[2*i+1].aid,s2);
				itemp = s1 > s2 ? 2*i : 2*i+1;
				if(s1 == s2)
				{
					int temp = random() % 2;
					if(temp == 0)
					{
						if(pdata->top8[2*i].aid != 0 && pdata->top8[2*i+1].aid == 0)
						{
							itemp = 2*i;
						}
						else
						{
							itemp = 2*i+1;
						}
					}
					if(temp == 1)
					{
						if(pdata->top8[2*i+1].aid != 0 && pdata->top8[2*i].aid == 0)
						{
							itemp = 2*i+1;
						}
						else
						{
							itemp = 2*i;
						}
					}
				}
			}
			else
			{
				error_log("guess data init fail");
			}
		}
		(pdata->top4[i]).aid = (pdata->top8[itemp]).aid;
		strcpy((pdata->top4[i]).aname, (pdata->top8[itemp]).aname);
		(pdata->top4[i]).flag = (pdata->top8[itemp]).flag;
		for (unsigned j = 0; j < 5; j++)
		{
			(pdata->top4[i]).players[j] = (pdata->top8[itemp]).players[j];
		}
	}
	return 0;
}

int CDataBaseMatch::StartPlayoff2(bool allserver)
{
	BaseMatchData *pdata = (BaseMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	if (pdata->stage != BMS_PLAYOFF4)
	{
		return R_ERR_LOGIC;
	}
	pdata->stage = BMS_PLAYOFF2;
	for (unsigned i = 0; i < 2; i++)
	{
		unsigned score1 = 0;
		unsigned score2 = 0;
		for (unsigned j = 0; j < 5; j++)
		{
			if ((pdata->top4[2*i]).damages[j] > (pdata->top4[2*i+1]).damages[j])
			{
				score1 += 3;
			}
			else if ((pdata->top4[2*i]).damages[j] < (pdata->top4[2*i+1]).damages[j])
			{
				score2 += 3;
			}
			else
			{
				score1++;
				score2++;
			}
		}
		unsigned itemp = 2*i;
		itemp = score1 > score2 ? 2*i : 2*i + 1;
		if(score1 == score2)
		{
			CGuessData guessData;
			int ret = 0;
			if (allserver)
			{
				ret = guessData.Init(Config::GetValue(ALL_SERVER_GUESS_DATA_PATH),1,sem_guessallserver);
			}
			else
			{
				ret = guessData.Init(Config::GetValue(GUESS_DATA_PATH),1,sem_guess);
			}
			if(0 == ret)
			{
				unsigned s1=0,s2=0;
				guessData.GetSupportCount(pdata->top4[2*i].aid,s1);
				guessData.GetSupportCount(pdata->top4[2*i+1].aid,s2);
				itemp = s1 > s2 ? 2*i : 2*i+1;
				if(s1 == s2)
				{
					int temp = random() % 2;
					if(temp == 0)
					{
						if(pdata->top4[2*i].aid != 0 && pdata->top4[2*i+1].aid == 0)
						{
							itemp = 2*i;
						}
						else
						{
							itemp = 2*i+1;
						}
					}
					if(temp == 1)
					{
						if(pdata->top4[2*i+1].aid != 0 && pdata->top4[2*i].aid == 0)
						{
							itemp = 2*i+1;
						}
						else
						{
							itemp = 2*i;
						}
					}
				}
			}
			else
			{
				error_log("guess data init fail");
			}
		}

		(pdata->top2[i]).aid = (pdata->top4[itemp]).aid;
		strcpy((pdata->top2[i]).aname, (pdata->top4[itemp]).aname);
		(pdata->top2[i]).flag = (pdata->top4[itemp]).flag;
		for (unsigned j = 0; j < 5; j++)
		{
			(pdata->top2[i]).players[j] = (pdata->top4[itemp]).players[j];
		}
	}
	return 0;
}

int CDataBaseMatch::FinishMatch(const BaseMatchData **p,bool allserver)
{
	BaseMatchData *pdata = (BaseMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	if (pdata->stage != BMS_PLAYOFF2)
	{
		return R_ERR_LOGIC;
	}
	pdata->stage = BMS_MATCH_FINISH;
	unsigned score1 = 0;
	unsigned score2 = 0;
	for (unsigned j = 0; j < 5; j++)
	{
		if ((pdata->top2[0]).damages[j] > (pdata->top2[1]).damages[j])
		{
			score1 += 3;
		}
		else if ((pdata->top2[0]).damages[j] < (pdata->top2[1]).damages[j])
		{
			score2 += 3;
		}
		else
		{
			score1++;
			score2++;
		}
	}
	unsigned itemp = 0;
	if(score1 > score2)
	{
		itemp = 0;
	}
	else if(score1 < score2)
	{
		itemp = 1;
	}
	else
	{
		CGuessData guessData;
		int ret = 0;
		if (allserver)
		{
			ret = guessData.Init(Config::GetValue(ALL_SERVER_GUESS_DATA_PATH),1,sem_guessallserver);
		}
		else
		{
			ret = guessData.Init(Config::GetValue(GUESS_DATA_PATH),1,sem_guess);
		}
		if(ret != 0)
		{
			error_log("guess data init fail");
			return R_ERR_LOGIC;
		}
		unsigned s1=0,s2=0;
		guessData.GetSupportCount(pdata->top2[0].aid,s1);
		guessData.GetSupportCount(pdata->top2[1].aid,s2);
		itemp = s1 > s2 ? 0 : 1;
		if(s1 == s2)
		{
			int temp = random() % 2;
			if(temp == 0)
			{
				if(pdata->top2[0].aid != 0 && pdata->top2[1].aid == 0)
				{
					itemp = 0;
				}
				else
				{
					itemp = 1;
				}

			}
			if(temp == 1)
			{
				if(pdata->top2[1].aid != 0 && pdata->top2[0].aid == 0)
				{
					itemp = 1;
				}
				else
				{
					itemp = 0;
				}
			}
		}
	}

	(pdata->champion).aid = (pdata->top2[itemp]).aid;
	strcpy((pdata->champion).aname, (pdata->top2[itemp]).aname);
	(pdata->champion).flag = (pdata->top2[itemp]).flag;
	for (unsigned j = 0; j < 5; j++)
	{
		(pdata->champion).players[j] = (pdata->top2[itemp]).players[j];
	}
	*p = pdata;
	return 0;
}

int CDataBaseMatch::NextTurn()
{
	BaseMatchData *pdata = (BaseMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	pdata->instance_ndx = 0;
	pdata->stage = BMS_APPLY;
	pdata->numOfApply = 0;
	memset(pdata->teams, 0, sizeof(pdata->teams));
	return 0;
}

int CDataBaseMatch::IsBaseMatchProtect(unsigned aid, unsigned uid, bool &prot)
{
	BaseMatchData *pdata = (BaseMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	prot = false;
	if (pdata->stage < BMS_PLAYOFF8 || pdata->stage >= BMS_MATCH_FINISH)
	{
		return 0;
	}
	for (unsigned i = 0; i < 8; i++)
	{
		if ((pdata->top8[i]).aid == aid)
		{
			for (unsigned j = 0; j < 5; j++)
			{
				if ((pdata->top8[i]).players[j].uid == uid)
				{
					prot = true;
					break;
				}
			}
			break;
		}
	}
	return 0;
}

int CDataBaseMatch::GetApplyPlayersOfTeam(unsigned aid, vector<BaseMatchPlayer> &players)
{
	BaseMatchData *pdata = (BaseMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	for (unsigned i = 0; i < pdata->numOfApply; i++)
	{
		if ((pdata->teams[i]).aid == aid)
		{
			for (unsigned j = 0; j < (pdata->teams[i]).number; j++)
			{
				players.push_back((pdata->teams[i]).players[j]);
			}
			break;
		}
	}
	return 0;
}

int CDataBaseMatch::GetRegularScore(unsigned aid, unsigned &damage, unsigned &rank, vector<BaseMatchRegularRival> &players)
{
	BaseMatchData *pdata = (BaseMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	damage = 0;
	rank = 0;
	for (unsigned i = 0; i < pdata->numOfReg; i++)
	{
		if ((pdata->rteams[i]).aid == aid)
		{
			rank = 1;
			damage = (pdata->rteams[i]).damage;
			for (unsigned j = 0; j < 5; j++)
			{
				players.push_back((pdata->rteams[i]).rivals[j]);
			}
			break;
		}
	}
	if (rank == 0)
		return 0;
	for (unsigned i = 0; i < pdata->numOfReg; i++)
	{
		if ((pdata->rteams[i]).aid == aid)
			continue;
		if ((pdata->rteams[i]).damage >= damage)
			rank++;
	}
	return 0;
}

BaseMatchData * CDataBaseMatch::GetMatchData(){
	BaseMatchData *pdata = (BaseMatchData *)m_sh.GetAddress();
	return pdata;
}

int CDataBaseMatch::GetStage(int& stage){
	BaseMatchData *pdata = (BaseMatchData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);

	stage = pdata->stage;

	return 0;
}
