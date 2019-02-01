#include "LogicMatch.h"

CDataMatch* CLogicMatch::GetCDataMatch()
{
	GET_MEM_DATA_SEM(CDataMatch, CONFIG_MATCH_DAT_PATH, sem_match)
	/*static CDataMatch* pMatch = NULL;
	if (!pMatch)
	{
		pMatch = new CDataMatch();
		if (pMatch->Init(Config::GetValue(CONFIG_MATCH_DAT_PATH)) != 0)
		{
			delete pMatch;
			pMatch = NULL;
		}
	}
	return pMatch;*/
}

int CLogicMatch::GetMatchInfo(unsigned aid, unsigned uid, Json::Value &data)
{
	CDataMatch *pmatch = GetCDataMatch();
	if (!pmatch)
	{
		DB_ERROR_RETURN_MSG("init_match_fail");
	}
	int stage = 0;
	MatchTeam team;
	MatchRegularRival rival;
	vector<MatchPlayoffTeam> playoff;
	int ret = pmatch->View(aid, uid, stage, team, rival, playoff);
	if (ret != 0 || playoff.size() != 15)	//must be 15
	{
		DB_ERROR_RETURN_MSG("view_match_fail");
	}
	data["stage"] = stage;	//0:报名阶段，1：常规赛阶段，2：8强赛，3：4强赛，4：决赛 5：比赛结束，尚未开始报名
	data["number"] = team.number;
	data["apply"] = 0;	//0:你未报名，1：你已报名 2:报名后未抽到 3报名后抽到 4报名未成功
	if (team.aid != 0)
	{
		bool bApply = false;
		for (unsigned i = 0; i < team.number; i++)
		{
			if (team.players[i].uid == uid)
			{
				bApply = true;
				break;
			}
		}
		if (stage == MS_APPLY)
		{
			if (bApply)
				data["apply"] = 1;
		}
		else
		{
			if (team.number < 7)
			{
				data["apply"] = 4;
			}
			else
			{
				if (rival.player.uid != 0)
				{
					data["apply"] = 3;
				}
				else if (bApply)
				{
					data["apply"] = 2;
				}
			}
		}
	}
	data["status"] = 0;	//0：报名阶段 1：报名失败 2:进预赛 3；进8强 4：进4强 5：进决赛 6：亚军 7：冠军
	if (team.aid != 0)
	{
		if (stage != MS_APPLY)
		{
			int status = team.number < 7?1:2;
			if (stage >= MS_PLAYOFF8)
			{
				for (unsigned i = 7; i < 15; i++)
				{
					if (playoff[i].aid == aid)
					{
						status = 3;
						break;
					}
				}
			}
			if (stage >= MS_PLAYOFF4)
			{
				for (unsigned i = 3; i < 7; i++)
				{
					if (playoff[i].aid == aid)
					{
						status = 4;
						break;
					}
				}
			}
			if (stage >= MS_PLAYOFF2)
			{
				for (unsigned i = 1; i < 3; i++)
				{
					if (playoff[i].aid == aid)
					{
						status = 5;
						break;
					}
				}
			}
			if (stage >= MS_MATCH_FINISH)
			{
				if (playoff[0].aid == aid)
					status = 7;
				else if (status == 5)
					status = 6;
			}
			data["status"] = status;
		}
	}

	data["teams"].resize(15);
	for (unsigned i = 0; i < 15; i++)
	{
		data["teams"][i]["aid"] = playoff[i].aid;
		if (playoff[i].aid != 0)
		{
			data["teams"][i]["aname"] = playoff[i].aname;
			data["teams"][i]["flag"] = playoff[i].flag;
			data["teams"][i]["score"] = (int)(playoff[i].score);
		}
	}
	data["fights"].resize(7);
	for (unsigned i = 0; i < 7; i++)
	{
		data["fights"][i]["team1"]["aid"] = playoff[2*i+1].aid;
		if (playoff[2*i+1].aid != 0)
		{
			data["fights"][i]["team1"]["aname"] = playoff[2*i+1].aname;
			data["fights"][i]["team1"]["score"] = playoff[2*i+1].score;
			data["fights"][i]["team1"]["players"].resize(5);
			for (unsigned j = 0; j < 5; j++)
			{
				data["fights"][i]["team1"]["players"][j]["uid"] = playoff[2*i+1].players[j].uid;
				data["fights"][i]["team1"]["players"][j]["name"] = playoff[2*i+1].players[j].name;
				data["fights"][i]["team1"]["players"][j]["result"] = (int)(playoff[2*i+1].result[j]);
			}
		}
		data["fights"][i]["team2"]["aid"] = playoff[2*i+2].aid;
		if (playoff[2*i+2].aid != 0)
		{
			data["fights"][i]["team2"]["aname"] = playoff[2*i+2].aname;
			data["fights"][i]["team2"]["score"] = playoff[2*i+2].score;
			data["fights"][i]["team2"]["players"].resize(5);
			for (unsigned j = 0; j < 5; j++)
			{
				data["fights"][i]["team2"]["players"][j]["uid"] = playoff[2*i+2].players[j].uid;
				data["fights"][i]["team2"]["players"][j]["name"] = playoff[2*i+2].players[j].name;
				data["fights"][i]["team2"]["players"][j]["result"] = (int)(playoff[2*i+2].result[j]);
			}
		}
	}

	data["rivals"].resize(6);
	for (unsigned i = 0; i < 6; i++)
	{
		data["rivals"][i]["order"] = (i+1);
		data["rivals"][i]["ruid"] = 0;
	}
	if (rival.player.uid != 0)
	{
		unsigned index = 0;
		for (; index < 3; index++)
		{
			data["rivals"][index]["ruid"] = rival.rplayers[index].uid;
			data["rivals"][index]["rname"] = rival.rplayers[index].name;
			data["rivals"][index]["result"] = (int)(rival.result[index]);
		}
		for (unsigned i = 3; i < 7; i++)
		{
			if (playoff[2*i+1].aid == aid)
			{
				for (unsigned j = 0; j < 5; j++)
				{
					if (playoff[2*i+1].players[j].uid == uid)
					{
						data["rivals"][index]["ruid"] = playoff[2*i+2].players[j].uid;
						data["rivals"][index]["rname"] = playoff[2*i+2].players[j].name;
						data["rivals"][index]["result"] = (int)(playoff[2*i+1].result[j]);
						break;
					}
				}
				break;
			}
			if (playoff[2*i+2].aid == aid)
			{
				for (unsigned j = 0; j < 5; j++)
				{
					if (playoff[2*i+2].players[j].uid == uid)
					{
						data["rivals"][index]["ruid"] = playoff[2*i+1].players[j].uid;
						data["rivals"][index]["rname"] = playoff[2*i+1].players[j].name;
						data["rivals"][index]["result"] = (int)(playoff[2*i+2].result[j]);
						break;
					}
				}
				break;
			}
		}
		index++;
		for (unsigned i = 1; i < 3; i++)
		{
			if (playoff[2*i+1].aid == aid)
			{
				for (unsigned j = 0; j < 5; j++)
				{
					if (playoff[2*i+1].players[j].uid == uid)
					{
						data["rivals"][index]["ruid"] = playoff[2*i+2].players[j].uid;
						data["rivals"][index]["rname"] = playoff[2*i+2].players[j].name;
						data["rivals"][index]["result"] = (int)(playoff[2*i+1].result[j]);
						break;
					}
				}
				break;
			}
			if (playoff[2*i+2].aid == aid)
			{
				for (unsigned j = 0; j < 5; j++)
				{
					if (playoff[2*i+2].players[j].uid == uid)
					{
						data["rivals"][index]["ruid"] = playoff[2*i+1].players[j].uid;
						data["rivals"][index]["rname"] = playoff[2*i+1].players[j].name;
						data["rivals"][index]["result"] = (int)(playoff[2*i+2].result[j]);
						break;
					}
				}
				break;
			}
		}
		index++;
		if (playoff[1].aid == aid)
		{
			for (unsigned j = 0; j < 5; j++)
			{
				if (playoff[1].players[j].uid == uid)
				{
					data["rivals"][index]["ruid"] = playoff[2].players[j].uid;
					data["rivals"][index]["rname"] = playoff[2].players[j].name;
					data["rivals"][index]["result"] = (int)(playoff[1].result[j]);
					break;
				}
			}
		}
		if (playoff[2].aid == aid)
		{
			for (unsigned j = 0; j < 5; j++)
			{
				if (playoff[2].players[j].uid == uid)
				{
					data["rivals"][index]["ruid"] = playoff[1].players[j].uid;
					data["rivals"][index]["rname"] = playoff[1].players[j].name;
					data["rivals"][index]["result"] = (int)(playoff[2].result[j]);
					break;
				}
			}
		}
	}
	return 0;
}

int CLogicMatch::Apply(unsigned uid)
{
	DataUser user;
	CLogicUser logicUser;
	int ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	if (!IsAllianceId(user.alliance_id))
	{
		LOGIC_ERROR_RETURN_MSG("not_join_alliance");
	}
	DataUserBasic userBasic;
	CLogicUserBasic logicUserBasic;
	ret = logicUserBasic.GetUserBasicLimit(uid, OpenPlatform::GetType(), userBasic);
	if (ret != 0)
		return ret;
	DataAlliance alliance;
	CLogicAlliance logicAlliance;
	ret = logicAlliance.GetAllianceLimit(user.alliance_id, alliance);
	if (ret != 0)
		return ret;
	CDataMatch *pmatch = GetCDataMatch();
	if (!pmatch)
	{
		DB_ERROR_RETURN_MSG("init_match_fail");
	}
	ret = pmatch->Apply(user.alliance_id, alliance.name, alliance.flag,uid, userBasic.name);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("match_apply_fail");
	}

	return 0;
}

int CLogicMatch::ReportResult(unsigned uid, int order, int result)
{
	if (order < 1 || order > 6 || (result != 1 && result != 3))
	{
		LOGIC_ERROR_RETURN_MSG("param_error");
	}
	DataUser user;
	CLogicUser logicUser;
	int ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	CDataMatch *pmatch = GetCDataMatch();
	if (!pmatch)
	{
		DB_ERROR_RETURN_MSG("init_match_fail");
	}
	ret = pmatch->ReportResult(uid, order, result);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("match_report_fail");
	}
	if (IsAllianceId(user.alliance_id))
	{
		CLogicAlliance logicAlliance;
		int point = 0;
		if (1 == result)
		{
			point = 20;
		}
		else
		{
			point = 50;
		}
		logicAlliance.AddPoint(user.alliance_id, uid, point);
	}
	return 0;
}

bool CLogicMatch::IsMatchProtect(unsigned aid, unsigned uid)
{
	CDataMatch *pmatch = GetCDataMatch();
	if (!pmatch)
		return false;
	bool prot = false;
	pmatch->IsMatchProtect(aid, uid, prot);
	return prot;
}

int CLogicMatch::GetApplyPlayers(unsigned aid, Json::Value &result)
{
	if (!IsAllianceId(aid))
	{
		PARAM_ERROR_RETURN_MSG("alliance_id_error");
	}
	CDataMatch *pmatch = GetCDataMatch();
	if (!pmatch)
	{
		DB_ERROR_RETURN_MSG("init_match_fail");
	}
	vector<MatchPlayer> players;
	int ret = pmatch->GetApplyPlayersOfTeam(aid, players);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("get_apply_users_fail");
	}
	result["players"].resize(players.size());
	for (unsigned i = 0; i < players.size(); i++)
	{
		result["players"][i]["uid"] = players[i].uid;
		result["players"][i]["name"] = players[i].name;
	}
	return 0;
}

int CLogicMatch::GetRegularScore(unsigned aid, Json::Value &result)
{
	if (!IsAllianceId(aid))
	{
		PARAM_ERROR_RETURN_MSG("alliance_id_error");
	}
	CDataMatch *pmatch = GetCDataMatch();
	if (!pmatch)
	{
		DB_ERROR_RETURN_MSG("init_match_fail");
	}
	vector<MatchRegularRival> players;
	unsigned score;
	unsigned rank;
	int ret = pmatch->GetRegularScore(aid, score, rank, players);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("get_regular_score_fail");
	}
	result["rank"] = rank;
	result["score"] = score;
	result["players"].resize(players.size());
	for (unsigned i = 0; i < players.size(); i++)
	{
		result["players"][i]["uid"] = players[i].player.uid;
		result["players"][i]["name"] = players[i].player.name;
		result["players"][i]["rivals"].resize(3);
		for (unsigned j = 0; j < 3; j++)
		{
			result["players"][i]["rivals"][j]["uid"] = players[i].rplayers[j].uid;
			result["players"][i]["rivals"][j]["name"] = players[i].rplayers[j].name;
			result["players"][i]["rivals"][j]["result"] = (int)(players[i].result[j]);
		}
	}
	return 0;
}

int CLogicMatch::GetStage(int& stage){
	CDataMatch *pmatch = GetCDataMatch();
	if (!pmatch)
	{
		DB_ERROR_RETURN_MSG("init_match_fail");
	}

	return pmatch->GetStage(stage);
}
