#include "LogicArena.h"

CDataArena* CLogicArena::GetCDataArena()
{
	GET_MEM_DATA_SEM(CDataArena, CONFIG_ARENA_DIR, sem_arena);
	/*static CDataArena* pArena = NULL;
	if (!pArena)
	{
		pArena = new CDataArena();
		if (pArena->Init(Config::GetValue(CONFIG_ARENA_DIR)) != 0)
		{
			delete pArena;
			pArena = NULL;
		}
	}
	return pArena;*/
}

int CLogicArena::GetArena(unsigned arenaId, DataArena &data)
{
	CDataArena* pLogicArena = GetCDataArena();
	if (!pLogicArena)
	{
		DB_ERROR_RETURN_MSG("init_arena_fail");
	}
	int ret = pLogicArena->GetArena(arenaId, data);
	if (ret != 0)
	{
		error_log("[GetArena][arenaId=%u,ret=%d]",arenaId,ret);
		DB_ERROR_RETURN_MSG("get_arena_fail");
	}
	return 0;
}

int CLogicArena::GetArenaLimit(unsigned arenaId, DataArenaLimit &data)
{
	CDataArena* pLogicArena = GetCDataArena();
	if (!pLogicArena)
	{
		DB_ERROR_RETURN_MSG("init_arena_fail");
	}
	int ret = pLogicArena->GetArenaLimit(arenaId, data);
	if (ret != 0)
	{
		error_log("[GetArena][arenaId=%u,ret=%d]",arenaId,ret);
		DB_ERROR_RETURN_MSG("get_arena_fail");
	}
	return 0;
}

int CLogicArena::BreathArena(int arenaId, unsigned challenger)
{
	CDataArena* pLogicArena = GetCDataArena();
	if (!pLogicArena)
	{
		DB_ERROR_RETURN_MSG("init_arena_fail");
	}
	int ret = pLogicArena->BreathArena(arenaId, challenger);
	if (ret != 0)
	{
		error_log("[BreathArena][arenaId=%u,challenger=%u,ret=%d]",arenaId,challenger,ret);
		DB_ERROR_RETURN_MSG("set_arena_fail");
	}
	return 0;
}

bool CLogicArena::EnableChallenge(int arenaId, unsigned challenger, unsigned addPrize, DataArena &data)
{
	CDataArena* pLogicArena = GetCDataArena();
	if (!pLogicArena)
	{
		return false;
	}
	return pLogicArena->EnableChallenge(arenaId, challenger, addPrize, data);
}

int CLogicArena::UpdateWinArena(int arenaId, unsigned host, unsigned level, const string &name,
		const string &pic, const string &archive, DataArenaLimit &data)
{
	CDataArena* pLogicArena = GetCDataArena();
	if (!pLogicArena)
	{
		DB_ERROR_RETURN_MSG("init_arena_fail");
	}
	int ret = pLogicArena->UpdateWinArena(arenaId, host, level, name, pic, archive, data);
	if (ret != 0)
	{
		error_log("[UpdateWinArena][arenaId=%u,host=%u,ret=%d]",arenaId,host,ret);
		DB_ERROR_RETURN_MSG("update_win_arena_fail");
	}
	return 0;
}

int CLogicArena::GetArenaJson(Json::Value &arena)
{
	int ret;
	DataArenaLimit dataArena;
	arena.resize(ARENA_ID_MAX - ARENA_ID_MIN + 1);
	for (unsigned id = ARENA_ID_MIN; id <= ARENA_ID_MAX; id++)
	{
		ret = GetArenaLimit(id, dataArena);
		if (ret != 0)
			return ret;
		unsigned temp = id - ARENA_ID_MIN;
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

int CLogicArena::Load(unsigned arenaid, unsigned uidBy, LoadType loadType, int regfee, Json::Value &result)
{
	if (!IsValidArenaId(arenaid))
	{
		PARAM_ERROR_RETURN_MSG("param_error");
	}
	int ret;
	Json::Reader reader;
	DataArena dataArena;
	if (loadType == LT_VIEW)
	{
		ret = GetArena(arenaid, dataArena);
		if (ret != 0)
			return ret;
		if (!reader.parse(dataArena.archive, result))
		{
			DB_ERROR_RETURN_MSG("arena_data_error");
		}
		result["attackpermitted"] = (dataArena.host == uidBy?APT_BAN_SELF:APT_ALLOW);
	}
	else if (loadType == LT_ATTACK)
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
		if (userFlag.isMember("arn"))
		{
			if (userFlag["arn"][(unsigned)2].asUInt() + 300 > Time::GetGlobalTime())
			{
				LOGIC_ERROR_RETURN_MSG("continuous_challenge_limit");
			}
			if (userFlag["arn"][(unsigned)0].asString() == today)
			{
				if (userFlag["arn"][(unsigned)1].asInt() >= 10)
				{
					LOGIC_ERROR_RETURN_MSG("challenge_count_limit");
				}
				userFlag["arn"][(unsigned)1] = userFlag["arn"][(unsigned)1].asInt() + 1;
				userFlag["arn"][(unsigned)2] = Time::GetGlobalTime();
			}
			else
			{
				userFlag["arn"][(unsigned)0] = today;
				userFlag["arn"][(unsigned)1] = 1;
				userFlag["arn"][(unsigned)2] = Time::GetGlobalTime();
			}
		}
		else
		{
			userFlag["arn"].resize(3);
			userFlag["arn"][(unsigned)0] = today;
			userFlag["arn"][(unsigned)1] = 1;
			userFlag["arn"][(unsigned)2] = Time::GetGlobalTime();
		}

		int hostfee = 0, prize = 0;
		if (regfee > 0)
		{
			hostfee = regfee * 4 / 10;
			prize = regfee * 5 / 10;
		}
		if (!EnableChallenge(arenaid, uidBy, prize, dataArena))
		{
			LOGIC_ERROR_RETURN_MSG("arena_not_idle");
		}
		if (!reader.parse(dataArena.archive, result))
		{
			DB_ERROR_RETURN_MSG("arena_data_error");
		}
		if (regfee > 0)
		{
			logicPay.ChangePay(uidBy, 0, 0 - regfee, "ARENA");
		}
		if (IsValidUid(dataArena.host) && hostfee > 0)
		{
			ret = logicPay.ChangePay(dataArena.host, 0, hostfee, "ARENAHOST");
			if (ret == 0)
			{
				CLogicUserBasic logicUserBasic;
				DataUserBasic userBasic;
				if (logicUserBasic.GetUserBasicLimit(uidBy, OpenPlatform::GetType(), userBasic) == 0)
				{
					Json::Value updates;
					updates.resize(1);
					updates[(unsigned)0]["s"] = "ARENAFEE";
					updates[(unsigned)0]["type"] = 1;
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

int CLogicArena::Save(unsigned arenaid, DataUser &userBy, Json::Value &data, Json::Value &result)
{
	if (!IsValidArenaId(arenaid))
	{
		PARAM_ERROR_RETURN_MSG("param_error");
	}
	int ret = 0;
	int damage = 0;
	Json::GetInt(data, "damage", damage);
	if (damage >= 99)	//打赢了擂主
	{
		DataArenaLimit dataArena;
		ret = GetArenaLimit(arenaid, dataArena);
		if (ret != 0)
			return ret;
		if (dataArena.challenger != 0)
		{
			DataUserBasic userBasic;
			CLogicUserBasic logicUserBasic;
			ret = logicUserBasic.GetUserBasicLimitSmart(userBy.uid, OpenPlatform::GetType(), userBasic);
			if (ret != 0)
				return ret;
			Json::Value archive;
			archive["userid"] = arenaid;
			archive["name"] = userBasic.name;
			archive["pic"] = userBasic.figure_url;
			archive["viptype"] = userBasic.vip_type;
			archive["viplevel"] = userBasic.vip_level;
			archive["pt"] = userBasic.platform;
			archive["invite_count"] = userBy.invite_count;
			archive["today_invite_count"] = userBy.today_invite_count;
			archive["level"] = userBy.level;
			archive["exp"] = Convert::UInt64ToString(userBy.point);
			archive["tutorialstage"] = userBy.tutorial_stage;
			archive["resources"].resize(5);
			archive["resources"][(unsigned)0]["c"] = userBy.r1;
			archive["resources"][(unsigned)0]["m"] = userBy.r1_max;
			archive["resources"][(unsigned)1]["c"] = userBy.r2;
			archive["resources"][(unsigned)1]["m"] = userBy.r2_max;
			archive["resources"][(unsigned)2]["c"] = userBy.r3;
			archive["resources"][(unsigned)2]["m"] = userBy.r3_max;
			archive["resources"][(unsigned)3]["c"] = userBy.r4;
			archive["resources"][(unsigned)3]["m"] = userBy.r4_max;
			archive["resources"][(unsigned)4]["c"] = userBy.r5;
			archive["resources"][(unsigned)4]["m"] = userBy.r5_max;
			archive["currencys"][(unsigned)0] = userBy.gcbase;
			archive["currencys"][(unsigned)1] = userBy.gcbuy;
			archive["currencys"][(unsigned)2] = userBy.prosper;
			archive["lasttime"] = userBy.last_save_time;
			archive["newgcbase"] = userBy.newgcbase;
			archive["protected_time"] = userBy.protected_time;
			Json::Reader reader;
			userBy.user_tech.empty() || reader.parse(userBy.user_tech, archive["tech"]);
			userBy.barrackQ.empty() || reader.parse(userBy.barrackQ, archive["barrackQ"]);
			userBy.soldier.empty() || reader.parse(userBy.soldier, archive["soldier"]);
			userBy.soldierlevel.empty() || reader.parse(userBy.soldierlevel, archive["soldierlevel"]);
			userBy.buildQ.empty() || reader.parse(userBy.buildQ, archive["buildQ"]);
			userBy.skillQ.empty() || reader.parse(userBy.skillQ, archive["skillQ"]);
			userBy.trainQ.empty() || reader.parse(userBy.trainQ, archive["trainQ"]);
			CLogicBuilding logicBuiding;
			ret = logicBuiding.GetBuilding(userBy.uid, archive["baseop"]);
			if (ret != 0)
				return ret;
			CLogicHero logicHero;
			ret = logicHero.GetHero(userBy.uid, archive["hero"]);
			if (ret != 0)
				return ret;
			CLogicEquipment logicEquip;
			ret = logicEquip.GetEquipmentUsed(userBy.uid, archive["equipment"]);
			if (ret != 0)
				return ret;

			Json::FastWriter writer;
			string archiveStr = writer.write(archive);
			ret = UpdateWinArena(arenaid, userBy.uid, userBy.level, userBasic.name, userBasic.figure_url, archiveStr, dataArena);
			if (ret == 0)
			{
				if (dataArena.prizePool > 0)
				{
					CLogicPay logicPay;
					ret = logicPay.ChangePay(userBy.uid, 0, dataArena.prizePool, "ARENAREWARD");
					if (ret == 0) result["prize"] = dataArena.prizePool;
				}

				if (IsValidUid(dataArena.host))
				{
					Json::Value updates;
					updates.resize(1);
					updates[(unsigned)0]["s"] = "ARENALOSE";
					updates[(unsigned)0]["type"] = 1;
					updates[(unsigned)0]["name"] = userBasic.name;
					updates[(unsigned)0]["gold"] = dataArena.prizePool;
					updates[(unsigned)0]["arenaid"] = arenaid;
					updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
					CLogicUpdates logicUpdates;
					logicUpdates.AddUpdates(dataArena.host, updates);
				}
			}
		}
	}
	else
	{
		BreathArena(arenaid, userBy.uid);
	}

	if (Json::IsObject(data, "attackinfo"))
	{
		CLogicArchive logicArchive;
		ret = logicArchive.ProcessAttackInfo(userBy.uid, data["attackinfo"]);
		if (ret != 0)
			return ret;
	}
	return 0;
}
