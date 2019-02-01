#include "DataLadder.h"

CDataLadder::CDataLadder()
{
	m_init = false;
}

int CDataLadder::Init(const string &path, semdat sem)
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
	if(!m_sh.CreateOrOpen(path.c_str(), sizeof(LadderData), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init_ladder_data_fail][path=%s]", path.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{
		LadderData *pdata = (LadderData *)m_sh.GetAddress();
		memset(pdata, 0, sizeof(*pdata));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}

int CDataLadder::GetRank(unsigned uid, int &rank)
{
	LadderData *pdata = (LadderData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	rank = -1;
	for (unsigned upos = 0; upos < LADDER_ITEM_NUMBER; upos++)
	{
		if (pdata->ladder[upos].uid == uid)
		{
			rank = upos + 1;
			break;
		}
	}
	return 0;
}

int CDataLadder::GetFivePlayers(int rank1, int rank2, int rank3, int rank4, int rank5,
		LadderItem &player1, LadderItem &player2, LadderItem &player3, LadderItem &player4, LadderItem &player5)
{
	LadderData *pdata = (LadderData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	memset(&player1, 0, sizeof(LadderItem));
	memset(&player2, 0, sizeof(LadderItem));
	memset(&player3, 0, sizeof(LadderItem));
	memset(&player4, 0, sizeof(LadderItem));
	memset(&player5, 0, sizeof(LadderItem));
	if (IsValidLadderRank(rank1))
		player1 = pdata->ladder[LadderRankToIndex(rank1)];
	if (IsValidLadderRank(rank2))
		player2 = pdata->ladder[LadderRankToIndex(rank2)];
	if (IsValidLadderRank(rank3))
		player3 = pdata->ladder[LadderRankToIndex(rank3)];
	if (IsValidLadderRank(rank4))
		player4 = pdata->ladder[LadderRankToIndex(rank4)];
	if (IsValidLadderRank(rank5))
		player5 = pdata->ladder[LadderRankToIndex(rank5)];
	return 0;
}

int CDataLadder::Win(int &rank1, unsigned uid1, int level1, const string &name1, const string &pic1,
		int &rank2, unsigned uid2, int level2, const string &name2, const string &pic2,
		unsigned upts, LadderError &error)
{
	error = LE_SUCCESS;
	LadderData *pdata = (LadderData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error = LE_SYS_ERROR;
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	if ((rank1 != -1 && !IsValidLadderRank(rank1))
			|| rank2 != -1 && !IsValidLadderRank(rank2))
	{
		error = LE_PARAM_ERROR;
		return R_ERR_DB;
	}
	if (!IsValidLadderRank(rank2))
	{
		error = LE_DEFENCER_OUT_OF_RANK;
		return R_ERR_DB;
	}
	if (rank1 == rank2)
	{
		error = LE_CHALL_SELF;
		return R_ERR_DB;
	}
	if (rank1 != -1 && rank1 < rank2)
	{
		error = LE_CHALL_BACK_RANK;
		return R_ERR_DB;
	}
	if (rank1 == -1)
	{
		if (pdata->ladder[rank2 - 1].uid != uid2)
		{
			error = LE_DEFENCER_MISTACK;
			return R_ERR_DB;
		}
		pdata->ladder[rank2 - 1].uid = uid1;
		pdata->ladder[rank2 - 1].level = level1;
		pdata->ladder[rank2 - 1].upts = upts;
		snprintf(pdata->ladder[rank2 - 1].name, sizeof(pdata->ladder[rank2 - 1].name), "%s", name1.c_str());
		snprintf(pdata->ladder[rank2 - 1].pic, sizeof(pdata->ladder[rank2 - 1].pic), "%s", pic1.c_str());
		rank1 = rank2;
		rank2 = -1;
	}
	else
	{
		if (pdata->ladder[rank2 - 1].uid != uid2)
		{
			error = LE_DEFENCER_MISTACK;
			return R_ERR_DB;
		}
		if (pdata->ladder[rank1 - 1].uid != uid1)
		{
			error = LE_ATTACKER_MISTACK;
			return R_ERR_DB;
		}
		pdata->ladder[rank2 - 1].uid = uid1;
		pdata->ladder[rank2 - 1].level = level1;
		pdata->ladder[rank2 - 1].upts = upts;
		snprintf(pdata->ladder[rank2 - 1].name, sizeof(pdata->ladder[rank2 - 1].name), "%s", name1.c_str());
		snprintf(pdata->ladder[rank2 - 1].pic, sizeof(pdata->ladder[rank2 - 1].pic), "%s", pic1.c_str());

		pdata->ladder[rank1 - 1].uid = uid2;
		pdata->ladder[rank1 - 1].level = level2;
		pdata->ladder[rank1 - 1].upts = upts;
		snprintf(pdata->ladder[rank1 - 1].name, sizeof(pdata->ladder[rank1 - 1].name), "%s", name2.c_str());
		snprintf(pdata->ladder[rank1 - 1].pic, sizeof(pdata->ladder[rank1 - 1].pic), "%s", pic2.c_str());

		int temp = rank1;
		rank1 = rank2;
		rank2 = temp;
	}
	return 0;
}

int CDataLadder::GetTopOne(LadderItem &data)
{
	LadderData *pdata = (LadderData *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		error_log("[GetAddress fail][]");
		return R_ERR_DB;
	}
	memcpy(&data,&(pdata->ladder[0]),sizeof(LadderItem));
	return 0;
}
