#include "LogicFightArena.h"

CDataFightArena* CLogicFightArena::GetCDataFightArena()
{
	GET_MEM_DATA_SEM(CDataFightArena, CONFIG_ARENA_DIR,sem_fightarena)
	/*static CDataFightArena* pFightArena = NULL;
	if (!pFightArena)
	{
		pFightArena = new CDataFightArena();
		if (pFightArena->Init(Config::GetValue(CONFIG_ARENA_DIR)) != 0)
		{
			delete pFightArena;
			pFightArena = NULL;
		}
	}
	return pFightArena;*/
}

int CLogicFightArena::GetFightArena(unsigned arenaId, DataFightArena &data)
{
	CDataFightArena* pLogicArena = GetCDataFightArena();
	if (!pLogicArena)
	{
		DB_ERROR_RETURN_MSG("init_fight_arena_fail");
	}
	int ret = pLogicArena->GetFightArena(arenaId, data);
	if (ret != 0)
	{
		error_log("[GetFightArena][arenaId=%u,ret=%d]",arenaId,ret);
		DB_ERROR_RETURN_MSG("get_fight_arena_fail");
	}
	return 0;
}

int CLogicFightArena::GetFightArenaLimit(unsigned arenaId, DataFightArenaLimit &data)
{
	CDataFightArena* pLogicArena = GetCDataFightArena();
	if (!pLogicArena)
	{
		DB_ERROR_RETURN_MSG("init_fight_arena_fail");
	}
	int ret = pLogicArena->GetFightArenaLimit(arenaId, data);
	if (ret != 0)
	{
		error_log("[GetFightArena][arenaId=%u,ret=%d]",arenaId,ret);
		DB_ERROR_RETURN_MSG("get_fight_arena_fail");
	}
	return 0;
}

int CLogicFightArena::BreathFightArena(int arenaId, unsigned challenger)
{
	CDataFightArena* pLogicArena = GetCDataFightArena();
	if (!pLogicArena)
	{
		DB_ERROR_RETURN_MSG("init_fight_arena_fail");
	}
	int ret = pLogicArena->BreathFightArena(arenaId, challenger);
	if (ret != 0)
	{
		error_log("[BreathFightArena][arenaId=%u,challenger=%u,ret=%d]",arenaId,challenger,ret);
		DB_ERROR_RETURN_MSG("set_fight_arena_fail");
	}
	return 0;
}

bool CLogicFightArena::EnableChallenge(int arenaId, unsigned challenger, unsigned addPrize, DataFightArenaLimit &data)
{
	CDataFightArena* pLogicArena = GetCDataFightArena();
	if (!pLogicArena)
		return false;

	return pLogicArena->EnableChallenge(arenaId, challenger, addPrize, data);
}

int CLogicFightArena::UpdateWinFightArena(int arenaId, unsigned host, unsigned level, const string &name,
		const string &pic, const string &archive, DataFightArenaLimit &data)
{
	CDataFightArena* pLogicArena = GetCDataFightArena();
	if (!pLogicArena)
	{
		DB_ERROR_RETURN_MSG("init_fight_arena_fail");
	}
	int ret = pLogicArena->UpdateWinFightArena(arenaId, host, level, name, pic, archive, data);
	if (ret != 0)
	{
		error_log("[UpdateWinFightArena][arenaId=%u,host=%u,ret=%d]",arenaId,host,ret);
		DB_ERROR_RETURN_MSG("update_win_fight_arena_fail");
	}
	return 0;
}

int CLogicFightArena::UpdateLoseFightArena(int arenaId, unsigned challenger, DataFightArenaLimit &data)
{
	CDataFightArena* pLogicArena = GetCDataFightArena();
	if (!pLogicArena)
	{
		DB_ERROR_RETURN_MSG("init_fight_arena_fail");
	}
	int ret = pLogicArena->UpdateLoseFightArena(arenaId, challenger, data);
	if (ret != 0)
	{
		error_log("[UpdateLoseFightArena][arenaId=%u,challenger=%u,ret=%d]",arenaId,challenger,ret);
		DB_ERROR_RETURN_MSG("update_lose_fight_arena_fail");
	}
	return 0;
}

int CLogicFightArena::GetFightArenaJson(Json::Value &arena)
{
	int ret;
	DataFightArenaLimit dataArena;
	arena.resize(FIGHT_ARENA_ID_MAX - FIGHT_ARENA_ID_MIN + 1);
	for (unsigned id = FIGHT_ARENA_ID_MIN; id <= FIGHT_ARENA_ID_MAX; id++)
	{
		ret = GetFightArenaLimit(id, dataArena);
		if (ret != 0)
			return ret;
		unsigned temp = id - FIGHT_ARENA_ID_MIN;
		arena[temp]["arenaid"] = id;
		arena[temp]["uid"] = dataArena.host;
		arena[temp]["name"] = dataArena.name;
		arena[temp]["pic"] = dataArena.pic;
		arena[temp]["level"] = dataArena.level;
		arena[temp]["winCount"] = dataArena.winCount;
		arena[temp]["challenger"] = dataArena.challenger;
		arena[temp]["challengeTs"] = dataArena.challengeTs;
		arena[temp]["preChallenger"] = dataArena.preChallenger;
		arena[temp]["prize"] = dataArena.prizePool;
	}
	return 0;
}

int CLogicFightArena::Load(unsigned arenaid, unsigned uidBy, LoadType loadType, int regfee, Json::Value &result)
{
	if (!IsValidFightArenaId(arenaid))
	{
		PARAM_ERROR_RETURN_MSG("param_error");
	}
	int ret;
	Json::Reader reader;
	if (loadType == LT_VIEW)
	{
		DataFightArena dataArena;
		ret = GetFightArena(arenaid, dataArena);
		if (ret != 0)
			return ret;
		if (!reader.parse(dataArena.archive, result))
		{
			DB_ERROR_RETURN_MSG("fight_arena_data_error");
		}
		result["attackpermitted"] = (dataArena.host == uidBy?APT_BAN_SELF:APT_ALLOW);
	}
	else if (loadType == LT_SPORTS)
	{
		DataPay pay;
		CLogicPay logicPay;
		ret = logicPay.GetPay(uidBy, pay);
		if(ret != 0)
			return ret;
		if (pay.coins < (unsigned)regfee)
		{
			LOGIC_ERROR_RETURN_MSG("not_enough_coins");
		}
		Json::Value userFlag;
		CLogicUser logicUser;
		ret = logicUser.GetUserFlag(uidBy, userFlag);
		if (ret != 0)
			return ret;
		string today = CTime::FormatTime("%y%m%d", Time::GetGlobalTime());
		if (userFlag.isMember("fit"))
		{
			if (userFlag["fit"][(unsigned)2].asUInt() + 300 > Time::GetGlobalTime())
			{
				LOGIC_ERROR_RETURN_MSG("continuous_challenge_limit");
			}
			if (userFlag["fit"][(unsigned)0].asString() == today)
			{
				if (userFlag["fit"][(unsigned)1].asInt() >= 10)
				{
					LOGIC_ERROR_RETURN_MSG("challenge_count_limit");
				}
				userFlag["fit"][(unsigned)1] = userFlag["fit"][(unsigned)1].asInt() + 1;
				userFlag["fit"][(unsigned)2] = Time::GetGlobalTime();
			}
			else
			{
				userFlag["fit"][(unsigned)0] = today;
				userFlag["fit"][(unsigned)1] = 1;
				userFlag["fit"][(unsigned)2] = Time::GetGlobalTime();
			}
		}
		else
		{
			userFlag["fit"].resize(3);
			userFlag["fit"][(unsigned)0] = today;
			userFlag["fit"][(unsigned)1] = 1;
			userFlag["fit"][(unsigned)2] = Time::GetGlobalTime();
		}

		int hostfee = 0, prize = 0;
		if (regfee > 0)
		{
			hostfee = regfee * 4 / 10;
			prize = regfee * 5 / 10;
		}
		DataFightArenaLimit dataArena;
		if (!EnableChallenge(arenaid, uidBy, prize, dataArena))
		{
			LOGIC_ERROR_RETURN_MSG("arena_not_idle");
		}
		if (CLogicFighting::GetFightingArchive(FIGHTING_ID_MIN, result) != 0)
		{
			DB_ERROR_RETURN_MSG("fight_data_error");
		}
		if (regfee > 0)
		{
			logicPay.ChangePay(uidBy, 0, 0 - regfee, "FIGHT");
		}
		if (IsValidUid(dataArena.host) && hostfee > 0)
		{
			logicPay.ChangePay(dataArena.host, 0, hostfee, "FIGHTHOST");
			if (ret == 0)
			{
				CLogicUserBasic logicUserBasic;
				DataUserBasic userBasic;
				if (logicUserBasic.GetUserBasicLimit(uidBy, OpenPlatform::GetType(), userBasic) == 0)
				{
					Json::Value updates;
					updates.resize(1);
					updates[(unsigned)0]["s"] = "ARENAFEE";
					updates[(unsigned)0]["type"] = 2;
					updates[(unsigned)0]["name"] = userBasic.name;
					updates[(unsigned)0]["gold"] = hostfee;
					updates[(unsigned)0]["arenaid"] = arenaid;
					updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
					CLogicUpdates logicUpdates;
					logicUpdates.AddUpdates(dataArena.host, updates);
				}
			}
		}
		logicUser.SetUserFlag(uidBy, userFlag);
	}
	else
	{
		PARAM_ERROR_RETURN_MSG("param_error");
	}

	result["userid"] = arenaid;

	return 0;
}

int CLogicFightArena::Save(unsigned arenaid, DataUser &userBy, const Json::Value &data, Json::Value &result)
{
	if (!IsValidFightArenaId(arenaid))
	{
		PARAM_ERROR_RETURN_MSG("param_error");
	}

	return 0;
}

int CLogicFightArena::FightOver(unsigned arenaid, unsigned challenger, bool isWin, Json::Value &result)
{
	if (!IsValidFightArenaId(arenaid))
	{
		PARAM_ERROR_RETURN_MSG("param_error");
	}
	int ret;
	DataFightArenaLimit dataArena;
	if (isWin)
	{
		DataUserBasic userBasic;
		DataUser user;
		CLogicUser logicUser;
		CLogicUserBasic logicUserBasic;
		ret = logicUserBasic.GetUserBasicLimitSmart(challenger, OpenPlatform::GetType(), userBasic);
		if (ret != 0)
			return ret;
		ret = logicUser.GetUser(challenger, user);
		if (ret != 0)
			return ret;
		Json::Value archive;
		archive["userid"] = arenaid;
		archive["name"] = userBasic.name;
		archive["pic"] = userBasic.figure_url;
		archive["viptype"] = userBasic.vip_type;
		archive["viplevel"] = userBasic.vip_level;
		archive["pt"] = userBasic.platform;
		archive["invite_count"] = user.invite_count;
		archive["today_invite_count"] = user.today_invite_count;
		archive["level"] = user.level;
		archive["exp"] = Convert::UInt64ToString(user.point);
		archive["tutorialstage"] = user.tutorial_stage;
		archive["resources"].resize(5);
		archive["resources"][(unsigned)0]["c"] = user.r1;
		archive["resources"][(unsigned)0]["m"] = user.r1_max;
		archive["resources"][(unsigned)1]["c"] = user.r2;
		archive["resources"][(unsigned)1]["m"] = user.r2_max;
		archive["resources"][(unsigned)2]["c"] = user.r3;
		archive["resources"][(unsigned)2]["m"] = user.r3_max;
		archive["resources"][(unsigned)3]["c"] = user.r4;
		archive["resources"][(unsigned)3]["m"] = user.r4_max;
		archive["resources"][(unsigned)4]["c"] = user.r5;
		archive["resources"][(unsigned)4]["m"] = user.r5_max;
		archive["lasttime"] = user.last_save_time;
		archive["gcbase"] = user.gcbase;
		archive["newgcbase"] = user.newgcbase;
		archive["gcbuy"] = user.gcbuy;
		archive["protected_time"] = user.protected_time;
		Json::Reader reader;
		user.user_tech.empty() || reader.parse(user.user_tech, archive["tech"]);
		user.barrackQ.empty() || reader.parse(user.barrackQ, archive["barrackQ"]);
		user.soldier.empty() || reader.parse(user.soldier, archive["soldier"]);
		user.soldierlevel.empty() || reader.parse(user.soldierlevel, archive["soldierlevel"]);
		user.buildQ.empty() || reader.parse(user.buildQ, archive["buildQ"]);
		user.skillQ.empty() || reader.parse(user.skillQ, archive["skillQ"]);
		user.trainQ.empty() || reader.parse(user.trainQ, archive["trainQ"]);
		CLogicBuilding logicBuiding;
		ret = logicBuiding.GetBuilding(challenger, archive["baseop"]);
		if (ret != 0)
			return ret;
		CLogicHero logicHero;
		ret = logicHero.GetHero(challenger, archive["hero"]);
		if (ret != 0)
			return ret;
		CLogicEquipment logicEquip;
		ret = logicEquip.GetEquipmentUsed(challenger, archive["equipment"]);
		if (ret != 0)
			return ret;

		Json::FastWriter writer;
		string archiveStr = writer.write(archive);
		ret = UpdateWinFightArena(arenaid, challenger, user.level, userBasic.name, userBasic.figure_url, archiveStr, dataArena);
		if (ret == 0)
		{
			if (dataArena.prizePool > 0)
			{
				CLogicPay logicPay;
				if (logicPay.ChangePay(challenger, 0, dataArena.prizePool, "FIGHTREWARD") == 0)
					result["prize"] = dataArena.prizePool;
			}
			if (IsValidUid(dataArena.host))
			{
				Json::Value updates;
				updates.resize(1);
				updates[(unsigned)0]["s"] = "ARENALOSE";
				updates[(unsigned)0]["type"] = 2;
				updates[(unsigned)0]["name"] = userBasic.name;
				updates[(unsigned)0]["gold"] = dataArena.prizePool;
				updates[(unsigned)0]["arenaid"] = arenaid;
				updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
				CLogicUpdates logicUpdates;
				logicUpdates.AddUpdates(dataArena.host, updates);
			}
		}
	}
	else
	{
		UpdateLoseFightArena(arenaid, challenger, dataArena);
	}

	return 0;
}
