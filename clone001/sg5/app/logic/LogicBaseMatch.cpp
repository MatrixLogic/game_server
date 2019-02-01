#include "LogicBaseMatch.h"

CDataBaseMatch* CLogicBaseMatch::GetCDataBaseMatch() {
	GET_MEM_DATA_SEM(CDataBaseMatch, CONFIG_BASE_MATCH_PATH, sem_basematch)

	/*static CDataBaseMatch* pBaseMatch = NULL;
	 if (!pBaseMatch)
	 {
	 pBaseMatch = new CDataBaseMatch();
	 if (pBaseMatch->Init(Config::GetValue(CONFIG_BASE_MATCH_PATH)) != 0)
	 {
	 delete pBaseMatch;
	 pBaseMatch = NULL;
	 }
	 }
	 return pBaseMatch;*/
}

CDataBaseMatch* CLogicBaseMatch::GetAllServerCDataBaseMatch() {
	GET_MEM_DATA_SEM(CDataBaseMatch, CONFIG_ALL_SERVER_BASE_MATCH_PATH,
			sem_basematchallserver)
}

int CLogicBaseMatch::GetBaseMatchInfo(unsigned aid, unsigned uid,
		Json::Value &data, bool allserver) {
	CDataBaseMatch *pmatch = NULL;
	if (allserver) {
		pmatch = GetAllServerCDataBaseMatch();
	} else {
		pmatch = GetCDataBaseMatch();
	}

	if (!pmatch) {
		DB_ERROR_RETURN_MSG("init_basematch_fail");
	}
	int stage = 0;
	BaseMatchTeam team;
	BaseMatchRegularRival rival;
	vector<BaseMatchPlayoffTeam> playoff;
	int ret = pmatch->View(aid, uid, stage, team, rival, playoff);
	if (ret != 0 || playoff.size() != 15)	//must be 15
			{
		DB_ERROR_RETURN_MSG("view_basematch_fail");
	}
	data["stage"] = stage;	//0:报名阶段，1：常规赛阶段，2：8强赛，3：4强赛，4：决赛 5：比赛结束，尚未开始报名
	data["number"] = team.number;
	data["apply"] = 0;	//0:你未报名，1：你已报名 2:报名后未抽到 3报名后抽到 4报名未成功
	if (team.aid != 0) {
		bool bApply = false;
		for (unsigned i = 0; i < team.number; i++) {
			if (team.players[i].uid == uid) {
				bApply = true;
				break;
			}
		}
		if (stage == BMS_APPLY) {
			if (bApply)
				data["apply"] = 1;
		} else {
			if (team.number < 7) {
				data["apply"] = 4;
			} else {
				if (rival.player.uid != 0) {
					data["apply"] = 3;
				} else if (bApply) {
					data["apply"] = 2;
				}
			}
		}
	}
	data["status"] = 0;	//0：报名阶段 1：报名失败 2:进预赛 3；进8强 4：进4强 5：进决赛 6：亚军 7：冠军
	if (team.aid != 0) {
		if (stage != BMS_APPLY) {
			int status = team.number < 7 ? 1 : 2;
			if (stage >= BMS_PLAYOFF8) {
				for (unsigned i = 7; i < 15; i++) {
					if (playoff[i].aid == aid) {
						status = 3;
						break;
					}
				}
			}
			if (stage >= BMS_PLAYOFF4) {
				for (unsigned i = 3; i < 7; i++) {
					if (playoff[i].aid == aid) {
						status = 4;
						break;
					}
				}
			}
			if (stage >= BMS_PLAYOFF2) {
				for (unsigned i = 1; i < 3; i++) {
					if (playoff[i].aid == aid) {
						status = 5;
						break;
					}
				}
			}
			if (stage >= BMS_MATCH_FINISH) {
				if (playoff[0].aid == aid)
					status = 7;
				else if (status == 5)
					status = 6;
			}
			data["status"] = status;
		}
	}

	data["teams"].resize(15);
	for (unsigned i = 0; i < 15; i++) {
		data["teams"][i]["aid"] = playoff[i].aid;
		if (playoff[i].aid != 0) {
			data["teams"][i]["aname"] = playoff[i].aname;
			data["teams"][i]["flag"] = playoff[i].flag;
		}
	}
	data["fights"].resize(7);
	for (unsigned i = 0; i < 7; i++) {
		data["fights"][i]["team1"]["aid"] = playoff[2 * i + 1].aid;
		if (playoff[2 * i + 1].aid != 0) {
			data["fights"][i]["team1"]["aname"] = playoff[2 * i + 1].aname;
			unsigned score = 0;
			data["fights"][i]["team1"]["players"].resize(5);
			for (unsigned j = 0; j < 5; j++) {
				data["fights"][i]["team1"]["players"][j]["uid"] = playoff[2 * i
						+ 1].players[j].uid;
				data["fights"][i]["team1"]["players"][j]["name"] = playoff[2 * i
						+ 1].players[j].name;
				data["fights"][i]["team1"]["players"][j]["result"] =
						(int) (playoff[2 * i + 1].damages[j]);
				if (playoff[2 * i + 1].damages[j]
						> playoff[2 * i + 2].damages[j])
					score += 3;
				else if (playoff[2 * i + 1].damages[j]
						== playoff[2 * i + 2].damages[j])
					score += 1;
			}
			data["fights"][i]["team1"]["score"] = score;
		}
		data["fights"][i]["team2"]["aid"] = playoff[2 * i + 2].aid;
		if (playoff[2 * i + 2].aid != 0) {
			data["fights"][i]["team2"]["aname"] = playoff[2 * i + 2].aname;
			unsigned score = 0;
			data["fights"][i]["team2"]["players"].resize(5);
			for (unsigned j = 0; j < 5; j++) {
				data["fights"][i]["team2"]["players"][j]["uid"] = playoff[2 * i
						+ 2].players[j].uid;
				data["fights"][i]["team2"]["players"][j]["name"] = playoff[2 * i
						+ 2].players[j].name;
				data["fights"][i]["team2"]["players"][j]["result"] =
						(int) (playoff[2 * i + 2].damages[j]);
				if (playoff[2 * i + 2].damages[j]
						> playoff[2 * i + 1].damages[j])
					score += 3;
				else if (playoff[2 * i + 2].damages[j]
						== playoff[2 * i + 1].damages[j])
					score += 1;
			}
			data["fights"][i]["team2"]["score"] = score;
		}
	}

	data["rivals"].resize(6);
	for (unsigned i = 0; i < 6; i++) {
		data["rivals"][i]["order"] = (i + 1);
		data["rivals"][i]["ruid"] = 0;
	}
	if (rival.player.uid != 0) {
		unsigned index = 0;
		for (; index < 3; index++) {
			data["rivals"][index]["ruid"] = rival.rplayers[index].instanceid;
			data["rivals"][index]["rname"] = rival.rplayers[index].name;
			data["rivals"][index]["result"] = (int) (rival.damages[index]);
		}
		for (unsigned i = 3; i < 7; i++) {
			if (playoff[2 * i + 1].aid == aid) {
				for (unsigned j = 0; j < 5; j++) {
					if (playoff[2 * i + 1].players[j].uid == uid) {
						data["rivals"][index]["ruid"] =
								playoff[2 * i + 2].players[j].instanceid;
						data["rivals"][index]["rname"] =
								playoff[2 * i + 2].players[j].name;
						data["rivals"][index]["result"] = (int) (playoff[2 * i
								+ 1].damages[j]);
						break;
					}
				}
				break;
			}
			if (playoff[2 * i + 2].aid == aid) {
				for (unsigned j = 0; j < 5; j++) {
					if (playoff[2 * i + 2].players[j].uid == uid) {
						data["rivals"][index]["ruid"] =
								playoff[2 * i + 1].players[j].instanceid;
						data["rivals"][index]["rname"] =
								playoff[2 * i + 1].players[j].name;
						data["rivals"][index]["result"] = (int) (playoff[2 * i
								+ 2].damages[j]);
						break;
					}
				}
				break;
			}
		}
		index++;
		for (unsigned i = 1; i < 3; i++) {
			if (playoff[2 * i + 1].aid == aid) {
				for (unsigned j = 0; j < 5; j++) {
					if (playoff[2 * i + 1].players[j].uid == uid) {
						data["rivals"][index]["ruid"] =
								playoff[2 * i + 2].players[j].instanceid;
						data["rivals"][index]["rname"] =
								playoff[2 * i + 2].players[j].name;
						data["rivals"][index]["result"] = (int) (playoff[2 * i
								+ 1].damages[j]);
						break;
					}
				}
				break;
			}
			if (playoff[2 * i + 2].aid == aid) {
				for (unsigned j = 0; j < 5; j++) {
					if (playoff[2 * i + 2].players[j].uid == uid) {
						data["rivals"][index]["ruid"] =
								playoff[2 * i + 1].players[j].instanceid;
						data["rivals"][index]["rname"] =
								playoff[2 * i + 1].players[j].name;
						data["rivals"][index]["result"] = (int) (playoff[2 * i
								+ 2].damages[j]);
						break;
					}
				}
				break;
			}
		}
		index++;
		if (playoff[1].aid == aid) {
			for (unsigned j = 0; j < 5; j++) {
				if (playoff[1].players[j].uid == uid) {
					data["rivals"][index]["ruid"] =
							playoff[2].players[j].instanceid;
					data["rivals"][index]["rname"] = playoff[2].players[j].name;
					data["rivals"][index]["result"] =
							(int) (playoff[1].damages[j]);
					break;
				}
			}
		}
		if (playoff[2].aid == aid) {
			for (unsigned j = 0; j < 5; j++) {
				if (playoff[2].players[j].uid == uid) {
					data["rivals"][index]["ruid"] =
							playoff[1].players[j].instanceid;
					data["rivals"][index]["rname"] = playoff[1].players[j].name;
					data["rivals"][index]["result"] =
							(int) (playoff[2].damages[j]);
					break;
				}
			}
		}
	}
	return 0;
}

int CLogicBaseMatch::Apply(unsigned uid, bool allserver) {
	DataUser user;
	CLogicUser logicUser;
	int ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	if (!IsAllianceId(user.alliance_id)) {
		LOGIC_ERROR_RETURN_MSG("not_join_alliance");
	}
	DataUserBasic userBasic;
	CLogicUserBasic logicUserBasic;
	ret = logicUserBasic.GetUserBasicLimit(uid, OpenPlatform::GetType(),
			userBasic);
	if (ret != 0)
		return ret;
	DataAlliance alliance;
	CLogicAlliance logicAlliance;
	ret = logicAlliance.GetAllianceLimit(user.alliance_id, alliance);
	if (ret != 0)
		return ret;
	Json::Value json;
	CLogicInstance logicInst;
	ret = logicInst.GetMinInstance(uid, json);
	if (ret != 0)
		return ret;
	json["name"] = userBasic.name;
	json["pic"] = userBasic.figure_url;
	//CDataBaseMatch *pbasematch = GetCDataBaseMatch();
	CDataBaseMatch *pbasematch = NULL;
	if (allserver) {
		pbasematch = GetAllServerCDataBaseMatch();
	} else {
		pbasematch = GetCDataBaseMatch();
	}
	if (!pbasematch) {
		DB_ERROR_RETURN_MSG("init_basematch_fail");
	}
	unsigned instanceId = 0;
	if (allserver) {
		int serverid = (int) (user.alliance_id - ALLIANCE_ID_START) / 500000
				+ 1;
		string allianceName;
		String::Format(allianceName, "%d区_%s", serverid, alliance.name.c_str());
		ret = pbasematch->Apply(user.alliance_id, allianceName, alliance.flag,
				uid, userBasic.name, instanceId, true);
		if (ret != 0 || !IsValidAllServerBMatchInstId(instanceId)) {
			DB_ERROR_RETURN_MSG("basematch_apply_fail");
		}
	}
	else
	{
		ret = pbasematch->Apply(user.alliance_id, alliance.name, alliance.flag, uid, userBasic.name, instanceId);
		if (ret != 0 || !IsValidBMatchInstId(instanceId))
		{
			DB_ERROR_RETURN_MSG("basematch_apply_fail");
		}
	}
	json["userid"] = instanceId;
	CLogicMatchInstance logicMatchInstance;
	if (allserver) {
		ret = logicMatchInstance.SetAllServerMatchInstance(instanceId, json);
	} else {
		ret = logicMatchInstance.SetLocalMatchInstance(instanceId, json);
	}
	if (0 != ret) {
		return ret;
	}
	return 0;
}

int CLogicBaseMatch::ReportResult(unsigned uid, int order, int damage,
		bool allserver) {
	if (order < 1 || order > 6 || damage < 0 || damage > 100) {
		LOGIC_ERROR_RETURN_MSG("param_error");
	}
	DataUser user;
	CLogicUser logicUser;
	int ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
	return ret;
	CDataBaseMatch *pmatch = NULL;
	if (allserver)
	{
		pmatch = GetAllServerCDataBaseMatch();
	}
	else
	{
		pmatch = GetCDataBaseMatch();
	}
	if (!pmatch)
	{
		DB_ERROR_RETURN_MSG("init_basematch_fail");
	}
	ret = pmatch->ReportResult(uid, order, damage);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("basematch_report_fail");
	}
	if (IsAllianceId(user.alliance_id))
	{
		int stage = BMS_APPLY;
		pmatch->GetStage(stage);
		int point = 0;
		if (stage == BMS_REGULAR)
		{
			point *= 50;
		}
		else
		{
			point *= 20;
		}
		if (0 != point)
		{
			CLogicAlliance logicAlliance;
			logicAlliance.AddPoint(user.alliance_id, uid, point);
		}
	}
	return 0;
}

bool CLogicBaseMatch::IsBaseMatchProtect(unsigned aid, unsigned uid,
		bool allserver) {
	CDataBaseMatch *pmatch = NULL;
	if (allserver) {
		pmatch = GetAllServerCDataBaseMatch();
	} else {
		pmatch = GetCDataBaseMatch();
	}
	if (!pmatch)
		return false;
	bool prot = false;
	pmatch->IsBaseMatchProtect(aid, uid, prot);
	return prot;
}

int CLogicBaseMatch::GetApplyPlayers(unsigned aid, Json::Value &result,
		bool allserver) {
	if (!IsAllianceId(aid)) {
		PARAM_ERROR_RETURN_MSG("alliance_id_error");
	}
	CDataBaseMatch *pmatch = NULL;
	if (allserver)
	{
		pmatch = GetAllServerCDataBaseMatch();
	}
	else
	{
		pmatch = GetCDataBaseMatch();
	}
	if (!pmatch)
	{
		DB_ERROR_RETURN_MSG("init_basematch_fail");
	}
	vector<BaseMatchPlayer> players;
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

int CLogicBaseMatch::GetRegularScore(unsigned aid, Json::Value &result,
		bool allserver) {
	if (!IsAllianceId(aid)) {
		PARAM_ERROR_RETURN_MSG("alliance_id_error");
	}
	//CDataBaseMatch *pmatch = GetCDataBaseMatch();
	CDataBaseMatch *pmatch = NULL;
	if (allserver)
	{
		pmatch = GetAllServerCDataBaseMatch();
	}
	else
	{
		pmatch = GetCDataBaseMatch();
	}
	if (!pmatch)
	{
		DB_ERROR_RETURN_MSG("init_basematch_fail");
	}
	vector<BaseMatchRegularRival> players;
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
			result["players"][i]["rivals"][j]["result"] = (int)(players[i].damages[j]);
		}
	}
	return 0;
}

int CLogicBaseMatch::Load(unsigned instid, unsigned uidBy, Json::Value &result,
		bool allserver) {
	int ret = 0;
	CLogicMatchInstance logicMatchInstance;
	if (allserver) {
		ret = logicMatchInstance.GetAllServerMatchInstance(instid, result);
	} else {
		ret = logicMatchInstance.GetLocalMatchInstance(instid, result);
	}
	if (ret != 0) {
		error_log("[GetInstance fail][uid=%u,instid=%u]", uidBy, instid);
		DB_ERROR_RETURN_MSG("get_matchinst_fail");
	}
	result["userid"] = instid;
	return 0;
}

int CLogicBaseMatch::Save(unsigned instid, DataUser &userBy,
		Json::Value &data, Json::Value &result, bool allserver) {
	int ret = 0;
	if (Json::IsObject(data, "attackinfo")) {
		CLogicArchive logicArchive;
		ret = logicArchive.ProcessAttackInfo(userBy.uid, data["attackinfo"]);
		if (ret != 0)
			return ret;
	}
	return 0;
}

int CLogicBaseMatch::GetStage(int& stage, bool allserver) {
	//CDataBaseMatch *pmatch = GetCDataBaseMatch();
	CDataBaseMatch *pmatch = NULL;
	if (allserver) {
		pmatch = GetAllServerCDataBaseMatch();
	} else {
		pmatch = GetCDataBaseMatch();
	}
	if (!pmatch) {
		DB_ERROR_RETURN_MSG("init_basematch_fail");
	}

	return pmatch->GetStage(stage);
}

int CLogicBaseMatch::RequestBaseMatch(const string &url, Json::Value &data) {
	string serverUrl;
	if (!Config::GetValue(serverUrl, CONFIG_ALLS_MATCH_SERVER_PATH)) {
		error_log("[get config fail][path=]");
		return R_ERR_DATA;
	}
	//serverUrl.append("?");
	string posturl=serverUrl;
	//serverUrl += url;
	string response;
/*	if (!Network::HttpGetRequest(response, serverUrl)) {
		error_log("[HttpGetRequest fail][url=%s]", serverUrl.c_str());
		return R_ERR_REFUSE;
	}
*/
//	if (!Json::FromString(data, response))
	{
//		error_log("[parse response error!][response=%s]", response.c_str());
//		response.clear();
		if (!Network::HttpPostRequest(response,posturl,url))
		{
			error_log("[HttpPostRequest fail][url=%s,data=%s]",posturl.c_str(), url.c_str());
		}
		if(!Json::FromString(data,response))
		{
			error_log("[parse post response error!][response=%s,url=%s,data=%s]",response.c_str(),posturl.c_str(), url.c_str());
			return R_ERR_DATA;
		}
	}

	string errorCode;
	if (Json::GetString(data, "error", errorCode) && "0" != errorCode) {
		error_log("[Request error][errorCode=%s,url=%s,data=%s]", errorCode.c_str(),posturl.c_str(), url.c_str());
		return R_ERR_DATA;
	}
	return 0;
}

bool CLogicBaseMatch::IsAllServerBaseMatch(void) {
	time_t now;
	time(&now);
	int tempNow = now;
	struct tm *pTm = localtime(&now);
	if (pTm->tm_wday == 2) {
		return true;
	}
	return false;
}
