/*
 * LogiAllServerLadder.cpp
 *
 *  Created on: 2013-6-18
 *      Author: Administrator
 */

#include "LogicAllServerLadder.h"

CDataLadder* CLogicAllServerLadder::GetCDataLadder(int level)
{
	if (level < 1 || level > 3)
	{
		return NULL;
	}
	static map<int,CDataLadder*> s_ladder_map;
	map<int,CDataLadder*>::iterator itr = s_ladder_map.find(level);
	if (itr != s_ladder_map.end() && itr->second != NULL)
	{
		return itr->second;
	}
	CDataLadder *tempData = new CDataLadder();
	if (NULL == tempData)
	{
		error_log("[new Ladder data fail]");
		return NULL;
	}
	string dataPath = Config::GetValue(CONFIG_ALL_SERVER_LADDER_PATH);
	if (dataPath.empty())
	{
		error_log("[data path is empty]");
		return NULL;
	}
	string dataPathlevel;
	String::Format(dataPathlevel,"%s.%d",dataPath.c_str(),level);
	const semdat sem[3] = {sem_ladderallserver_1,sem_ladderallserver_2,sem_ladderallserver_3};
	int ret = tempData->Init(dataPathlevel, sem[level - 1]);
	if (0 != ret)
	{
		error_log("[init data fail][ret=%d]",ret);
		delete tempData;
		tempData = NULL;
	}
	s_ladder_map[level] = tempData;
	return tempData;
}

void CLogicAllServerLadder::CheckLadderJson(Json::Value &ladder, bool *init)
{

	if (ladder.isMember("upts"))
	{
		if (init) *init = false;
		if (ladder["winMax"].asInt() < ladder["win"].asInt())
			ladder["winMax"] = ladder["win"];
		if (!IsValidLadderRank(ladder["rankMax"].asInt()) && IsValidLadderRank(ladder["rank"].asInt()))
			ladder["rankMax"] = ladder["rank"];
		if (IsValidLadderRank(ladder["rankMax"].asInt()) && IsValidLadderRank(ladder["rank"].asInt())
				&& ladder["rankMax"].asInt() > ladder["rank"].asInt())
			ladder["rankMax"] = ladder["rank"];
		unsigned upts = ladder["upts"].asUInt();
		if (CTime::GetDayInterval(upts, Time::GetGlobalTime()) != 0)
		{
			ladder["win"] = 0;
		}
	}
	else
	{
		if (init) *init = true;
		ladder["upts"] = Time::GetGlobalTime();
		ladder["rank"] = -1;
		ladder["rankMax"] = -1;
		ladder["win"] = 0;
		ladder["winMax"] = 0;
		ladder["prs"][0u] = -1;
		ladder["prs"][1u] = -1;
		ladder["prs"][2u] = -1;
		ladder["prs"][3u] = -1;
		ladder["prs"][4u] = -1;
		ladder["log"].resize(0);
	}
}

void CLogicAllServerLadder::RandLadderPlayers(Json::Value &ladder)
{
	int rank = ladder["rank"].asInt();
	int begin = 1, num = 0;
	if (rank == -1)
	{
		begin = 901;
		num = 100;
	}
	else if (rank > 1 && rank <= 1000)
	{
		begin = rank > 100 ? rank - 100 : 1;
		num = rank - begin;
	}
	for (unsigned i = 0; i < 5; i++)
	{
		ladder["prs"][i] = -1;
	}
	if (num > 0 && num <= 100)
	{
		if (num <= 5)
		{
			int i = 0;
			for (; i < num; i++)
				ladder["prs"][i] = begin + i;
		}
		else
		{
			bool mark[100];
			memset(mark, 0, sizeof(mark));
			int rest = num;
	        for (int k = 0; k < 5; k++,rest--)
	        {
				int r = ((unsigned)rand()) % rest + 1;
				int j = 0;
				while (r > 0 && j < 100)
				{
					if (mark[j++]) continue;
					r--;
				}
				mark[j - 1] = true;
	        }
	        unsigned ndx = 0;
			for (int k = 0; k < num; k++)
			{
				if (ndx >= 5) break;
				if (!mark[k]) continue;
				ladder["prs"][ndx++] = begin + k;
			}
		}
	}
}

int CLogicAllServerLadder::ViewLadder(unsigned uid, Json::Value &result, int level)
{
	CDataLadder* pladder = GetCDataLadder(level);
	if (NULL == pladder)
	{
		DB_ERROR_RETURN_MSG("init_ladder_fail");
	}

	string ladderData;
	CDataUserData dbUserData;
	int ret = dbUserData.GetAllServerLadder(uid, ladderData);
	if (ret != 0)
		return ret;

	int rank = -1;
	ret = pladder->GetRank(uid, rank);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("get_ladder_rank_fail");
	}

	Json::Value &ladder = result["ladder"];
	Json::Reader reader;
	if (!ladderData.empty() && !reader.parse(ladderData, ladder))
	{
		DB_ERROR_RETURN_MSG("parse_ladder_fail");
	}
	bool bInit = false;
	bool needSave = false;
	CheckLadderJson(ladder,&bInit);
	if (bInit || ladder["rank"].asInt() != rank)
	{
		ladder["rank"] = rank;
		RandLadderPlayers(ladder);
		needSave = true;
		if (bInit)
		{
			ladder["rankMax"] = rank;
		}
	}
	if (needSave)
	{
		ladder["upts"] = Time::GetGlobalTime();
		Json::FastWriter writer;
		ladderData = writer.write(ladder);
		ret = dbUserData.SetAllServerLadder(uid, ladderData);
		if (ret != 0)
			return ret;
	}

	int rank1 = ladder["prs"][0u].asInt();
	int rank2 = ladder["prs"][1u].asInt();
	int rank3 = ladder["prs"][2u].asInt();
	int rank4 = ladder["prs"][3u].asInt();
	int rank5 = ladder["prs"][4u].asInt();
	LadderItem player1;
	LadderItem player2;
	LadderItem player3;
	LadderItem player4;
	LadderItem player5;
	pladder->GetFivePlayers(rank1, rank2, rank3, rank4, rank5,
			player1, player2, player3, player4, player5);

	Json::Value &players = result["players"];
	players.resize(0);
	if (IsValidLadderRank(rank1))
	{
		Json::Value json;
		json["rank"] = rank1;
		json["uid"] = player1.uid;
		json["level"] = player1.level;
		json["name"] = player1.name;
		json["pic"] = player1.pic;
		players.append(json);
	}
	if (IsValidLadderRank(rank2))
	{
		Json::Value json;
		json["rank"] = rank2;
		json["uid"] = player2.uid;
		json["level"] = player2.level;
		json["name"] = player2.name;
		json["pic"] = player2.pic;
		players.append(json);
	}
	if (IsValidLadderRank(rank3))
	{
		Json::Value json;
		json["rank"] = rank3;
		json["uid"] = player3.uid;
		json["level"] = player3.level;
		json["name"] = player3.name;
		json["pic"] = player3.pic;
		players.append(json);
	}
	if (IsValidLadderRank(rank4))
	{
		Json::Value json;
		json["rank"] = rank4;
		json["uid"] = player4.uid;
		json["level"] = player4.level;
		json["name"] = player4.name;
		json["pic"] = player4.pic;
		players.append(json);
	}
	if (IsValidLadderRank(rank5))
	{
		Json::Value json;
		json["rank"] = rank5;
		json["uid"] = player5.uid;
		json["level"] = player5.level;
		json["name"] = player5.name;
		json["pic"] = player5.pic;
		players.append(json);
	}
	return 0;
}

int CLogicAllServerLadder::Refresh(unsigned uid, Json::Value &result,int level)
{
	CDataLadder* pladder = GetCDataLadder(level);
	if (!pladder)
	{
		DB_ERROR_RETURN_MSG("init_ladder_fail");
	}

	string ladderData;
	CDataUserData dbUserData;
	int ret = dbUserData.GetAllServerLadder(uid, ladderData);
	if (ret != 0)
		return ret;

	Json::Value &ladder = result["ladder"];
	Json::Reader reader;
	if (ladderData.empty() || !reader.parse(ladderData, ladder))
	{
		DB_ERROR_RETURN_MSG("parse_ladder_fail");
	}
	CheckLadderJson(ladder);
	RandLadderPlayers(ladder);
	ladder["upts"] = Time::GetGlobalTime();

	Json::FastWriter writer;
	ladderData = writer.write(ladder);
	ret = dbUserData.SetAllServerLadder(uid,ladderData);
	if (0 != ret)
	{
		return ret;
	}

	int rank1 = ladder["prs"][0u].asInt();
	int rank2 = ladder["prs"][1u].asInt();
	int rank3 = ladder["prs"][2u].asInt();
	int rank4 = ladder["prs"][3u].asInt();
	int rank5 = ladder["prs"][4u].asInt();
	LadderItem player1;
	LadderItem player2;
	LadderItem player3;
	LadderItem player4;
	LadderItem player5;
	pladder->GetFivePlayers(rank1, rank2, rank3, rank4, rank5,
			player1, player2, player3, player4, player5);

	Json::Value &players = result["players"];
	players.resize(0);
	if (IsValidLadderRank(rank1))
	{
		Json::Value json;
		json["rank"] = rank1;
		json["uid"] = player1.uid;
		json["level"] = player1.level;
		json["name"] = player1.name;
		json["pic"] = player1.pic;
		players.append(json);
	}
	if (IsValidLadderRank(rank2))
	{
		Json::Value json;
		json["rank"] = rank2;
		json["uid"] = player2.uid;
		json["level"] = player2.level;
		json["name"] = player2.name;
		json["pic"] = player2.pic;
		players.append(json);
	}
	if (IsValidLadderRank(rank3))
	{
		Json::Value json;
		json["rank"] = rank3;
		json["uid"] = player3.uid;
		json["level"] = player3.level;
		json["name"] = player3.name;
		json["pic"] = player3.pic;
		players.append(json);
	}
	if (IsValidLadderRank(rank4))
	{
		Json::Value json;
		json["rank"] = rank4;
		json["uid"] = player4.uid;
		json["level"] = player4.level;
		json["name"] = player4.name;
		json["pic"] = player4.pic;
		players.append(json);
	}
	if (IsValidLadderRank(rank5))
	{
		Json::Value json;
		json["rank"] = rank5;
		json["uid"] = player5.uid;
		json["level"] = player5.level;
		json["name"] = player5.name;
		json["pic"] = player5.pic;
		players.append(json);
	}
	return 0;
}

int CLogicAllServerLadder::ReportResult(unsigned uid, const Json::Value &data,int level)
{
	unsigned fuid = 0;
	int flevel = 0;
	int frank = 0;
	string fname;
	string fpic;
	unsigned tuid = 0;
	int tlevel = 0;
	int trank = 0;
	string tname;
	string tpic;
	int win = 0;
	//debug_log("ReportResult--1wq");
	Json::GetUInt(data, "fuid", fuid);
	Json::GetInt(data, "flevel", flevel);
	Json::GetInt(data, "frank", frank);
	Json::GetString(data, "fname", fname);
	Json::GetString(data, "fpic", fpic);
	Json::GetUInt(data, "tuid", tuid);
	Json::GetInt(data, "tlevel", tlevel);
	Json::GetInt(data, "trank", trank);
	Json::GetString(data, "tname", tname);
	Json::GetString(data, "tpic", tpic);

	if (!Json::GetInt(data, "win", win)
			|| !IsValidUid(fuid) || fuid != uid
			|| (frank != -1 && !IsValidLadderRank(frank)) || !IsValidLadderRank(trank))
	{
		error_log("[invalid_param][uid=%u,fuid=%u,tuid=%u,frank=%d,trank=%d,win=%d]",
				uid,fuid,tuid,frank,trank,win);
		PARAM_ERROR_RETURN_MSG("invalid_param");
	}

	string fLadderData;
	string tLadderData;
	CDataUserData dbUserData;
	int ret = dbUserData.GetAllServerLadder(fuid, fLadderData);
	if (ret != 0)
		return ret;
	if (IsValidUid(tuid))
	{
		ret = dbUserData.GetAllServerLadder(tuid, tLadderData);
		if (ret != 0)
			return ret;
	}

	Json::Value fLadder;
	Json::Value tLadder;
	Json::Reader reader;
	if (!fLadderData.empty() && !reader.parse(fLadderData, fLadder))
	{
		DATA_ERROR_RETURN_MSG("parse_fladder_fail");
	}

	if (IsValidUid(tuid) &&!tLadderData.empty() && !reader.parse(tLadderData, tLadder))
	{
		DATA_ERROR_RETURN_MSG("parse_tladder_fail");
	}

	CheckLadderJson(fLadder);
	CheckLadderJson(tLadder);

	if (win == 1)
	{
		int win_unm = fLadder["win"].asInt()+1;
		fLadder["win"] = win_unm ;

		CDataLadder* pladder = GetCDataLadder(level);
		if (!pladder)
		{
			DB_ERROR_RETURN_MSG("init_ladder_fail");
		}
		LadderError error = LE_SUCCESS;
		ret  = pladder->Win(frank, fuid, flevel, fname, fpic,
				trank, tuid, tlevel, tname, tpic, Time::GetGlobalTime(), error);
		if (ret != 0)
		{
			error_log("[ladder win][uid=%u,frank=%d,fuid=%u,trank=%d,tuid=%u,error=%d]",
					uid,frank,fuid,trank,tuid,error);
			DB_ERROR_RETURN_MSG("set_ladder_fail");
		}
		fLadder["rank"] = frank;
		tLadder["rank"] = trank;
		RandLadderPlayers(fLadder);
		RandLadderPlayers(tLadder);
	}
	else
	{
		fLadder["win"] = 0;
	}

	Json::Value flog;
	flog["ts"] = Time::GetGlobalTime();
	flog["name"] = tname;
	flog["win"] = win;
	flog["rank"] = fLadder["rank"].asInt();
	flog["toward"] = 1;
	fLadder["log"].append(flog);
	unsigned size = fLadder["log"].size();
	if (size > 4)
	{
		for (unsigned i = 0; i < 4; i++)
		{
			fLadder["log"][i] = fLadder["log"][size - 4 + i];
		}
		fLadder["log"].resize(4);
	}

	if (IsValidUid(tuid))
	{
		Json::Value tlog;
		tlog["ts"] = Time::GetGlobalTime();
		tlog["name"] = fname;
		tlog["win"] = win == 1 ? 0 : 1;
		tlog["rank"] = tLadder["rank"].asInt();
		tlog["toward"] = 0;
		tLadder["log"].append(tlog);
		size = tLadder["log"].size();
		if (size > 4)
		{
			for (unsigned i = 0; i < 4; i++)
			{
				tLadder["log"][i] = tLadder["log"][size - 4 + i];
			}
			tLadder["log"].resize(4);
		}
	}
	fLadder["upts"] = Time::GetGlobalTime();
	Json::FastWriter writer;
	fLadderData = writer.write(fLadder);
	//debug_log("fLadderData=%s",fLadderData.c_str());
	ret = dbUserData.SetAllServerLadder(fuid, fLadderData);
	if (ret != 0)
		return ret;
	if (IsValidUid(tuid))
	{
		tLadder["upts"] = Time::GetGlobalTime();
		tLadderData = writer.write(tLadder);
		ret = dbUserData.SetAllServerLadder(tuid, tLadderData);
		if (ret != 0)
			return ret;
	}
	return 0;
}

int CLogicAllServerLadder::ViewTopOne(Json::Value &result)
{
	int ret = 0;
	LadderItem player;
	bool sucess = false;
	for (int i=1; i<=3; i++)
	{
		Json::Value tempData;
		CDataLadder* pladder = GetCDataLadder(i);
		if (NULL != pladder)
		{
			memset(&player,0,sizeof(LadderItem));
			ret = pladder->GetTopOne(player);
			if (0 == ret)
			{
				sucess = true;
				CLogicUser logicUser;
				DataUser user;
				tempData["power"] = 0;
				tempData["union"] = "";
				if (IsValidUid(player.uid))
				{
					ret = logicUser.GetUser(player.uid,user);
					if (0 == ret)
					{
						Json::Value userStat;
						Json::FromString(userStat,user.user_stat);
						tempData["power"] = userStat["power"];
						if (IsAllianceId(user.alliance_id))
						{
							CLogicAlliance logicAlliance;
							DataAlliance alliance;
							ret = logicAlliance.GetAlliance(user.alliance_id,alliance);
							if (0 == ret)
							{
								tempData["union"] = alliance.name;
							}
						}
					}
				}
				tempData["uid"] = player.uid;
				tempData["level"] = player.level;
				tempData["name"] = player.name;
				tempData["pic"] = player.pic;
			}
		}

		if (!sucess)
		{
			tempData["power"] = 0;
			tempData["uid"] = 0;
			tempData["level"] = 0;
			tempData["name"] = "";
			tempData["pic"] = "";
			tempData["union"] = "";
		}
		result[i-1] = tempData;
	}
	return 0;
}
