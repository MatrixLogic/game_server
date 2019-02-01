#include "LogicArchive.h"

static struct S_LOAD_TYPE {
	LoadType type;
	const char *desc;
} loadTypes[] = { { LT_BUILD, "build" }, { LT_VIEW, "view" },
		{ LT_HELP, "help" }, { LT_ATTACK, "attack" }, { LT_SPORTS, "sports" }, {
				LT_UNKNOW, NULL } };

static int SetProtectedTime(int userLevel, int damage,
		unsigned &protectedTime) {
	const unsigned anHour = 60 * 60;
	const unsigned halfHour = 30 * 60;
	protectedTime = Time::GetGlobalTime();
	if (userLevel < 0) {
		return R_ERROR;
	}
	if (userLevel >= 1 && userLevel <= 30) {
		if (damage >= 70) {
			protectedTime += anHour;
		} else if (damage >= 50) {
			protectedTime += halfHour;
		}
	} else if (userLevel >= 31 && userLevel <= 60) {
		if (damage >= 70) {
			protectedTime += 2 * anHour;
		} else if (damage >= 50) {
			protectedTime += anHour;
		}
	} else if (userLevel > 60) {
		if (damage >= 70) {
			protectedTime += 5 * anHour;
		} else if (damage >= 50) {
			protectedTime += 2 * anHour + halfHour;
		}
	} else {
		error_log("error level,level=%d", userLevel);
	}
	return 0;
}

int CLogicArchive::GetInitArchive(const Json::Value *&pArchive) {
	static unsigned s_lastUpdateTime = 0;
	static Json::Value s_initArchive(Json::objectValue);  // 对象类型
	if (Time::GetGlobalTime() - s_lastUpdateTime > 1800) {
		int result;
		string archivePath = Config::GetValue(CONFIG_INIT_ARCHIVE);
		if (!archivePath.empty()) {
			string data;
			result = File::Read(archivePath, data);
			if (result != 0) {
				fatal_log("[init archive not exist][ret=%d,path=%s]", result,
						archivePath.c_str());
				DB_ERROR_RETURN_MSG("db_init_archive_fail");
			}
			Json::Reader reader;
			if (!reader.parse(data, s_initArchive)) {
				fatal_log("[parse json fail][path=%s]", archivePath.c_str());
				DB_ERROR_RETURN_MSG("db_init_archive_fail");
			}
			s_lastUpdateTime = Time::GetGlobalTime();
		}
	}

	pArchive = &s_initArchive;

	return 0;
}

int CLogicArchive::Load(unsigned uid, unsigned uidBy, const string &type,
		const Json::Value &data, Json::Value &result) {
	LoadType loadType = LT_BUILD;
	for (int i = 0; loadTypes[i].desc != NULL; i++) {
		if (type == loadTypes[i].desc) {
			loadType = loadTypes[i].type;
			break;
		}
	}
	int ret = 0;
	CLogicUser logicUser;
	WorldPoint worldPoint;
	CLogicWorld logicWorld;
	CLogicBaseExtra logicBaseExtra;
	DataBaseExtra dataBaseExtra;
	// 判断要加载的是不是NPC角色
	if (IsNPCUid(uid)) {
		CLogicNpc logicNpc;
		ret = logicNpc.Load(uid, uidBy, result);
		if (ret != 0)
			return ret;
		//result["lasttime"] = Time::GetGlobalTime();

		result["attackpermitted"] = APT_ALLOW;
		result["online"] = 0;
		// 判断是不是世界bos
		if (IsWorldNPCUid(uid)) {
			unsigned world_pos;
			if (Json::GetUInt(data, "world_pos", world_pos)) {
				//result["world_pos"] = world_pos;
				result["world_pos"]["world_pos"] = world_pos;
				if (IsValidWorldPos(world_pos)) {
					logicWorld.GetUserInfo(world_pos, worldPoint);
					result["world_pos"]["type"] = worldPoint.map_flag;
				}
			}
			result["worldnpc"] = 1;
			if (loadType == LT_ATTACK) {
				WORLD_ATTACK_LOG("attack|%u|%u|%u|%d|%d|", uidBy, uid,
						world_pos, worldPoint.map_flag, 0);
			}
			return 0;
		}
		result["npc"] = 1;
		if (result["userid"].asInt() == 401)  // 竞技场初始化npc
				{
			result["name"] = "挑战我吧";
		}
		return 0;
	}
	// 判断是不是擂主
	if (IsValidArenaId(uid)) {
		int regfee = 0;
		Json::GetInt(data, "coin", regfee);
		CLogicArena logicArena;
		ret = logicArena.Load(uid, uidBy, loadType, regfee, result);
		if (ret != 0)
			return ret;
		result["arena"] = 1;
		result["online"] = 0;
		return 0;
	}
	if (IsValidInstanceId(uid)) {
		int iscontinue = 0;
		Json::GetInt(data, "iscontinue", iscontinue);
		CLogicInstance logicInstance;
		ret = logicInstance.Load(uid, uidBy, (iscontinue == 1 ? false : true),
				result);
		if (ret != 0)
			return ret;
		result["instance"] = 1;
		result["online"] = 0;
		return 0;
	}
	if (IsValidFightingId(uid)) {
		CLogicFighting logicFighting;
		ret = logicFighting.Load(uid, uidBy, result);
		if (ret != 0)
			return ret;
		result["fighting"] = 1;
		result["online"] = 0;
		return 0;
	}
	if (IsValidFightArenaId(uid)) {
		int regfee = 0;
		Json::GetInt(data, "coin", regfee);
		CLogicFightArena logicArena;
		ret = logicArena.Load(uid, uidBy, loadType, regfee, result);
		if (ret != 0)
			return ret;
		result["arena"] = 2;
		result["online"] = 0;
		return 0;
	}
	if (IsValidWorldBossId(uid)) {
		CLogicWorldBoss logicWorldBoss;
		ret = logicWorldBoss.Load(uid, uidBy, result);
		if (ret != 0)
			return ret;
		result["online"] = 0;
		return 0;
	}
	//AllianceBoss
	if (IsValidAllianceBossId(uid)) {
			CLogicAllianceBoss logicAllianceBoss;
			ret = logicAllianceBoss.Load(uid, uidBy, result);
			if (ret != 0)
				return ret;
			result["online"] = 0;
			return 0;
		}
	if (IsValidBMatchInstId(uid)) {
		CLogicBaseMatch logicBaseMatch;
		ret = logicBaseMatch.Load(uid, uidBy, result);
		if (ret != 0)
			return ret;
		result["online"] = 0;
		return 0;
	}
	if (IsValidAllServerBMatchInstId(uid)) {
		string url = "action=loadallserverbmatch";
		string strUid;
		String::Format(strUid, "%u", uid);
		url.append("&uid=");
		url += strUid;

		string strUidBy;
		String::Format(strUidBy, "%u", uidBy);
		url.append("&userid=");
		url += strUidBy;

		CLogicBaseMatch logicBaseMatch;
		ret = logicBaseMatch.RequestBaseMatch(url, result);
		if (ret != 0)
			return ret;
		result["online"] = 0;
		return 0;
	}
	if (IsValidPersonMatchId(uid)) {
		CLogicPersonMatch logicPersonMatch;
		ret = logicPersonMatch.Load(uid, uidBy, result);
		if (ret != 0)
			return ret;
		result["online"] = 0;
		return 0;
	}
	if (IsValidAllServerPersonMatchId(uid)) {
		string url = "action=loadallserverpmatch";
		string strUid;
		String::Format(strUid, "%u", uid);
		url.append("&uid=");
		url += strUid;

		string strUidBy;
		String::Format(strUidBy, "%u", uidBy);
		url.append("&userid=");
		url += strUidBy;

		CLogicBaseMatch logicBaseMatch;
		ret = logicBaseMatch.RequestBaseMatch(url, result);
		if (ret != 0)
			return ret;
		result["online"] = 0;
		return 0;
	}
	if (!IsValidUid(uid)) {
		REFUSE_RETURN_MSG("invalid_uid");
	}

	DataPay payData;
	CLogicPay logicPay;
	ret = logicPay.GetPay(uidBy, payData);
	if (ret != 0)
		return ret;

	Json::Reader reader;
	DataUserBasic userBasic;
	DataUser user;
	DataUserData userData;
	CLogicUserBasic logicUserBasic;
	ret = logicUserBasic.GetUserBasicLimitSmart(uid, OpenPlatform::GetType(),
			userBasic);
	if (ret != 0)
		return ret;
	ret = logicUser.GetUser(uid, user);
	if (ret != 0)
		return ret;

	bool isMainPos = true;
	unsigned world_pos;
	if (!Json::GetUInt(data, "world_pos", world_pos) || world_pos == 0) {
		if (IsValidWorldPos(user.mainpos)) {
			world_pos = user.mainpos;
		} else {
			world_pos = 0;
		}
	}
	// 默认主城坐标
	isMainPos = ((world_pos == 0 || world_pos == user.mainpos) ? true : false);
	if (!isMainPos) {
		if (IsValidUid(uid)) {
			ret = logicBaseExtra.GetBaseExtra(uid, world_pos, dataBaseExtra);
			if (0 != ret) {
				error_log("uid_worrldpos_mismatch,uid=%u,world_pos=%u,uidBy=%u",
						uid, world_pos, uidBy);
				LOGIC_ERROR_RETURN_MSG("uid_worrldpos_mismatch");
			}
		}
	}
	// add by aaron 20121030
	CLogicUserData logicUserData;
	ret = logicUserData.GetUserData(uid, userData);
	if (ret != 0)
		return ret;
	DataUser userBy;
	if (uid == uidBy) {
		userBy.copyLimit(user);
	} else {
		ret = logicUser.GetUserLimit(uidBy, userBy);
		if (ret != 0)
			return ret;
	}
	CLogicAlliance logicAlliance;
	DataAlliance alliance;
	DataAllianceMember allianceMember;
	vector<DataEmail> vDataEmail;
	CLogicEmail clogicEmail;
	if (IsAllianceId(user.alliance_id)) {
		ret = logicAlliance.GetAlliance(user.alliance_id, alliance);
		if (ret != 0)
			return ret;
		CDataAllianceMember dbAllianceMember;
		ret = dbAllianceMember.GetMember(user.alliance_id, uid, allianceMember);
		if (ret != 0) {
			if (R_ERR_NO_DATA == ret) {
				user.alliance_id = ALLIANCE_ID_NULL;
				CDataUser dbUser;
				dbUser.SetAllianceId(uid, ALLIANCE_ID_NULL);
				alliance.alliance_id = ALLIANCE_ID_NULL;
				alliance.flag = 0;
			} else {
				error_log("[GetMember fail][ret=%d,allianceId=%u,uid=%u]", ret,
						user.alliance_id, uid);
				DB_ERROR_RETURN_MSG("get_alliance_member_fail");
			}
		}
	}
	else
	{
		alliance.alliance_id = ALLIANCE_ID_NULL;
		alliance.flag = 0;
	}

	result["online"] = 0;
	if (CTime::GetDayInterval(user.last_active_time, Time::GetGlobalTime())
			!= 0) {
		result["onlinets"] = 0;
	} else {
		result["onlinets"] = user.ext;
	}
	unsigned ballid = 0;
	unsigned ballts = 0;
	CLogicDragonball logicBall;

	logicBall.WhichDragonballHold(uid, ballid, ballts);
	result["ballid"] = ballid;
	if (!logicBall.IsDragonballActivityTs() && ballid != 0) {
		result["ballid"] = INVALID_BALL_ID;
	}
	result["ballts"] = ballts;
	bool isFriend = false;
	//不同类型载入的差异部分

	if (uid == uidBy) {
		if (isMainPos) {
			if (user.status == US_BAN) {
				REFUSE_RETURN_MSG("being_blocked");
			}
			if (user.tribute_time > Time::GetGlobalTime())
			{  // 封一段时间，being_blocked_后面是还剩的秒数
				string retStr = "being_blocked_" + CTrans::ITOS(user.tribute_time-Time::GetGlobalTime());
				REFUSE_RETURN_MSG(retStr);
			}
			if (loadType != LT_BUILD)
			{
				REFUSE_RETURN_MSG("load_type_error");
			}
			//20131107 Ralf last_breath_time -> last_attacked_time : fix for online attacking bugs
			if (CLogicUser::GetFlag(user.bit_info, UF_BE_ATTACKED) && IsBeingAttacked(user.last_breath_time))
			{
				//debug_log("[load self be attacked,uid=%u,last_breath_time=%u]",uid,user.last_breath_time);
				LOGIC_ERROR_RETURN_MSG("u_being_attacked");
			}
			//20131107 Ralf last_save_uid -> last_load_save_uid : fix for bugs, can save only last load archieve
			user.last_save_uid = uid;
			ret = logicUser.SetUserLimit(uid, user);
			if (ret != 0)
				return ret;
			//debug_log("[load self uid=%u]",uid);

			//处理朝贡
			CLogicTribute logicTribute;
			ret = logicTribute.GetTribute(uid, result["tributes"]);
			if (ret != 0)
			return ret;
			if (!userData.npcattack.empty() && !reader.parse(userData.npcattack, result["npcattack"]))
			{
				error_log("[npcattack parse fail][uid=%u]",uid);
				DATA_ERROR_RETURN_MSG("parse_npcattack_fail");
			}
			if (!userData.gift.empty() && !reader.parse(userData.gift, result["gift"]))
			{
				error_log("[gift parse fail][uid=%u]",uid);
				DATA_ERROR_RETURN_MSG("parse_gift_fail");
			}
			if (!userData.dailyquest.empty() && !reader.parse(userData.dailyquest, result["dailyquest"]))
			{
				error_log("[dailyquest parse fail][uid=%u]",uid);
				DATA_ERROR_RETURN_MSG("parse_dailyquest_fail");
			}
			CLogicQuest logicQuest;
			ret = logicQuest.GetQuest(uid, result["quest"]);
			if (ret != 0)
				return ret;

			Json::Value userFlag;
			if (!user.user_flag.empty())
			{
				if(reader.parse(user.user_flag, userFlag))
				{
					if (!userFlag.isMember("dchg")
							|| CTime::GetDayInterval(userFlag["dchg"][0u].asUInt(), Time::GetGlobalTime()) != 0)
					{
						userFlag["dchg"][0u] = Time::GetGlobalTime();
						userFlag["dchg"][1u] = 0;
					}
					result["dailyacccharge"] = userFlag["dchg"][1u];
				}
				else
				{
					error_log("[user_flag parse fail][uid=%u]",uid);
					DATA_ERROR_RETURN_MSG("parse_user_flag_fail");
				}
			}

			// add by aaron 20130130
			result["paytotal"] = 0;
			if(userFlag.isMember("payTotal"))
			{
				int lastPay_ts = userFlag["payTotal"][1u].asUInt();
				if(lastPay_ts >= Config::GetIntValue(CONFIG_PAY_TOTAL_BEGIN_TS) && lastPay_ts <= Config::GetIntValue(CONFIG_PAY_TOTAL_END_TS))
				{
					result["paytotal"] = userFlag["payTotal"][0u];
				}
			}

			result["pointpay"] = 0;
			if(userFlag.isMember("pointpay"))
			{
				int lastPay_ts = userFlag["pointpay"][1u].asUInt();
				if(lastPay_ts >= Config::GetIntValue(CONFIG_PAYRANK_BEGIN_TS) && lastPay_ts <= Config::GetIntValue(CONFIG_PAYRANK_END_TS))
				{
					result["pointpay"] = userFlag["pointpay"][0u];
				}
			}

			if(!userFlag.isMember("heroCoins"))   //用于购买武将碎片
			{
				result["coins3"] = 0;
				result["todaycharge"] = 0;
			}
			else
			{
				result["coins3"] = userFlag["heroCoins"][1u].asInt();
				unsigned last_ts = userFlag["heroCoins"][0u].asUInt();
				unsigned now = time(0);
				if(CTime::GetDayInterval(last_ts,now) == 0)
				{
					result["todaycharge"] = userFlag["heroCoins"][2u].asInt();
				}
				else
				{
					result["todaycharge"] = 0;
				}
			}

			// add by aaron 20130130
			CLogicBroadcast logicBd;
			vector<BroadcastMessage> bdMsgs;
			ret = logicBd.GetBdMessage(bdMsgs);
			if (ret == 0 && bdMsgs.size() > 0)
			{
				result["broadcast"].resize(bdMsgs.size());
				for (unsigned i = 0; i < bdMsgs.size(); i++)
				{
					result["broadcast"][i]["id"] = bdMsgs[i].id;
					result["broadcast"][i]["content"] = bdMsgs[i].content;
				}
			}
			result["acccharge"] = user.accCharge;
			//未成年标志
			int nonage = 0;
			if (!userBasic.extra.empty())
			{
				Json::Value extraJson;
				if (reader.parse(userBasic.extra, extraJson))
				{
					if (Json::GetInt(extraJson, "nng", nonage) && nonage != 0)
					nonage = 1;
				}
				else
					error_log("[userBasic.extra parse fail][uid=%u]",uid);
			}
			result["nonage"] = nonage;
			// daily accharge add by aaron 20121106
			/********************for new email remind, add by aaron 20120803******************/
			ret = clogicEmail.GetInboxEmailList(uid,vDataEmail);
			if(ret == 0)
			{
				unsigned int iCount = vDataEmail.size();
				for(unsigned int i = 0; i < iCount;i++)
				{

					if(vDataEmail[i].read_ts == 0)
					{
						result["not_read"] = 1;
						break;
					}
					result["not_read"] = 0;
				}
			}
			/********************for new email remind, add by aaron 20120803******************/
			DataUserBasic dataUserBasic;
			CDataInviteMapping mappingdata;
			DataUser userInvited;
			CLogicUser logicuser;
			vector<unsigned> vUid;
			int iCount = 0;
			int level45Count = 0;
			int level25Count = 0;
			int level15Count = 0;
			ret = mappingdata.GetUid(uid, vUid);
			if(ret == 0)
			{
				int i = 0;
				iCount = vUid.size();
				for(i = iCount - 1;i >=0;i--)
				{
					logicuser.GetUser(vUid[i],userInvited);
					if( userInvited.level >= 45)
					{
						level45Count++;
					}
					if(userInvited.level >= 25 ) {
						level25Count++;
					}
					if(userInvited.level >= 15 )
					{
						level15Count++;
					} else
					{
						;
					}
				}
			}
			result["invite_reward_info"]["level45"] = level45Count;
			result["invite_reward_info"]["level25"] = level25Count;
			result["invite_reward_info"]["level15"] = level15Count;
			result["invite_reward_info"]["invite_count"] = iCount;
			// 返回开服天数
			result["open_days"] = CTime::GetDayInterval(Config::GetIntValue(CONFIG_OPEN_TS), Time::GetGlobalTime()) + 1;
			GetActivityTime(result["activitytime"]);
		}
		else
		{
			if (BEING_ATTACK == dataBaseExtra.being_attack_flag && IsBeingAttacked(dataBaseExtra.last_breath_time) && uidBy != dataBaseExtra.last_breath_uid)
			{
				LOGIC_ERROR_RETURN_MSG("u_being_attacked");
			}
			dataBaseExtra.last_breath_time = Time::GetGlobalTime();
			dataBaseExtra.last_breath_uid = uidBy;
			dataBaseExtra.being_attack_flag = NO_ATTACK;
			ret = logicBaseExtra.SetBaseExtra(uid,world_pos,dataBaseExtra);
			if(0 != ret)
			{
				LOGIC_ERROR_RETURN_MSG("update_baseExtra_failed");
			}
		}
		vector<DataUserAttack> attacks;
		CLogicAttack logicAttack;
		ret = logicAttack.GetUnreadAttacks(uid,world_pos, AT_DEFENCE, attacks,isMainPos);
		if(ret == 0)
		{
			result["attacks"].resize(attacks.size());
			for(unsigned i = 0; i < attacks.size(); i++)
			{
				Json::Value attackData;
				attackData["userid"] = attacks[i].opposite_uid;
				attackData["starttime"] = attacks[i].start_time;
				attackData["world_pos"] = attacks[i].worldpos;
				attackData["log"] = "";
				DataUserBasic userAttack;
				ret = logicUserBasic.GetUserBasicLimit(attacks[i].opposite_uid, (PlatformType)attacks[i].opposite_platform, userAttack);
				if(ret == 0)
				{
					attackData["fbid"] = userAttack.open_id;
					attackData["name"] = userAttack.name;
					attackData["pic_square"] = userAttack.figure_url;
				}
				result["attacks"][i] = attackData;
			}
		}
		// 读取分基地坐标信息
		logicBaseExtra.GetWorldPosList(uid,result["world_pos"]["sub_base_pos"]);
		//加载自己才加载stat
		if (!user.user_stat.empty()
				&& !reader.parse(user.user_stat, result["stats"])) {
			error_log("[user stat parse error][uid=%u]", uid);
			DATA_ERROR_RETURN_MSG("parse_user_stat_fail");
		}
	}
	else
	{	//加载别人的数据
		isFriend = logicUserBasic.IsFriend(uidBy, OpenPlatform::GetType(), uid);
		if(isMainPos)
		{
			if (IsOnlineUser(user.last_active_time))
			{
				result["online"] = 1;
			}
		}
		if (loadType == LT_ATTACK)
		{
			if (ballid != 0 && Time::GetGlobalTime() - ballts < 5 * 60)
			{	//持有龙珠不足5分钟不能被攻击
				LOGIC_ERROR_RETURN_MSG("dragonball_protect");
			}
			DataUserBasic userBasicBy;
			ret = logicUserBasic.GetUserBasicLimit(uidBy, OpenPlatform::GetType(), userBasicBy);
			if (ret != 0)
				return ret;
			//判断是否可以攻击
			CLogicMap logicMap;
			AttackPermitType permit = APT_ALLOW;
			if(isMainPos)
			{
				ret = logicMap.GetAttackPermit(user, userBy, permit);
				if (ret != 0)
					return ret;
				if ( ballid != 0
						&& logicBall.EnableAttackForDragonball(ballid, userBy.level)
						&& logicBall.IsDragonballActivityTs())
				{	//对方持有龙珠，并且加载者等级符合抢该龙珠的条件
					if (permit != APT_BAN_BEING_ATTACK
							&& permit != APT_BAN_ADMIN
							&& permit != APT_BAN_SELF
							&& permit != APT_BAN_ALLIANCE
							&& permit != APT_BAN_OTHER)
					{
						permit = APT_ALLOW;
					}
				}
				else
				{
					if (!IsAllowAttack(permit))
					{
						if (permit != APT_BAN_ONLINE || !CLogicUser::GetFlag(user.bit_info, UF_BE_CAN_ONLINE_ATTACKED))
						{
							string banMsg = "ban_attack_" + CTrans::ITOS(permit);
							LOGIC_ERROR_RETURN_MSG(banMsg.c_str());
						}
					}
				}
			}
			else
			{
				if(Time::GetGlobalTime() < dataBaseExtra.protected_time)
				{
					LOGIC_ERROR_RETURN_MSG("ban_attack_world_24_protected");
				}
				if (IsOnlineUser(dataBaseExtra.last_breath_time) && uid == dataBaseExtra.last_breath_uid)
				{
					result["online"] = 1;
					// LOGIC_ERROR_RETURN_MSG("user_online");
				}
				if (BEING_ATTACK == dataBaseExtra.being_attack_flag && IsBeingAttacked(dataBaseExtra.last_breath_time) && uidBy != dataBaseExtra.last_breath_uid)
				{
					LOGIC_ERROR_RETURN_MSG("being_attacked");
				}
				WORLD_ATTACK_LOG("attack|%u|%u|%u|%d|%d|",uidBy,uid,world_pos,dataBaseExtra.type,Time::GetGlobalTime() - dataBaseExtra.occupy_time);
			}

			DataAlliance allianceBy;
			if (IsAllianceId(userBy.alliance_id))
			{
				ret = logicAlliance.GetAlliance(userBy.alliance_id, allianceBy);
				if(ret != 0)
				return ret;
			}
			else
			{
				allianceBy.alliance_id = ALLIANCE_ID_NULL;
				allianceBy.flag = 0;
			}
			CLogicAttack logicAttack;
			DataAttack attackData;
			attackData.attack_id = 0;
			attackData.attack_uid = uidBy;
			attackData.defence_uid = uid;
			attackData.worldpos = world_pos;
			attackData.attack_platform = OpenPlatform::GetType();
			attackData.attack_user_name = userBasicBy.name;
			attackData.attack_photo = userBasicBy.figure_url;
			attackData.attack_alliance_id = allianceBy.alliance_id;
			attackData.attack_alliance_name = allianceBy.name;
			attackData.attack_alliance_flag = allianceBy.flag;
			attackData.defence_platform = userBasic.platform;
			attackData.defence_user_name = userBasic.name;
			attackData.defence_photo = userBasic.figure_url;
			attackData.defence_alliance_id = alliance.alliance_id;
			attackData.defence_alliance_name = alliance.name;
			attackData.defence_alliance_flag = alliance.flag;
			attackData.start_time = 0;
			attackData.end_time = 0;
			attackData.r1_loot = 0;
			attackData.r2_loot = 0;
			attackData.r3_loot = 0;
			attackData.r4_loot = 0;
			attackData.r5_loot = 0;
			ret = logicAttack.AddAttack(attackData);
			if (ret != 0)
			return ret;
			result["attackid"] = Convert::UInt64ToString(attackData.attack_id);
			//更新联盟动态
			if(isMainPos)
			{
				if(attackData.attack_alliance_id != attackData.defence_alliance_id)
				{
					if(IsAllianceId(attackData.attack_alliance_id))
					{
						Json::Value attackActive;
						attackActive["type"] = "attack";
						attackActive["attackid"] = Convert::UInt64ToString(attackData.attack_id);
						attackActive["a_uid"] = attackData.attack_uid;
						attackActive["a_name"] = attackData.attack_user_name;
						attackActive["d_uid"] = attackData.defence_uid;
						attackActive["d_name"] = attackData.defence_user_name;
						if(IsAllianceId(attackData.defence_alliance_id))
						{
							attackActive["d_aid"] = attackData.defence_alliance_id;
							attackActive["d_aname"] = attackData.defence_alliance_name;
							attackActive["d_aflag"] = attackData.defence_alliance_flag;
						}
						attackActive["time"] = Time::GetGlobalTime();
						ret = logicAlliance.AddActivity(allianceBy, attackActive);
					}
					if(IsAllianceId(attackData.defence_alliance_id))
					{
						Json::Value defenceActive;
						defenceActive["time"] = Time::GetGlobalTime();
						defenceActive["a_uid"] = attackData.attack_uid;
						defenceActive["a_name"] = attackData.attack_user_name;
						if(IsAllianceId(attackData.attack_alliance_id))
						{
							defenceActive["a_aid"] = attackData.attack_alliance_id;
							defenceActive["a_aname"] = attackData.attack_alliance_name;
							defenceActive["a_aflag"] = attackData.attack_alliance_flag;
						}
						ret = logicAlliance.AddLastAttack(attackData.defence_alliance_id, attackData.defence_uid, defenceActive);

						defenceActive["d_uid"] = attackData.defence_uid;
						defenceActive["d_name"] = attackData.defence_user_name;
						defenceActive["type"] = "defence";
						defenceActive["attackid"] = Convert::UInt64ToString(attackData.attack_id);
						ret = logicAlliance.AddActivity(alliance, defenceActive);
					}
				}
				//设置连续进攻保护
				int beAttackedCount = 0;
				ret = logicAttack.GetConBeAttackedCount(uid, beAttackedCount);
				/*
				if (ret == 0 && beAttackedCount >= 4)
				{
					user.protected_time = Time::GetGlobalTime() + CON_PROTECT_TIME;
				}
				*/
				result["beAttackedCount"] = beAttackedCount;

				//设置攻击状态
				CLogicUser::SetFlag(user.bit_info, UF_BE_ATTACKED, true);
				CLogicUser::SetFlag(user.bit_info, UF_BE_CAN_ONLINE_ATTACKED, false);
				ret = logicUser.UpdateBitInfo(uid, user.bit_info);
				if (ret != 0)
				return ret;
				//20131107 Ralf last_save_uid -> last_load_save_uid : fix for bugs, can save only last load archieve
				user.last_save_uid = uidBy;
				//20131107 Ralf last_breath_time -> last_attacked_time : fix for online attacking bugs
				user.last_breath_time = Time::GetGlobalTime();
				ret = logicUser.SetUserLimit(uid, user);
				if (ret != 0)
					return ret;
				//debug_log("[load attack uid=%u,userid=%u,last_breath_time=%u,]",uid, uidBy,user.last_breath_time);

				//取消攻击者保护
				if (IsInProtect(userBy.protected_time))
				{
					//ret = logicUser.UpdateProtectedTime(uidBy);
					userBy.protected_time = Time::GetGlobalTime();
				}
				userBy.last_breath_time = 0;
				//20131107 Ralf last_breath_time -> last_attacked_time : fix for online attacking bugs
				userBy.last_save_uid = uid;
				ret = logicUser.SetUserLimit(uidBy, userBy);
					if (ret != 0)
						return ret;
			}
			else
			{
				dataBaseExtra.last_breath_time = Time::GetGlobalTime();
				dataBaseExtra.last_breath_uid = uidBy;
				dataBaseExtra.being_attack_flag = BEING_ATTACK;
				ret = logicBaseExtra.SetBaseExtra(uid,world_pos,dataBaseExtra);
				if(0 != ret)
				{
					LOGIC_ERROR_RETURN_MSG("update_baseExtra_failed");
				}
			}
		}
		else if (loadType == LT_VIEW)
		{
			AttackPermitType permit = APT_ALLOW;
			if(isMainPos) // add by aaron
			{
				if ( CLogicUser::GetFlag(user.bit_info, UF_BE_ATTACKED)
				//20131107 Ralf last_save_uid -> last_load_save_uid : fix for bugs, can save only last load archieve
						//&& user.last_save_uid != uidBy
				//20131107 Ralf last_breath_time -> last_attacked_time : fix for online attacking bugs
						&& IsBeingAttacked(user.last_breath_time))
				{
					//debug_log("[load view be attacked,uid=%u,userid=%u,last_breath_time=%u]",uid,uidBy,user.last_breath_time);
					LOGIC_ERROR_RETURN_MSG("being_attacked");
				}
				if (ballid != 0 && Time::GetGlobalTime() - ballts < 5 * 60)
				{
					permit = APT_BAN_OTHER;
				}
				else
				{ //设置进攻许可
					CLogicMap logicMap;
					ret = logicMap.GetAttackPermit(user, userBy, permit);
					if (ret != 0)
						return ret;
				}
				CLogicTribute logicTribute;
				bool enable = false;
				logicTribute.IsEnableTribute(user, uidBy, enable);
				result["enable_tribute"] = enable?1:0;
			}
			else
			{
				if (IsOnlineUser(dataBaseExtra.last_breath_time) && dataBaseExtra.uid == dataBaseExtra.last_breath_uid)
				{
					permit = APT_BAN_ONLINE;
					result["online"] = 1;
				}
				if (BEING_ATTACK == dataBaseExtra.being_attack_flag && IsBeingAttacked(dataBaseExtra.last_breath_time) && uidBy != dataBaseExtra.last_breath_uid)
				{
					permit = APT_BAN_BEING_ATTACK;
				}
				if(Time::GetGlobalTime() < dataBaseExtra.protected_time)
				{
					permit = APT_BAN_WORLD_PROTECT;
				}
				result["enable_tribute"] = 0;
			}
			result["attackpermitted"] = permit;
		}
		else
		{
			if(isMainPos)
			{
				if (CLogicUser::GetFlag(user.bit_info, UF_BE_ATTACKED)
				//20131107 Ralf last_save_uid -> last_load_save_uid : fix for bugs, can save only last load archieve
						//&& user.last_save_uid != uidBy
				//20131107 Ralf last_breath_time -> last_attacked_time : fix for online attacking bugs
						&& IsBeingAttacked(user.last_breath_time))
				{
					//debug_log("[load other be attacked,uid=%u,userid=%u,last_breath_time=%u]",uid,uidBy,user.last_breath_time);
					LOGIC_ERROR_RETURN_MSG("being_attacked");
				}
			}
		}
	}
	if (isMainPos) {
		if (loadType != LT_ATTACK
				&& CLogicUser::GetFlag(user.bit_info, UF_BE_ATTACKED))	//取消进攻状态
						{
			CLogicUser::SetFlag(user.bit_info, UF_BE_ATTACKED, false);
			ret = logicUser.UpdateBitInfo(uid, user.bit_info);
			if (ret != 0)
				return ret;
		}
		if(!user.glory.empty() && !reader.parse(user.glory, result["md"]))
		{
			error_log("[user.glory parse fail][uid=%u]",uid);
			DATA_ERROR_RETURN_MSG("parse_user.glory_fail");
		}

		//alliance
		if (IsAllianceId(user.alliance_id)) {
			Json::Value &allianceData = result["alliance"];
			allianceData["id"] = alliance.alliance_id;
			allianceData["name"] = alliance.name;
			allianceData["flag"] = alliance.flag;
			Json::FromString(allianceData["data"], alliance.extra_data);
			Json::Value &memberData = allianceData["member"];
			memberData["type"] = allianceMember.type;
			memberData["point"] = Convert::UInt64ToString(allianceMember.point);
			Json::FromString(memberData["data"], allianceMember.extra_data);
		}
	}

	// 分基地load
	if (IsValidWorldPos(world_pos)) {
		logicWorld.GetUserInfo(world_pos, worldPoint);
		result["world_pos"]["type"] = worldPoint.map_flag;
		result["world_pos"]["world_pos"] = world_pos;
	} else {
		result["world_pos"]["type"] = USER_MAIN_CITY;
		result["world_pos"]["world_pos"] = 0;
	}
	// 分基地资源 需求更改 ，暂不用
//	CLogicBaseExtra logicBaseExtra;
//	ret = logicBaseExtra.GetSubBaseInfoJson(uid,result["worldres"]);
//	if(IsValidWorldPos(user.mainpos))
//	{
//		unsigned size = result["worldres"].size();
//		result["worldres"].resize(size + 1);
//		result["worldres"][size]["world_pos"] = user.mainpos;
//		if(user.worldres.size() > 2)
//		{
//			reader.parse(user.worldres,result["worldres"][size]["m"]);
//		}
//	}
	//下面为共同load部分
	result["userid"] = uid;

	//user basic
	result["name"] = userBasic.name;
	result["pic"] = userBasic.figure_url;
	result["viptype"] = userBasic.vip_type;
	result["viplevel"] = userBasic.vip_level;
	int level_3366 = 1;
	string level_name_3366;
	int grow_level_3366 = 1;
	int grow_value_3366 = 1;
	if (!userBasic.extra.empty())
	{
		Json::Value extraJson;
		if (reader.parse(userBasic.extra, extraJson))
			{
				Json::GetInt(extraJson, "3366_level", level_3366);
				Json::GetString(extraJson, "3366_level_name", level_name_3366);
				Json::GetInt(extraJson, "3366_grow_level", grow_level_3366);
				Json::GetInt(extraJson, "3366_grow_value", grow_value_3366);
			}
		else
			error_log("[userBasic.extra parse fail][uid=%u]",uid);
	}
	result["level_3366"] = level_3366;
	result["level_name_3366"] = level_name_3366;
	result["grow_level_3366"] = grow_level_3366;
	result["grow_value_3366"] = grow_value_3366;
	result["pt"] = userBasic.platform;
	//user
	result["kingdom"] = user.kingdom;
	result["sstate"] = user.sstate;
	result["register_time"] = user.register_time;
	result["invite_count"] = user.invite_count;
	result["today_invite_count"] = user.today_invite_count;
	result["level"] = user.level;
	result["exp"] = Convert::UInt64ToString(user.point);
	result["tutorialstage"] = user.tutorial_stage;
	result["resources"].resize(5);
	result["resources"][(unsigned) 0]["c"] = user.r1;
	result["resources"][(unsigned) 0]["m"] = user.r1_max;
	result["resources"][(unsigned) 1]["c"] = user.r2;
	result["resources"][(unsigned) 1]["m"] = user.r2_max;
	result["resources"][(unsigned) 2]["c"] = user.r3;
	result["resources"][(unsigned) 2]["m"] = user.r3_max;
	result["resources"][(unsigned) 3]["c"] = user.r4;
	result["resources"][(unsigned) 3]["m"] = user.r4_max;
	result["resources"][(unsigned) 4]["c"] = user.r5;
	result["resources"][(unsigned) 4]["m"] = user.r5_max;
	result["currencys"][(unsigned) 0] = user.gcbase;
	result["currencys"][(unsigned) 1] = user.gcbuy;
	result["currencys"][(unsigned) 2] = user.prosper;
	result["currencys"][(unsigned) 3] = user.battle_spirits;
	result["lasttime"] = user.last_save_time;
	result["newgcbase"] = user.newgcbase;
	result["protected_time"] = user.protected_time;
	result["lastseq"] = user.lastseq;
	result["game_vip_level"] = user.viplevel;
	if (isMainPos) {
		if (!user.barrackQ.empty()
				&& !reader.parse(user.barrackQ, result["barrackQ"])) {
			error_log("[barrackQ parse error][uid=%u]", uid);
			DATA_ERROR_RETURN_MSG("parse_barrackQ_fail");
		}
		if (!user.buildQ.empty()
				&& !reader.parse(user.buildQ, result["buildQ"])) {
			error_log("[buildQ parse error][uid=%u]", uid);
			DATA_ERROR_RETURN_MSG("parse_buildQ_fail");
		}
		if (!user.skillQ.empty()
				&& !reader.parse(user.skillQ, result["skillQ"])) {
			error_log("[skillQ parse error][uid=%u]", uid);
			DATA_ERROR_RETURN_MSG("parse_skillQ_fail");
		}
		if (!user.trainQ.empty()
				&& !reader.parse(user.trainQ, result["trainQ"])) {
			error_log("[trainQ parse error][uid=%u]", uid);
			DATA_ERROR_RETURN_MSG("parse_trainQ_fail");
		}
	}
	if (!user.vipstat.empty()
			&& !reader.parse(user.vipstat, result["game_vip_state"])) {
		error_log("[user game_vip_state parse error][uid=%u]", uid);
		DATA_ERROR_RETURN_MSG("parse_game_vip_state_fail");
	}

	if (!user.user_tech.empty()
			&& !reader.parse(user.user_tech, result["tech"])) {
		error_log("[user user_tech parse error][uid=%u]", uid);
		DATA_ERROR_RETURN_MSG("parse_user_tech_fail");
	}

	if (!user.soldier.empty()
			&& !reader.parse(user.soldier, result["soldier"])) {
		error_log("[soldier parse error][uid=%u]", uid);
		DATA_ERROR_RETURN_MSG("parse_soldier_fail");
	}
	if (!user.soldierlevel.empty()
			&& !reader.parse(user.soldierlevel, result["soldierlevel"])) {
		error_log("[soldierlevel parse error][uid=%u]", uid);
		DATA_ERROR_RETURN_MSG("parse_soldierlevel_fail");
	}
	if (!user.wf_status.empty()
			&& !reader.parse(user.wf_status, result["wf_status"])) {
		error_log("[wf_status parse error][uid=%u]", uid);
		DATA_ERROR_RETURN_MSG("parse_wf_status_fail");
	}

	if (user.alliance_tech.empty()) {
		string allianceTech("{\"techLv\":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]}");
		if (!reader.parse(allianceTech, result["alliance_tech"])) {
			error_log("[user alliance_tech parse error][uid=%u]", uid);
			DATA_ERROR_RETURN_MSG("parse_alliance_tech_fail");
		}
	} else {
		if (!reader.parse(user.alliance_tech, result["alliance_tech"])) {
			error_log("[user alliance_tech parse error][uid=%u]", uid);
			DATA_ERROR_RETURN_MSG("parse_alliance_tech_fail");
		}
	}
	CLogicUpdates logicUpdates;
	ret = logicUpdates.GetUpdates(uid, result["updates"]["data"], uid != uidBy);
	if (ret != 0)
		return ret;

	//building
	CLogicBuilding logicBuiding;

	ret = logicBuiding.GetBuilding(uid, world_pos, result["baseop"], isMainPos);
	if (ret != 0)
		return ret;

	//hero
	CLogicHero logicHero;
	ret = logicHero.GetHero(uid, result["hero"]);
	if (ret != 0)
		return ret;

	//equipment
	CLogicEquipment logicEquip;
	ret = logicEquip.GetEquipment(uid, result["equipment"],
			(uid == uidBy ? true : false));
	if (ret != 0)
		return ret;

	result["npc"] = 0;
	result["friend"] = isFriend ? 1 : 0;
	result["coins"] = payData.coins;	//金币
	result["coins2"] = payData.cash;	//钻石

	logicUser.UpdateActiveExtBreathTime(uidBy, userBy.last_active_time,
			userBy.ext);
	//20131107 Ralf last_breath_time -> last_attacked_time : fix for online attacking bugs
	/*
	if (uid != uidBy) {
		logicUser.UpdateBreathTime(uid);
	}
	*/
	return 0;
}

int CLogicArchive::Save(unsigned uid, unsigned uidBy, const string &type,
		Json::Value &data, Json::Value &result) {
	CLogicTopTenUser logicTopTenUser;
	int ret = 0;
	Json::Reader reader;
	Json::FastWriter writer;
	CLogicBaseExtra logicBaseExtra;
	DataPay payData;
	CLogicPay logicPay;
	DataUser userBy;
	CLogicUser logicUser;

	ret = logicUser.GetUser(uidBy, userBy);
	if (ret != 0)
		return ret;
	logicUser.UpdateActiveExtBreathTime(uidBy, userBy.last_active_time,
			userBy.ext);
	//20131107 Ralf last_breath_time -> last_attacked_time : fix for online attacking bugs
	/*
	if (uid != uidBy && IsValidUid(uid)) {
		logicUser.UpdateBreathTime(uid);
	}
	*/

	ret = logicPay.GetPay(uidBy, payData);
	if (ret != 0)
		return ret;

	if (!IsValidUid(uid))
	{
		ret = ProcessOrders(uidBy, data, payData,false);
		if (ret != 0)
			return ret;

		if (IsNPCUid(uid)) {
			CLogicNpc logicNpc;
			ret = logicNpc.Save(uid, userBy, data, result);
			if (ret != 0)
				return ret;
			//result["lasttime"] = Time::GetGlobalTime();
			return 0;
		}
		if (IsValidArenaId(uid)) {
			CLogicArena logicArena;
			ret = logicArena.Save(uid, userBy, data, result);
			if (ret != 0)
				return ret;
			return 0;
		}
		if (IsValidInstanceId(uid)) {
			CLogicInstance logicInstance;
			ret = logicInstance.Save(uid, userBy, data, result);
			if (ret != 0)
				return ret;
			return 0;
		}
		if (IsValidFightingId(uid)) {
			CLogicFighting logicFighting;
			ret = logicFighting.Save(uid, userBy, data, result);
			if (ret != 0)
				return ret;
			return 0;
		}
		if (IsValidFightArenaId(uid)) {
			CLogicFightArena logicFight;
			ret = logicFight.Save(uid, userBy, data, result);
			if (ret != 0)
				return ret;
			return 0;
		}
		if (IsValidWorldBossId(uid)) {
			CLogicWorldBoss logicWorldBoss;
			ret = logicWorldBoss.Save(uid, userBy, data, result);
			if (ret != 0)
				return ret;
			return 0;
		}
		//allianceBoss
		if (IsValidAllianceBossId(uid)) {
				CLogicAllianceBoss logicAllianceBoss;
				ret = logicAllianceBoss.Save(uid, userBy, data, result);
				if (ret != 0)
					return ret;
				return 0;
			}
		if (IsValidBMatchInstId(uid)) {
			CLogicBaseMatch logicBaseMatch;
			ret = logicBaseMatch.Save(uid, userBy, data, result);
			if (ret != 0)
				return ret;
			return 0;
		}
		if (IsValidAllServerBMatchInstId(uid)) {
			CLogicBaseMatch logicBaseMatch;
			ret = logicBaseMatch.Save(uid, userBy, data, result, true);
			if (ret != 0)
				return ret;
			return 0;
		}
		if (IsValidPersonMatchId(uid)) {
			CLogicBaseMatch logicBaseMatch;
			ret = logicBaseMatch.Save(uid, userBy, data, result);
			if (ret != 0)
				return ret;
			return 0;
		}
		if (IsValidAllServerPersonMatchId(uid)) {
			CLogicBaseMatch logicBaseMatch;
			ret = logicBaseMatch.Save(uid, userBy, data, result, true);
			if (ret != 0)
				return ret;
			return 0;
		}

		REFUSE_RETURN_MSG("invalid_uid");
	}

	DataUser user;
	if (uid == uidBy) {
		user = userBy;
	} else {
		ret = logicUser.GetUser(uid, user);
		if (ret != 0)
			return ret;
	}
	// add by aaron 20121030
	bool isMainPos = true;
	unsigned world_pos;
	if (!Json::GetUInt(data, "world_pos", world_pos) || world_pos == 0) {
		world_pos = user.mainpos;
	}
	// 默认主城坐标
	isMainPos = ((world_pos == 0 || world_pos == user.mainpos) ? true : false);
	// uid == uidBy
	if (isMainPos)
	{
		unsigned lasttime = 0;
		unsigned seqid = 0;
		Json::GetUInt(data, "seqid", seqid);
		Json::GetUInt(data, "lastime", lasttime);
		/*
		if (!Json::GetUInt(data, "seqid", seqid))
		{
			error_log(
					"[lasttime_mismatch seqid mis][uid=%u,userid=%u,lasttime=%u,last_save_time=%u,seqid=%u,lastseq=%u]",
					uidBy, uid, lasttime, user.last_save_time, seqid, user.lastseq);
			LOGIC_ERROR_RETURN_MSG("lasttime_mismatch");
		}
		if (!Json::GetUInt(data, "lastime", lasttime))
		{
			error_log(
					"[lasttime_mismatch lastime mis][uid=%u,userid=%u,lasttime=%u,last_save_time=%u,seqid=%u,lastseq=%u]",
					uidBy, uid, lasttime, user.last_save_time, seqid, user.lastseq);
			LOGIC_ERROR_RETURN_MSG("lasttime_mismatch");
		}
		*/
		if (lasttime != user.last_save_time || seqid != user.lastseq + 1)
		{
			result["lasttime"] = user.last_save_time;
			result["lastseq"] = user.lastseq;
			result["saveuserid"] = uid;

			unsigned retry = 0;
			if(Json::GetUInt(data, "retry", retry) && retry)
			{
				error_log("[retry_mismatch][uid=%u,userid=%u,lasttime=%u,last_save_time=%u,seqid=%u,lastseq=%u]",
						uidBy, uid, lasttime, user.last_save_time, seqid, user.lastseq);
				return 0;
			}

			error_log(
					"[lasttime_mismatch][uid=%u,userid=%u,lasttime=%u,last_save_time=%u,seqid=%u,lastseq=%u]",
					uidBy, uid, lasttime, user.last_save_time, seqid, user.lastseq);
			//LOGIC_ERROR_RETURN_MSG("lasttime_mismatch");
			return 0;
		}
		//20131107 Ralf last_save_uid -> last_load_save_uid : fix for bugs, can save only last load archieve
		if(user.last_save_uid != uidBy || userBy.last_save_uid != uid)
		{
			error_log("[last_save_uid_mismatch][uid=%u,userid=%u,userBy_last_save_uid=%u,user_last_save_uid=%u]",
					uidBy, uid, userBy.last_save_uid, user.last_save_uid);
			//debug_log("[last_save_uid_mismatch][uid=%u,userid=%u,userBy_last_save_uid=%u,user_last_save_uid=%u]",	uidBy, uid, lasttime, userBy.last_save_uid, user.last_save_uid);
			return 0;
		}

		//差异部分
		int damage = 0;
		if (uid != uidBy)
		{
			ret = ProcessOrders(uidBy, data, payData,false);
			if (ret != 0)
				return ret;

			ret = UpdateAttack(userBy, user, data);
			//龙珠活动
			CLogicDragonball logicBall;
			if (logicBall.IsDragonballActivityTs())
			{
				int destroy = 0;
				if (Json::GetInt(data, "destroy", destroy) && destroy == 1)
				{
					unsigned ballId = 0;
					unsigned ballts = 0;
					if (logicBall.WhichDragonballHold(uid, ballId, ballts) == 0
							&& IsValidDragonballId(ballId)
							&& logicBall.EnableAttackForDragonball(ballId,
									userBy.level))
					{
						DataUserBasic userBasicBy;
						CLogicUserBasic logicUserBasic;
						logicUserBasic.GetUserBasicLimit(uidBy,
								OpenPlatform::GetType(), userBasicBy);
						logicBall.SnatchDragonball(ballId, uidBy,
								userBasicBy.name);
					}
				}
			}
			if (Json::GetInt(data, "damage", damage))	//	受损保护
					{
				//联盟得分
				if (damage > 100)
					damage = 100;
				//联盟得分
				unsigned protectedTime = 0;
				ret = SetProtectedTime(user.level, damage, protectedTime);
				if (0 == ret && protectedTime > user.protected_time) {
					user.protected_time = protectedTime;
				}
				int attackend = 0;
				if (Json::GetInt(data, "attackend", attackend)
						&& attackend == 1) {

					if (IsAllianceId(userBy.alliance_id)
							&& userBy.alliance_id != user.alliance_id) {
						CLogicAlliance logicAlliance;
						ret = logicAlliance.AddAttackPoint(userBy.alliance_id,
								uidBy, data, userBy.level);
					}
					CLogicUser::SetFlag(user.bit_info, UF_BE_ATTACKED, false);
					ret = logicUser.UpdateBitInfo(uid, user.bit_info);
					if (ret != 0)
						return ret;
					logicTopTenUser.SetUserPlunder(userBy.uid);
				}
			}
			//20131107 Ralf last_breath_time -> last_attacked_time : fix for online attacking bugs
			user.last_breath_time = Time::GetGlobalTime();
			//debug_log("[save attack,uid=%u,userid=%u,last_breath_time=%u]",uid,uidBy,user.last_breath_time);
		}
		else
		{
			ret = ProcessOrders(uidBy, data, payData,true);
			if (ret != 0)
				return ret;

			if (data.isMember("amicablenpcid")) {
				CLogicInstance dbInstance;
				unsigned npcid = data["amicablenpcid"].asUInt();
				ret = dbInstance.RemoveInstance(uid, npcid);
				if (0 != ret) {
					error_log("amicable npc failed ,uid = %u,npcid = %u", uid,
							npcid);
					return ret;
				}
			}
			//自己保存自己
			if (Json::IsArray(data, "secinc")) {
				CLogicSecinc logicSecinc;
				ret = logicSecinc.ProcessSecinc(uid, data["secinc"]);
				if (ret != 0)
					return ret;
			}
			//朝贡
			if (Json::IsArray(data, "tributes")) {
				CLogicTribute logicTribute;
				ret = logicTribute.ProcessTribute(uid, data["tributes"]);
				if (ret != 0)
					return ret;
			}
			CLogicUserData logicUserData;
			if (data.isMember("npcattack")) {
				ret = logicUserData.SetNpcattack(uid, data["npcattack"]);
				if (ret != 0)
					return ret;
			}
			if (data.isMember("gift")) {
				ret = logicUserData.SetGift(uid, data["gift"]);
				if (ret != 0)
					return ret;
			}
			if (data.isMember("dailyquest")) {
				ret = logicUserData.SetDailyQuest(uid, data["dailyquest"]);
				if (ret != 0)
					return ret;
			}
			//quest
			if (Json::IsArray(data, "quest")) {
				CLogicQuest logicQuest;
				ret = logicQuest.UpdateQuest(uid, data["quest"]);
				if (ret != 0)
					return ret;
			}
			//gate
			if (Json::IsArray(data, "gate")) {
				CLogicGate logicGate;
				ret = logicGate.UpdateGate(uid, data["gate"]);
				if (ret != 0)
					return ret;
			}
			if (Json::IsArray(data, "wf_status")) {
				user.wf_status = writer.write(data["wf_status"]);
			}
			//add by aaron for barbarian 20130110
			int barbarianPoint = 0;
			if (Json::GetInt(data, "barbarianpoint",
					barbarianPoint) && barbarianPoint != 0 && IsAllianceId(user.alliance_id)) {static
				CLogicBarbarianInvasion logicBarbarianInvasion;
				DataPlayerItem playerItem;
				memset(&playerItem, 0x00, sizeof(DataPlayerItem));
				CLogicUserBasic logicUserBasic;
				DataUserBasic userBasic;
				logicUserBasic.GetUserBasicLimit(uid, OpenPlatform::GetType(),
						userBasic);
				playerItem.uid = uid;
				playerItem.points = barbarianPoint;
				memcpy(playerItem.name, userBasic.name.c_str(),
						sizeof(playerItem.name) - 1);
				logicBarbarianInvasion.UpdateRank(playerItem, user.alliance_id);
				DataPlayersPoints playersRank;
				memset(&playersRank, 0x00, sizeof(DataPlayersPoints));
				DataTeamItem teamRank[512];
				memset(teamRank, 0x00, sizeof(teamRank));
				int teamNum = 0;
				int i = 0;
				logicBarbarianInvasion.GetTeamPlayersRank(playersRank,
						user.alliance_id);
				logicBarbarianInvasion.GetTeamRank(teamRank, teamNum);
				for (i = 0; i < playersRank.playerNum; i++) {
					result["barbairanplayersrank"][i]["uid"] =
							playersRank.players[i].uid;
					result["barbairanplayersrank"][i]["points"] =
							playersRank.players[i].points;
					result["barbairanplayersrank"][i]["name"] =
							playersRank.players[i].name;
				}
				for (i = 0; i < teamNum; i++) {
					result["barbairanteamrank"][i]["aid"] = teamRank[i].aid;
					result["barbairanteamrank"][i]["teampoints"] =
							teamRank[i].teamPoints;
					result["barbairanteamrank"][i]["name"] = teamRank[i].aname;
				}
			}
			// add by aaron 20130130
			Json::Value userFlag;
			ret = logicUser.GetUserFlag(uid,userFlag);
			result["paytotal"] = 0;
			result["pointpay"] = 0;
			if (ret == 0)
			{
				user.user_flag = writer.write(userFlag);

				if (userFlag.isMember("payTotal"))
				{
					int lastPay_ts = userFlag["payTotal"][1u].asUInt();
					if (lastPay_ts>= Config::GetIntValue(CONFIG_PAY_TOTAL_BEGIN_TS)&& lastPay_ts<= Config::GetIntValue(CONFIG_PAY_TOTAL_END_TS))
					{
						result["paytotal"] = userFlag["payTotal"][0u];
					}
				}
				if (userFlag.isMember("pointpay"))
				{
					int lastPay_ts = userFlag["pointpay"][1u].asUInt();
					if (lastPay_ts
							>= Config::GetIntValue(CONFIG_PAYRANK_BEGIN_TS)
							&& lastPay_ts
									<= Config::GetIntValue(
											CONFIG_PAYRANK_END_TS)) {
						result["pointpay"] = userFlag["pointpay"][0u];
					}
				}

				if(!userFlag.isMember("heroCoins"))   //用于购买武将碎片
				{
					result["coins3"] = 0;
					result["todaycharge"] = 0;
				}
				else
				{
					result["coins3"] = userFlag["heroCoins"][1u].asInt();
					unsigned last_ts = userFlag["heroCoins"][0u].asUInt();
					unsigned now = time(0);
					if(CTime::GetDayInterval(last_ts,now) == 0)
					{
						result["todaycharge"] = userFlag["heroCoins"][2u].asInt();
					}
					else
					{
						result["todaycharge"] = 0;
					}
				}
			}

			//add by aaron
			if (Config::GetIntValue("new_flag") == 1) // 判断是否为混战三国  1：混战三国 其他:非混战三国
					{
				int count = 0;
				uint64_t seq = 0;
				CLogicBuyGoods logicBuyGoods;
				CDataGoods datagoods;
				int dbCount = 0;
				if (data["buy_goods"].isArray()
						&& data["buy_goods"].size() > 0) {
					int iCount = data["buy_goods"].size();
					for (int i = 0; i < iCount; i++) {
						seq = atol(
								data["buy_goods"][i]["seq"].asString().c_str());
						Json::GetInt(data["buy_goods"][i], "count", count);
						datagoods.GetCountById(uid, seq, dbCount);
						ret = logicBuyGoods.UpdateGoods(uid, seq,
								dbCount - count);
						if (0 != ret) {
							error_log(
									"update buy equipments failed ,uid=%u,seq=%ld,count=%d",
									uid, seq, count);
						} else {
							info_log(
									"update buy equipments success ,uid=%u,seq=%ld,count=%d",
									uid, seq, count);
						}
					}
				}
			}
			// add by aaron

			//自己保存自己才存stat
			if (data.isMember("stats"))
			{
				Json::Value old;
				reader.parse(user.user_stat, old);
				ret = checkUserStats(old,data["stats"]);
				if(ret != 0)
				{
					error_log("[user_stat error][uid=%u,userid=%u,ret=%u]", uidBy,uid,ret);
					LOGIC_ERROR_RETURN_MSG("user_stat error");
				}

				if (data["stats"].isMember("dehp")) {
					int dehp = 0;
					Json::GetInt(data["stats"], "dehp", dehp);
					logicTopTenUser.SetHeroPower(user.uid, dehp, user.user_stat);
				}
				user.user_stat = writer.write(data["stats"]);
			}
		}
		//共同部分
		// user
		int r1chg = 0, r2chg = 0, r3chg = 0, r4chg = 0, r5chg = 0;
		int gcbaschg = 0, gcbuychg = 0, addprosper = 0, addbs = 0, itemp = 0;
		int qleold = -1, qlechg = 0, qleres = -1;
		uint64_t exp = 0;

		user.last_save_time = Time::GetGlobalTime();
		if (data.isMember("tech")) {
			Json::Value OldUserTech;
			reader.parse(user.user_tech, OldUserTech);

			ret = checkUserTech(OldUserTech,data["tech"]);
			if(ret != 0)
			{
				error_log("[user_tech error][uid=%u,userid=%u,ret=%u]", uidBy,uid,ret);
				LOGIC_ERROR_RETURN_MSG("user_tech error");
			}

			if (data["tech"]["godE"].isObject()) {
				if (data["tech"]["godE"]["qlE"].isArray()) {
					if (OldUserTech["godE"]["qlE"].isArray()) {
						qleold = OldUserTech["godE"]["qlE"][0u].asInt();
						qlechg = data["tech"]["godE"]["qlE"][0u].asInt();
						qleres = qlechg + qleold;
						if(qleres < 0){
							error_log("[resource_error][uid=%u,userid=%u]", uidBy, uid);
							LOGIC_ERROR_RETURN_MSG("resource_error");
						}
						data["tech"]["godE"]["qlE"][0u] = qleres;
					}
				}
			}
			user.user_tech = writer.write(data["tech"]);
		}
		if (data.isMember("barrackQ")) {
			user.barrackQ = writer.write(data["barrackQ"]);
		}
		if (data.isMember("soldier")) {
			user.soldier = writer.write(data["soldier"]);
		}
		if (data.isMember("soldierlevel")) {
			user.soldierlevel = writer.write(data["soldierlevel"]);
		}
		if (data.isMember("buildQ")) {
			user.buildQ = writer.write(data["buildQ"]);
		}
		if (data.isMember("skillQ")) {
			user.skillQ = writer.write(data["skillQ"]);
		}
		if (data.isMember("trainQ")) {
			user.trainQ = writer.write(data["trainQ"]);
		}
		if (data.isMember("game_vip_state")) {
			user.vipstat = writer.write(data["game_vip_state"]);
		}
		if (data.isMember("godfb")) {
			CLogicWeapon logicWeapon;
			string godfb = writer.write(data["godfb"]);
			logicWeapon.SetWeapon(user.uid, godfb);
		}

		if(!user.kingdom)
			Json::GetUInt(data, "kingdom", user.kingdom);
		Json::GetInt(data, "tutorialstage", user.tutorial_stage);

		int leveltemp = 0;
		Json::GetInt(data, "level", leveltemp);
		if(user.level > 50 && leveltemp > user.level + 1)
		{
			error_log("[level_error][uid=%u,userid=%u]", uidBy, uid);
			LOGIC_ERROR_RETURN_MSG("level_error");
		}
		else if(leveltemp != user.level)
		{
			user.level =  leveltemp;
			logicTopTenUser.SetUserLevel(user.uid, user.level);
		}

		Json::GetUInt64(data, "exp", exp);
		user.point += exp;
		Json::GetInt(data, "damage", user.damage);
		Json::GetUInt(data, "seqid", user.lastseq);
		//20131107 Ralf last_breath_time -> last_attacked_time : fix for online attacking bugs
		//user.last_breath_time = Time::GetGlobalTime();
		//20131107 Ralf last_save_uid -> last_load_save_uid : fix for bugs, can save only last load archieve
		//user.last_save_uid = uidBy;
		Json::GetUInt(data, "newgcbase", user.newgcbase);
		if (Json::IsArray(data, "currencys")) {
			if (data["currencys"].size() > 0) {
				if (Json::GetInt(data["currencys"], (unsigned) 0, gcbaschg)
						&& gcbaschg != 0) {
					itemp = gcbaschg + user.gcbase;
					if (itemp >= 0)
						user.gcbase += gcbaschg;
					else if(uid != uidBy)
					{
						user.gcbase = 0;
						error_log("[resource_error][uid=%u,userid=%u,gcbaschg=%d]", uidBy, uid, gcbaschg);
					}
					else {
						error_log("[resource_error][uid=%u,userid=%u]", uidBy,
								uid);
						LOGIC_ERROR_RETURN_MSG("resource_error");
					}
				}
			}
			if (data["currencys"].size() > 1) {
				if (Json::GetInt(data["currencys"], (unsigned) 1, gcbuychg)
						&& gcbuychg != 0) {
					itemp = gcbuychg + user.gcbuy;
					if (itemp >= 0)
						user.gcbuy += gcbuychg;
					else if(uid != uidBy)
					{
						user.gcbuy = 0;
						error_log("[resource_error][uid=%u,userid=%u,gcbuychg=%d]", uidBy, uid, gcbuychg);
					}
					else {
						error_log("[resource_error][uid=%u,userid=%u]", uidBy, uid);
						LOGIC_ERROR_RETURN_MSG("resource_error");
					}
				}
			}
			if (data["currencys"].size() > 2) {
				if (Json::GetInt(data["currencys"], (unsigned) 2, addprosper)
						&& addprosper != 0) {
					itemp = addprosper + user.prosper;
					if (itemp >= 0)
						user.prosper += addprosper;
					else if(uid != uidBy)
					{
						user.prosper = 0;
						error_log("[resource_error][uid=%u,userid=%u,addprosper=%d]", uidBy, uid, addprosper);
					}
					else {
						error_log("[resource_error][uid=%u,userid=%u]", uidBy, uid);
						LOGIC_ERROR_RETURN_MSG("resource_error");
					}
				}
			}
			if (data["currencys"].size() > 3) {
				if (Json::GetInt(data["currencys"], (unsigned) 3, addbs)
						&& addbs != 0) {
					itemp = addbs + user.battle_spirits;
					if (itemp >= 0)
						user.battle_spirits += addbs;
					else if(uid != uidBy)
					{
						user.battle_spirits = 0;
						error_log("[resource_error][uid=%u,userid=%u,addbs=%d]", uidBy, uid, addbs);
					}
					else {
						error_log("[resource_error][uid=%u,userid=%u]", uidBy, uid);
						LOGIC_ERROR_RETURN_MSG("resource_error");
					}
				}
			}
		}
		if (Json::IsArray(data, "resources")) {
			if (data["resources"].size() > 0) {
				if (Json::GetInt(data["resources"][(unsigned) 0], "d", r1chg)) {
					itemp = r1chg + user.r1;
					if (itemp >= 0)
						user.r1 += r1chg;
					else if(uid != uidBy)
					{
						user.r1 = 0;
						error_log("[resource_error][uid=%u,userid=%u,r1chg=%d]", uidBy, uid, r1chg);
					}
					else {
						error_log("[resource_error][uid=%u,userid=%u]", uidBy,
								uid);
						LOGIC_ERROR_RETURN_MSG("resource_error");
					}
				}
				Json::GetUInt(data["resources"][(unsigned) 0], "m",
						user.r1_max);
			}
			if (data["resources"].size() > 1) {
				if (Json::GetInt(data["resources"][(unsigned) 1], "d", r2chg)) {
					itemp = r2chg + user.r2;
					if (itemp >= 0)
						user.r2 += r2chg;
					else if(uid != uidBy)
					{
						user.r2 = 0;
						error_log("[resource_error][uid=%u,userid=%u,r2chg=%d]", uidBy, uid, r2chg);
					}
					else {
						error_log("[resource_error][uid=%u,userid=%u]", uidBy, uid);
						LOGIC_ERROR_RETURN_MSG("resource_error");
					}
				}
				Json::GetUInt(data["resources"][(unsigned) 1], "m",
						user.r2_max);
			}
			if (data["resources"].size() > 2) {
				if (Json::GetInt(data["resources"][(unsigned) 2], "d", r3chg)) {
					itemp = r3chg + user.r3;
					if (itemp >= 0)
						user.r3 += r3chg;
					else if(uid != uidBy)
					{
						user.r3 = 0;
						error_log("[resource_error][uid=%u,userid=%u,r3chg=%d]", uidBy, uid, r3chg);
					}
					else {
						error_log("[resource_error][uid=%u,userid=%u]", uidBy, uid);
						LOGIC_ERROR_RETURN_MSG("resource_error");
					}
				}
				Json::GetUInt(data["resources"][(unsigned) 2], "m",
						user.r3_max);
			}
			if (data["resources"].size() > 3) {
				if (Json::GetInt(data["resources"][(unsigned) 3], "d", r4chg)) {
					itemp = r4chg + user.r4;
					if (itemp >= 0)
						user.r4 += r4chg;
					else if(uid != uidBy)
					{
						user.r4 = 0;
						error_log("[resource_error][uid=%u,userid=%u,r4chg=%d]", uidBy, uid, r4chg);
					}
					else {
						error_log("[resource_error][uid=%u,userid=%u]", uidBy, uid);
						LOGIC_ERROR_RETURN_MSG("resource_error");
					}
				}
				Json::GetUInt(data["resources"][(unsigned) 3], "m",
						user.r4_max);
			}
			if (data["resources"].size() > 4) {
				if (Json::GetInt(data["resources"][(unsigned) 4], "d", r5chg)) {
					itemp = r5chg + user.r5;
					if (itemp >= 0)
						user.r5 += r5chg;
					else if(uid != uidBy)
					{
						user.r5 = 0;
						error_log("[resource_error][uid=%u,userid=%u,r5chg=%d]", uidBy, uid, r5chg);
					}
					else {
						error_log("[resource_error][uid=%u,userid=%u]", uidBy, uid);
						LOGIC_ERROR_RETURN_MSG("resource_error");
					}
				}
				Json::GetUInt(data["resources"][(unsigned) 4], "m",
						user.r5_max);
			}
		}

		// 保存用户状态信息
		int unsigned sstate;
		if (Json::GetUInt(data, "sstate", sstate)) {
			user.sstate = sstate;
		}
		if (data.isMember("md")) {
			user.glory = writer.write(data["md"]);
		}
		// 保存用户状态信息
		ret = logicUser.SetUser(uid, user);
		if (ret != 0)
			return ret;
		RESOURCE_LOG(
				"[save change][uid=%u,gcbaschg=%d,gcbase=%u,gcbuychg=%d,gcbuy=%u,r1chg=%d,r1=%u,r2chg=%d,r2=%u,r3chg=%d,r3=%u,r4chg=%d,r4=%u,r5chg=%d,r5=%u,addprosper=%d,prosper=%u,addbs=%d,bs=%u,qlechg=%d,qle=%d,expchg=%u,exp=%u,lvl=%u]",
				uid, gcbaschg, user.gcbase, gcbuychg, user.gcbuy, r1chg,
				user.r1, r2chg, user.r2, r3chg, user.r3, r4chg, user.r4, r5chg,
				user.r5, addprosper, user.prosper, addbs, user.battle_spirits,
				qlechg,qleres,exp,user.point,user.level);
		//移除帮助好友的记录
		unsigned lastts = 0;
		if (Json::IsObject(data, "updates")
				&& Json::GetUInt(data["updates"], "lastts", lastts)
				&& lastts > 0) {
			CLogicUpdates logicUpdates;
			ret = logicUpdates.RemoveUpdatesBeforeTs(uid, lastts);
			if (ret != 0)
				return ret;
		}
		result["lasttime"] = user.last_save_time;
		result["lastseq"] = user.lastseq;
		result["saveuserid"] = uid;
	}
	else
	{
		ret = ProcessOrders(uidBy, data, payData, false);
		if (ret != 0)
			return ret;
	}
// 暂时不用
//	if(data.isMember("worldres"))
//	{
//		if(!isMainPos)
//		{
//				// 设置分基地资源容量
//			string resource = writer.write(data["worldres"][0u]["m"]);
//			logicBaseExtra.SetSubBaseResource(uid,world_pos,resource);
//		}else
//		{
//			user.worldres = writer.write(data["worldres"][0u]["m"]);
//		}
//	}

	//building
	if (Json::IsArray(data, "baseop")) {
		CLogicBuilding logicBuilding;
		ret = logicBuilding.UpdateBuilding(uid, world_pos, data["baseop"],
				isMainPos);
		if (ret != 0)
			return ret;
		// 二级地图初始化主城
		if (isMainPos && user.mainpos == 0) {
			int iCount = data["baseop"].size();
			for (int i = 0; i < iCount; i++) {
				if (data["baseop"][i]["t"].asInt() == 1
						&& data["baseop"][i]["l"].asInt() >= 40) {
					CLogicWorld logicWorld;
					WorldPoint worldPoint;
					worldPoint.uid = uid;
					worldPoint.map_flag = USER_MAIN_CITY;
					ret = logicWorld.AddUser(worldPoint, user.mainpos);
					if (0 != ret) {
						error_log("add world user failed %u", uid);
						return ret;
					}
					ret = logicUser.SetMainpos(uid, user.mainpos);
					if (0 != ret) {
						error_log("set user limit failed %u", uid);
						return ret;
					}
					result["world_pos"]["world_pos"] = user.mainpos;
					break;
				}
			}
		}
	}

	//hero
	if (Json::IsArray(data, "hero")) {
		CLogicHero logicHero;
		ret = logicHero.UpdateHero(uid, data["hero"]);
		if (ret != 0)
			return ret;
	}
	if (Json::IsArray(data, "equipment")) {
		CLogicEquipment logicEquip;
		ret = logicEquip.UpdateEquipment(uid, data["equipment"]);
		if (ret != 0)
			return ret;
	}

	if (data.isMember("wnpcdrwd")) {
		Json::Value baseExtraData;
		DataBaseExtra baseExtra;
		int iCount = data["wnpcdrwd"].size();
		for (int i = 0; i < iCount; i++) {
			ret = logicBaseExtra.GetBaseExtra(uid, data["wnpcdrwd"][i].asUInt(),
					baseExtra);
			if (0 != ret) {
				error_log("Get baseExtra data failed worldpos = %u, uid = %u",
						baseExtra.worldpos, uid);
			}
			baseExtra.last_collect_ts = Time::GetGlobalTime();
			ret = logicBaseExtra.SetBaseExtra(uid, baseExtra.worldpos,
					baseExtra);
			if (0 != ret) {
				error_log("Set baseExtra data failed worldpos = %u, uid = %u",
						baseExtra.worldpos, uid);
			}
		}

	}
	// 更新攻击者保存分基地的时间。
	if (!isMainPos) {
		int attackend = 0;
		DataBreathInfo breathInfo;
		breathInfo.last_breath_time = Time::GetGlobalTime();
		breathInfo.last_breath_uid = uidBy;
		if (uid != uidBy) {
			ret = UpdateAttack(userBy, user, data);
			if (Json::GetInt(data, "attackend", attackend) && attackend == 1) {
				breathInfo.being_attack_flag = NO_ATTACK;
			} else {
				breathInfo.being_attack_flag = BEING_ATTACK;
			}
		} else {
			breathInfo.being_attack_flag = NO_ATTACK;
		}
		logicBaseExtra.UpdateBreathInfo(uid, world_pos, breathInfo);
	}
	result["coins"] = payData.coins;
	result["coins2"] = payData.cash;

	return 0;
}

/*
int CLogicArchive::UpdateSave(unsigned uid, unsigned uidBy,
		Json::Value &result) {
	int ret = 0;
	DataUser userBy;
	CLogicUser logicUser;
	ret = logicUser.GetUserLimit(uidBy, userBy);
	if (ret != 0)
		return ret;
	logicUser.UpdateActiveExtBreathTime(uidBy, userBy.last_active_time,
			userBy.ext);

	if (IsValidArenaId(uid)) {
		CLogicArena logicArena;
		logicArena.BreathArena(uid, uidBy);
		return 0;

	}
	if (IsValidFightArenaId(uid)) {
		CLogicFightArena logicArena;
		logicArena.BreathFightArena(uid, uidBy);
		return 0;

	}
	if (IsValidWorldBossId(uid)) {
		CLogicWorldBoss logicWorldBoss;
		ret = logicWorldBoss.ViewWorldBoss(uidBy, uid, result["worldboss"]);
		if (ret != 0)
			return ret;
		return 0;
	}
	//alliance
	if (IsValidAllianceBossId(uid)) {
			CLogicAllianceBoss logicAllianceBoss;
			ret = logicAllianceBoss.ViewAllianceBoss(uidBy, uid, result["allianceboss"]);
			if (ret != 0)
				return ret;
			return 0;
		}
	if (!IsValidUid(uid)) {
		return 0;
	}

	DataPay payData;
	CLogicPay logicPay;
	ret = logicPay.GetPay(uidBy, payData);
	if (ret != 0)
		return ret;

	DataUser user;
	if (uid == uidBy) {
		user.copyLimit(userBy);
		result["acccharge"] = user.accCharge;
		CLogicSecinc logicSecinc;
		ret = logicSecinc.GetSecinc(uid, result["secinc"]);
		if (ret != 0)
			return ret;
		Json::Value userFlag;
		ret = logicUser.GetUserFlag(uidBy, userFlag);
		if (ret != 0)
			return ret;

		if (!userFlag.isMember("dchg")
				|| CTime::GetDayInterval(userFlag["dchg"][0u].asUInt(),
						Time::GetGlobalTime()) != 0) {
			userFlag["dchg"][0u] = Time::GetGlobalTime();
			userFlag["dchg"][1u] = 0;
		}
		result["dailyacccharge"] = userFlag["dchg"][1u];
		//add by aaron
		error_log("buy_goods");
		info_log("buy_goods");
		if (Config::GetIntValue("new_flag") == 1) {
			error_log("buy_goods");
			info_log("buy_goods");
			CLogicBuyGoods logicBuyGoods;
			ret = logicBuyGoods.GetGoods(uid, result["buy_goods"]);
			if (0 != ret)
				error_log("buy_goods");
			info_log("buy_goods");

		}
		//add by aaron
	} else {
		ret = logicUser.GetUserLimit(uid, user);
		if (ret != 0)
			return ret;
		//20131107 Ralf last_breath_time -> last_attacked_time : fix for online attacking bugs
		logicUser.UpdateBreathTime(uid);
	}

	CLogicUpdates logicUpdates;
	ret = logicUpdates.GetUpdates(uid, result["updates"]["data"], uid != uidBy);
	if (ret != 0)
		return ret;

	CLogicBroadcast logicBd;
	vector<BroadcastMessage> bdMsgs;
	ret = logicBd.GetBdMessage(bdMsgs);
	if (ret == 0 && bdMsgs.size() > 0) {
		result["broadcast"].resize(bdMsgs.size());
		for (unsigned i = 0; i < bdMsgs.size(); i++) {
			result["broadcast"][i]["id"] = bdMsgs[i].id;
			result["broadcast"][i]["content"] = bdMsgs[i].content;
		}
	}
	// add by aaron 20121106
	// daily accharge
	error_log("buy_goods");
	Json::Value userFlag;
	Json::Reader reader;
	if (!user.user_flag.empty() && reader.parse(user.user_flag, userFlag)) {
		if (!userFlag.isMember("dchg")
				|| CTime::GetDayInterval(userFlag["dchg"][0u].asUInt(),
						Time::GetGlobalTime()) != 0) {
			userFlag["dchg"][0u] = Time::GetGlobalTime();
			userFlag["dchg"][1u] = 0;
		}
		result["dailyacccharge"] = userFlag["dchg"][1u];
	}

	// hero synchronization
	//hero
//	CLogicHero logicHero;
//	ret = logicHero.GetHero(uid, result["hero"]);
//	if (ret != 0)
//		return ret;
//	// 分基地容量变动
//	result["world_pos"]["world_pos"] = user.mainpos;
//	result["mainpos"] = user.mainpos;
//	CLogicBaseExtra logicBaseExtra;
//	ret = logicBaseExtra.GetSubBaseInfoJson(uid,result["worldres"]);
//	unsigned size = result["worldres"].size();
//	result["worldres"].resize(size + 1);
//	result["worldres"][size]["world_pos"] = user.mainpos;
//	result["worldres"][size]["m"][0u] = user.r1_max;
//	result["worldres"][size]["m"][1u] = user.r2_max;
//	result["worldres"][size]["m"][2u] = user.r3_max;
//	result["worldres"][size]["m"][3u] = user.r4_max;
//	result["worldres"][size]["m"][4u] = user.r5_max;
	// add by aaron 20121030
	result["coins"] = payData.coins;
	result["coins2"] = payData.cash;
	result["invite_count"] = user.invite_count;
	result["today_invite_count"] = user.today_invite_count;
	result["resources"].resize(5);
	result["resources"][(unsigned) 0]["c"] = user.r1;
	result["resources"][(unsigned) 1]["c"] = user.r2;
	result["resources"][(unsigned) 2]["c"] = user.r3;
	result["resources"][(unsigned) 3]["c"] = user.r4;
	result["resources"][(unsigned) 4]["c"] = user.r5;
	result["currencys"][(unsigned) 0] = user.gcbase;
	result["currencys"][(unsigned) 1] = user.gcbuy;
	result["currencys"][(unsigned) 2] = user.prosper;
	result["currencys"][(unsigned) 3] = user.battle_spirits;
	result["sstate"] = user.sstate;

	return 0;
}
*/

int CLogicArchive::UpdateSave(unsigned uid, unsigned uidBy, unsigned world_pos,
		Json::Value &result) {
	int ret = 0;
	DataUser userBy;
	CLogicUser logicUser;
	ret = logicUser.GetUserLimit(uidBy, userBy);
	if (ret != 0)
		return ret;

	logicUser.UpdateActiveExtBreathTime(uidBy, userBy.last_active_time,
			userBy.ext);

	if (IsValidArenaId(uid)) {
		CLogicArena logicArena;
		logicArena.BreathArena(uid, uidBy);
		return 0;

	}
	if (IsValidFightArenaId(uid)) {
		CLogicFightArena logicArena;
		logicArena.BreathFightArena(uid, uidBy);
		return 0;

	}
	if (IsValidWorldBossId(uid)) {
		CLogicWorldBoss logicWorldBoss;
		ret = logicWorldBoss.ViewWorldBoss(uidBy, uid, result["worldboss"]);
		if (ret != 0)
			return ret;
		return 0;
	}
	//AllianceBoss
	if (IsValidAllianceBossId(uid)) {
			CLogicAllianceBoss logicAllianceBoss;
			ret = logicAllianceBoss.ViewAllianceBoss(uidBy, uid, result["allianceboss"]);
			if (ret != 0)
				return ret;
			return 0;
		}
	if (!IsValidUid(uid)) {
		return 0;
	}

	DataPay payData;
	CLogicPay logicPay;
	ret = logicPay.GetPay(uidBy, payData);
	if (ret != 0)
		return ret;

	DataUser user;
	if (uid == uidBy) {
		if (userBy.tribute_time > Time::GetGlobalTime()) {
			string retStr = "being_blocked_"
					+ CTrans::ITOS(userBy.tribute_time - Time::GetGlobalTime());
			REFUSE_RETURN_MSG(retStr);
		}
		CLogicUserBasic logicUserBaisc;
		DataUserBasic userBasic;
		ret = logicUserBaisc.GetUserBasic(uid, OpenPlatform::GetType(), userBasic);
		if (0 == ret)
		{
			bool isBlackUser = false;
			logicUserBaisc.CheckBlackUser(userBasic.open_id,isBlackUser);
			if (isBlackUser)
			{
				REFUSE_RETURN_MSG("being_blocked");
			}
		}

		user.copyLimit(userBy);
		result["acccharge"] = user.accCharge;
		CLogicSecinc logicSecinc;
		ret = logicSecinc.GetSecinc(uid, result["secinc"]);
		if (ret != 0)
		return ret;

		Json::Value userFlag;
		ret = logicUser.GetUserFlag(uidBy, userFlag);
		if (ret != 0)
			return ret;

		if (!userFlag.isMember("dchg")
				|| CTime::GetDayInterval(userFlag["dchg"][0u].asUInt(), Time::GetGlobalTime()) != 0)
		{
			userFlag["dchg"][0u] = Time::GetGlobalTime();
			userFlag["dchg"][1u] = 0;
		}
		result["dailyacccharge"] = userFlag["dchg"][1u];
		if(!userFlag.isMember("heroCoins"))   //用于购买武将碎片
		{
			result["coins3"] = 0;
			result["todaycharge"] = 0;
		}
		else
		{
			result["coins3"] = userFlag["heroCoins"][1u].asInt();
			unsigned last_ts = userFlag["heroCoins"][0u].asUInt();
			unsigned now = time(0);
			if(CTime::GetDayInterval(last_ts,now) == 0)
			{
				result["todaycharge"] = userFlag["heroCoins"][2u].asInt();
			}
			else
			{
				result["todaycharge"] = 0;
			}
		}
		if(Config::GetIntValue("new_flag") == 1)
		{

			CLogicBuyGoods logicBuyGoods;
			ret = logicBuyGoods.GetGoods(uid,result["buy_goods"]);
		}
	}
	else
	{
		ret = logicUser.GetUserLimit(uid, user);
		if (ret != 0)
			return ret;
		bool isMainPos = ((world_pos == 0 || world_pos == user.mainpos) ? true : false);
		if (isMainPos)
		{
			//20131107 Ralf last_breath_time -> last_attacked_time : fix for online attacking bugs
			logicUser.UpdateBreathTime(uid);
			//debug_log("[update attack,uid=%u,userid=%u,last_breath_time=%u]",uid,uidBy,Time::GetGlobalTime());
		}
	}

	CLogicUpdates logicUpdates;
	ret = logicUpdates.GetUpdates(uid, result["updates"]["data"], uid != uidBy);
	if (ret != 0)
		return ret;

	CLogicBroadcast logicBd;
	vector<BroadcastMessage> bdMsgs;
	ret = logicBd.GetBdMessage(bdMsgs);
	if (ret == 0 && bdMsgs.size() > 0) {
		result["broadcast"].resize(bdMsgs.size());
		for (unsigned i = 0; i < bdMsgs.size(); i++) {
			result["broadcast"][i]["id"] = bdMsgs[i].id;
			result["broadcast"][i]["content"] = bdMsgs[i].content;
		}
	}
	// add by aaron 20121106

//	Json::Value userFlag;
//	Json::Reader reader;
//	if (!user.user_flag.empty() && reader.parse(user.user_flag, userFlag))
//	{
//		if (!userFlag.isMember("dchg")
//				|| CTime::GetDayInterval(userFlag["dchg"][0u].asUInt(), Time::GetGlobalTime()) != 0)
//		{
//			userFlag["dchg"][0u] = Time::GetGlobalTime();
//			userFlag["dchg"][1u] = 0;
//		}
//		result["dailyacccharge"] = userFlag["dchg"][1u];
//	}

//	// hero synchronization
//	//hero
//	CLogicHero logicHero;
//	ret = logicHero.GetHero(uid, result["hero"]);
//	if (ret != 0)
//		return ret;
	// 分基地容量变动
	//result["world_pos"]["world_pos"] = user.mainpos;
	result["mainpos"] = user.mainpos;
//	CLogicBaseExtra logicBaseExtra;
//	ret = logicBaseExtra.GetSubBaseInfoJson(uid,result["worldres"]);
//	unsigned size = result["worldres"].size();
//	result["worldres"].resize(size + 1);
//	result["worldres"][size]["world_pos"] = user.mainpos;
//	result["worldres"][size]["m"][0u] = user.r1_max;
//	result["worldres"][size]["m"][1u] = user.r2_max;
//	result["worldres"][size]["m"][2u] = user.r3_max;
//	result["worldres"][size]["m"][3u] = user.r4_max;
//	result["worldres"][size]["m"][4u] = user.r5_max;
	// add by aaron 20121030

	CLogicBaseExtra logicBaseExtra;
	// 默认主城坐标
	bool isMainPos = (
			(world_pos == 0 || world_pos == user.mainpos) ? true : false);
	if (!isMainPos) {
		DataBaseExtra baseExtra;
		logicBaseExtra.GetBaseExtra(uid, world_pos, baseExtra);
		if (BEING_ATTACK == baseExtra.being_attack_flag
				&& IsBeingAttacked(baseExtra.last_breath_time)
				&& uidBy != baseExtra.last_breath_uid) {
			//permit = APT_BAN_BEING_ATTACK;
			LOGIC_ERROR_RETURN_MSG("u_being_attacked");

		}
		baseExtra.last_breath_time = Time::GetGlobalTime();
		baseExtra.last_breath_uid = uidBy;
		logicBaseExtra.SetBaseExtra(uid, world_pos, baseExtra);
		//logicBaseExtra.UpdateLastBreathTime(uid,world_pos,Time::GetGlobalTime());
	}
	result["coins"] = payData.coins;
	result["coins2"] = payData.cash;

	result["invite_count"] = user.invite_count;
	result["today_invite_count"] = user.today_invite_count;
	result["resources"].resize(5);
	result["resources"][(unsigned) 0]["c"] = user.r1;
	result["resources"][(unsigned) 1]["c"] = user.r2;
	result["resources"][(unsigned) 2]["c"] = user.r3;
	result["resources"][(unsigned) 3]["c"] = user.r4;
	result["resources"][(unsigned) 4]["c"] = user.r5;
	result["currencys"][(unsigned) 0] = user.gcbase;
	result["currencys"][(unsigned) 1] = user.gcbuy;
	result["currencys"][(unsigned) 2] = user.prosper;
	result["currencys"][(unsigned) 3] = user.battle_spirits;
	result["sstate"] = user.sstate;
	return 0;
}
int CLogicArchive::Updates(unsigned uid, unsigned uidBy,
		Json::Value &data, Json::Value &result) {
	CLogicUser logicUser;
	//if (!IsValidUid(uid))
	//{
	//	return 0;
	//}
	int ret = 0;
	if (uid != uidBy && IsValidUid(uid)) {
		CLogicHelpHist logicHelphist;
		CLogicUpdates logicUpdates;
		if (Json::IsObject(data, "updates")
				&& Json::IsArray(data["updates"], "data")) {
			ret = logicUpdates.AddUpdates(uid, data["updates"]["data"], true);
			if (ret != 0)
				return ret;
			//增加帮助信息
			CLogicUserInteract logicInteract;
			logicInteract.AddHelp(uidBy, uid);
			logicInteract.AddFriendInteract(uidBy, uid, "HELP");
			logicHelphist.AddHelpHist(uid, data["updates"]["data"]);
		}
		if (Json::IsObject(data, "tribute2")) {
			CLogicTribute logicTribute;
			ret = logicTribute.Tribute(uid, uidBy, data["tribute2"]);
			if (ret != 0)
				return ret;
			unsigned ts = data["ts"].asUInt();
			ret = logicHelphist.AddTributeHist(uid, uidBy, ts);
			if (0 != ret) {
				return ret;
			}
		}
		if (Json::IsObject(data, "updateinfo")) {
			unsigned sstate = 0;
			if (Json::GetUInt(data["updateinfo"], "sstate", sstate)) {
				CLogicUser logicUser;
				DataUser user;
				ret = logicUser.GetUser(uid, user);
				if (ret != 0) {
					info_log("[get user faile!][uid=%u,ret=%d]", uid, ret);
					return ret;
				}
				user.sstate = sstate;
				ret = logicUser.SetUser(uid, user);
				if (0 != ret) {
					info_log("[set user faile!][uid=%u,ret=%d]", uid, ret);
					return ret;
				}
			}
		}
	}
	if (Json::IsObject(data, "updateinfo")) {
		ret = ProcessUpdateInfo(uidBy, data["updateinfo"]);
		if (ret != 0)
			return ret;
	}
	int onlineattcked = 0;
	if (Json::GetInt(data, "onlineattcked", onlineattcked)) {
		DataUser userBy;
		ret = logicUser.GetUserLimit(uidBy, userBy);
		if (ret != 0)
			return ret;
		CLogicUser::SetFlag(userBy.bit_info, UF_BE_CAN_ONLINE_ATTACKED,
				(onlineattcked == 0 ? false : true));
		ret = logicUser.UpdateBitInfo(uidBy, userBy.bit_info);
		if (ret != 0)
			return ret;
		//info_log("[onlineattcked][uidBy=%u,uid=%u,bit_info=%d,onlineattcked=%d]",uidBy,uid,userBy.bit_info,onlineattcked);
	}

	string broadcast;
	if (Json::GetString(data, "broadcast", broadcast)) {
		CLogicBroadcast logicBd;
		logicBd.AddBdMessage(broadcast);
	}

	return 0;
}

int CLogicArchive::ProcessOrders(unsigned uid, const Json::Value data,
		DataPay &payData, bool addable) {
	if (data.isMember("orders")) {
		if (!data["orders"].isArray()) {
			error_log("[orders error][uid=%u,type=%d]", uid,
					data["orders"].type());
			DATA_ERROR_RETURN_MSG("data_orders_error");
		}
		Json::FastWriter writer;
		int totalCoins = 0;
		int totalCash = 0;
		int totalCoins2 = 0;
		string payType;
		for (unsigned i = 0; i < data["orders"].size(); i++)
		{
			if (!data["orders"][i].isArray()
					|| data["orders"][i].size() != 4
					|| !data["orders"][i][(unsigned)0].isString()
					|| !data["orders"][i][(unsigned)1].isIntegral()
					|| !data["orders"][i][(unsigned)2].isIntegral()
					|| !data["orders"][i][(unsigned)3].isIntegral())
			{
				error_log("[orders error][uid=%u,index=%d,data=%s]",uid,i,(writer.write(data["orders"][i])).c_str());
				DATA_ERROR_RETURN_MSG("data_orders_error");
			}
			int orderType = data["orders"][i][(unsigned)3].asInt();
			if (orderType != 0 && orderType != 1 && orderType != 2)
			{
				error_log("[orders error][uid=%u,index=%d,data=%s]",uid,i,(writer.write(data["orders"][i])).c_str());
				DATA_ERROR_RETURN_MSG("orders_type_error");
			}

			int price = data["orders"][i][(unsigned)1].asInt();
			if(!addable && price > 0)
			{
				error_log("[not addable][uid=%u,index=%d,data=%s]",uid,i,(writer.write(data["orders"][i])).c_str());
				continue;
			}

			const char *pCode = data["orders"][i][(unsigned)0].asCString();
			int amount = data["orders"][i][(unsigned)2].asInt();
			if (price != 0 && amount != 0)
			{
				ORDERS_LOG("uid=%u&code=%s&price=%d&amount=%d&type=%d",uid,pCode,price,amount,orderType);
			}
			if (orderType == 1)
			{
				totalCash += price * amount;
			}
			else if(0 == orderType)
			{
				totalCoins += price * amount;
			}
			else if(2 == orderType)
			{
				totalCoins2 += price * amount;
			}
			if (!payType.empty()) payType.append(";");
			payType.append(pCode).append(":").append(CTrans::ITOS(amount));
		}
		if (totalCash != 0 || totalCoins != 0)
		{
			CLogicPay logicPay;
			int ret = logicPay.ChangePay(uid, totalCash, totalCoins, payData, payType);
			if (ret != 0)
			return ret;
		}

		if(totalCoins2 != 0)
		{
			CLogicPay logicPay;
			int ret = logicPay.ChangePayHeroCoins(uid, totalCoins2,payType);
			if (ret != 0)
			return ret;
		}
	}
	return 0;
}
/*
int CLogicArchive::UpdateAttack(unsigned uid, unsigned uidDefence,
		Json::Value &data) {
	int ret = 0;
	if (data.isMember("lootreport") || data.isMember("attackreport")) {
		Json::FastWriter writer;
		//更新进攻历史
		DataAttack attackData;
		attackData.attack_uid = uid;
		attackData.defence_uid = uidDefence;
		attackData.attack_platform = OpenPlatform::GetType();
		attackData.r1_loot = 0;
		attackData.r2_loot = 0;
		attackData.r3_loot = 0;
		attackData.r4_loot = 0;
		attackData.r5_loot = 0;
		if (data.isMember("lootreport") && data["lootreport"].isObject()) {
			Json::GetInt(data["lootreport"], "r1", attackData.r1_loot);
			Json::GetInt(data["lootreport"], "r2", attackData.r2_loot);
			Json::GetInt(data["lootreport"], "r3", attackData.r3_loot);
			Json::GetInt(data["lootreport"], "r4", attackData.r4_loot);
			Json::GetInt(data["lootreport"], "r5", attackData.r5_loot);
		}
		if (data.isMember("attackreport")) {
			attackData.log = writer.write(data["attackreport"]);
		}

		CLogicAttack logicAttack;
		ret = logicAttack.UpdateAttack(attackData);
		if (ret != 0)
			return ret;
	}

	if (Json::IsObject(data, "attackinfo")) {
		ret = ProcessAttackInfo(uid, data["attackinfo"]);
		if (ret != 0)
			return ret;
	}
	return 0;
}
*/
int CLogicArchive::UpdateAttack(const DataUser &user,
		const DataUser &userDefence, Json::Value &data) {
	int ret = 0;

	if (data.isMember("lootreport") || data.isMember("attackreport")) {
		Json::FastWriter writer;
		//更新进攻历史
		DataAttack attackData;
		attackData.attack_uid = user.uid;
		attackData.defence_uid = userDefence.uid;
		attackData.attack_platform = OpenPlatform::GetType();
		attackData.r1_loot = 0;
		attackData.r2_loot = 0;
		attackData.r3_loot = 0;
		attackData.r4_loot = 0;
		attackData.r5_loot = 0;

		if (data.isMember("lootreport") && data["lootreport"].isObject()) {
			Json::GetInt(data["lootreport"], "r1", attackData.r1_loot);
			Json::GetInt(data["lootreport"], "r2", attackData.r2_loot);
			Json::GetInt(data["lootreport"], "r3", attackData.r3_loot);
			Json::GetInt(data["lootreport"], "r4", attackData.r4_loot);
			Json::GetInt(data["lootreport"], "r5", attackData.r5_loot);

		}
		if (data.isMember("attackreport")) {
			attackData.log = writer.write(data["attackreport"]);
		}

		CLogicAttack logicAttack;
		//ret = logicAttack.UpdateAttack(attackData);
		uint64_t attackId = 0;
		ret = logicAttack.UpdateAttack(attackData, attackId);
		if (ret != 0)
			return ret;
		//attackid|进攻者id|进攻者等级|被进攻者id|被进攻者等级|damage
		int damage = 0;
		Json::GetInt(data, "damage", damage);
		int attackend = 0;
		if (Json::GetInt(data, "attackend", attackend) && attackend == 1) {
			ATTACK_LOG("%lu|%u|%d|%u|%d|%d|%d|%d|%d|%d|%d|", attackId, user.uid,
					user.level, userDefence.uid, userDefence.level, damage,
					attackData.r1_loot, attackData.r2_loot, attackData.r3_loot,
					attackData.r4_loot, attackData.r5_loot);
		}
	}

	if (Json::IsObject(data, "attackinfo")) {
		ret = ProcessAttackInfo(user.uid, data["attackinfo"]);
		if (ret != 0)
			return ret;
	}
	return 0;
}
int CLogicArchive::ProcessAttackInfo(unsigned uid,
		Json::Value &attackinfo) {
	if (!attackinfo.isObject()) {
		error_log("[attackinfo type error][uid=%u]", uid);
		PARAM_ERROR_RETURN_MSG("param_error");
	}
	if (attackinfo.isNull())
	return 0;
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUser(uid, user);
	if (ret != 0)
	return ret;

	int gcbaschg = 0, gcbuychg = 0, addprosper = 0, addbs = 0, itemp = 0;
	int qleold = -1, qlechg = 0, qleres = -1;
	uint64_t exp = 0;

	Json::GetUInt64(attackinfo, "exp", exp);
	user.point += exp;

	int leveltemp = 0;
	Json::GetInt(attackinfo, "level", leveltemp);
	if(user.level > 50 && leveltemp > user.level + 1)
	{
		error_log("[level_error][uid=%u]", uid);
		LOGIC_ERROR_RETURN_MSG("level_error");
	}
	else if(leveltemp != user.level)
	{
		user.level =  leveltemp;
		//logicTopTenUser.SetUserLevel(user.uid, user.level);
	}

	if (Json::IsArray(attackinfo, "currencys"))
	{
		if (attackinfo["currencys"].size() > 0)
		{
			if (Json::GetInt(attackinfo["currencys"], (unsigned)0, gcbaschg) && gcbaschg != 0)
			{
				itemp = gcbaschg + user.gcbase;
				if (itemp >= 0)
					user.gcbase += gcbaschg;
				else {
					user.gcbase = 0;
					error_log("[resource_error][uid=%u,gcbaschg=%d]", uid, gcbaschg);
					//LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
		if (attackinfo["currencys"].size() > 1)
		{
			if (Json::GetInt(attackinfo["currencys"], (unsigned)1, gcbuychg) && gcbuychg != 0)
			{
				itemp = gcbuychg + user.gcbuy;
				if (itemp >= 0)
					user.gcbuy += gcbuychg;
				else {
					user.gcbuy = 0;
					error_log("[resource_error][uid=%u,gcbuychg=%d]", uid, gcbuychg);
					//LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
		if (attackinfo["currencys"].size() > 2)
		{
			if (Json::GetInt(attackinfo["currencys"], (unsigned)2, addprosper) && addprosper != 0)
			{
				itemp = addprosper + user.prosper;
				if (itemp >= 0)
					user.prosper += addprosper;
				else {
					error_log("[resource_error][uid=%u]", uid);
					LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
		if (attackinfo["currencys"].size() > 3)
		{
			if (Json::GetInt(attackinfo["currencys"], (unsigned)3, addbs) && addbs != 0)
			{
				itemp = addbs + user.battle_spirits;
				if (itemp >= 0)
					user.battle_spirits += addbs;
				else {
					error_log("[resource_error][uid=%u]", uid);
					LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
	}
	int r1chg = 0, r2chg = 0, r3chg = 0, r4chg = 0, r5chg = 0;
	if (Json::IsArray(attackinfo, "resources"))
	{
		if (attackinfo["resources"].size() > 0)
		{
			if (Json::GetInt(attackinfo["resources"][(unsigned)0], "d", r1chg))
			{
				itemp = r1chg + user.r1;
				if (itemp >= 0)
					user.r1 += r1chg;
				else {
					error_log("[resource_error][uid=%u]", uid);
					LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
		if (attackinfo["resources"].size() > 1)
		{
			if (Json::GetInt(attackinfo["resources"][(unsigned)1], "d", r2chg))
			{
				itemp = r2chg + user.r2;
				if (itemp >= 0)
					user.r2 += r2chg;
				else {
					error_log("[resource_error][uid=%u]", uid);
					LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
		if (attackinfo["resources"].size() > 2)
		{
			if (Json::GetInt(attackinfo["resources"][(unsigned)2], "d", r3chg))
			{
				itemp = r3chg + user.r3;
				if (itemp >= 0)
					user.r3 += r3chg;
				else {
					error_log("[resource_error][uid=%u]", uid);
					LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
		if (attackinfo["resources"].size() > 3)
		{
			if (Json::GetInt(attackinfo["resources"][(unsigned)3], "d", r4chg))
			{
				itemp = r4chg + user.r4;
				if (itemp >= 0)
					user.r4 += r4chg;
				else {
					error_log("[resource_error][uid=%u]", uid);
					LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
		if (attackinfo["resources"].size() > 4)
		{
			if (Json::GetInt(attackinfo["resources"][(unsigned)4], "d", r5chg))
			{
				itemp = r5chg + user.r5;
				if (itemp >= 0)
					user.r5 += r5chg;
				else {
					error_log("[resource_error][uid=%u]", uid);
					LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
	}
	Json::Reader reader;
	Json::FastWriter writer;
	if (attackinfo.isMember("soldier"))
	{
		user.soldier = writer.write(attackinfo["soldier"]);
	}
	if (attackinfo.isMember("stats"))
	{
		Json::Value old;
		reader.parse(user.user_stat, old);
		ret = checkUserStats(old,attackinfo["stats"]);
		if(ret != 0)
		{
			error_log("[user_stat error][uid=%u,ret=%u]", uid,ret);
			LOGIC_ERROR_RETURN_MSG("user_stat error");
		}
		user.user_stat = writer.write(attackinfo["stats"]);
	}
	if (attackinfo.isMember("tech"))
	{
		Json::Value OldUserTech;
		reader.parse(user.user_tech, OldUserTech);

		ret = checkUserTech(OldUserTech,attackinfo["tech"]);
		if(ret != 0)
		{
			error_log("[user_tech error][uid=%u,ret=%u]", uid,ret);
			LOGIC_ERROR_RETURN_MSG("user_tech error");
		}

		if (attackinfo["tech"]["godE"].isObject()) {
			if (attackinfo["tech"]["godE"]["qlE"].isArray()) {
				if (OldUserTech["godE"]["qlE"].isArray()) {
					qleold = OldUserTech["godE"]["qlE"][0u].asInt();
					qlechg = attackinfo["tech"]["godE"]["qlE"][0u].asInt();
					qleres = qlechg + qleold;
					if(qleres < 0){
						error_log("[resource_error][uid=%u]", uid);
						LOGIC_ERROR_RETURN_MSG("resource_error");
					}
					attackinfo["tech"]["godE"]["qlE"][0u] = qleres;
				}
			}
		}
		user.user_tech = writer.write(attackinfo["tech"]);
	}
	//前台不需要处理wood字段
	/*Json::Value statJson;
	 if (user.user_stat.empty())
	 user.user_stat = "{}";
	 if (reader.parse(user.user_stat, statJson))
	 {
	 unsigned wood = 0;
	 if (!statJson.isMember("wood") || Json::GetUInt(statJson, "wood", wood))
	 {
	 wood += r2chg + r3chg + r4chg + r5chg;
	 statJson["wood"] = wood;
	 user.user_stat = writer.write(statJson);
	 }
	 }*/
	ret = logicUser.SetUser(uid, user);
	if (ret != 0)
	return ret;
	RESOURCE_LOG("[atck change][uid=%u,gcbaschg=%d,gcbase=%u,gcbuychg=%d,gcbuy=%u,r1chg=%d,r1=%u,r2chg=%d,r2=%u,r3chg=%d,r3=%u,r4chg=%d,r4=%u,r5chg=%d,r5=%u,addprosper=%d,prosper=%u,addbs=%d,bs=%u,qlechg=%d,qle=%d,expchg=%u,exp=%u,lvl=%u]",
			uid,gcbaschg,user.gcbase,gcbuychg,user.gcbuy,r1chg,user.r1,r2chg,user.r2,r3chg,user.r3,r4chg,user.r4,r5chg,user.r5,addprosper,user.prosper,addbs,user.battle_spirits,qlechg,qleres,exp,user.point,user.level);

	if (Json::IsArray(attackinfo, "hero"))
	{
		CLogicHero logicHero;
		ret = logicHero.UpdateHero(uid, attackinfo["hero"]);
		if (ret != 0)
		return ret;
	}
	if (Json::IsArray(attackinfo, "equipment"))
	{
		CLogicEquipment logicEquip;
		ret = logicEquip.UpdateEquipment(uid, attackinfo["equipment"]);
		if (ret != 0)
		return ret;
	}
	return 0;
}

int CLogicArchive::ProcessUpdateInfo(unsigned uid,
		Json::Value &updateinfo) {
	if (!updateinfo.isObject()) {
		error_log("[updateinfo type error][uid=%u]", uid);
		PARAM_ERROR_RETURN_MSG("param_error");
	}
	if (updateinfo.isNull())
	return 0;
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUser(uid, user);
	if (ret != 0)
	return ret;

	int gcbaschg = 0, gcbuychg = 0, addprosper = 0, addbs = 0, itemp = 0;
	int qleold = -1, qlechg = 0, qleres = -1;

	if (Json::IsArray(updateinfo, "currencys"))
	{
		if (updateinfo["currencys"].size() > 0)
		{
			if (Json::GetInt(updateinfo["currencys"], (unsigned)0, gcbaschg) && gcbaschg != 0)
			{
				itemp = gcbaschg + user.gcbase;
				if (itemp >= 0)
				user.gcbase += gcbaschg;
				else {
					error_log("[resource_error][uid=%u]", uid);
					LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
		if (updateinfo["currencys"].size() > 1)
		{
			if (Json::GetInt(updateinfo["currencys"], (unsigned)1, gcbuychg) && gcbuychg != 0)
			{
				itemp = gcbuychg + user.gcbuy;
				if (itemp >= 0)
				user.gcbuy += gcbuychg;
				else {
					error_log("[resource_error][uid=%u]", uid);
					LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
		if (updateinfo["currencys"].size() > 2)
		{
			if (Json::GetInt(updateinfo["currencys"], (unsigned)2, addprosper) && addprosper != 0)
			{
				itemp = addprosper + user.prosper;
				if (itemp >= 0)
				user.prosper += addprosper;
				else {
					error_log("[resource_error][uid=%u]", uid);
					LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
		if (updateinfo["currencys"].size() > 3)
		{
			if (Json::GetInt(updateinfo["currencys"], (unsigned)3, addbs) && addbs != 0)
			{
				itemp = addbs + user.battle_spirits;
				if (itemp >= 0)
				user.battle_spirits += addbs;
				else {
					error_log("[resource_error][uid=%u]", uid);
					LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
	}
	int r1chg = 0, r2chg = 0, r3chg = 0, r4chg = 0, r5chg = 0;
	if (Json::IsArray(updateinfo, "resources"))
	{
		if (updateinfo["resources"].size() > 0)
		{
			if (Json::GetInt(updateinfo["resources"][(unsigned)0], "d", r1chg))
			{
				itemp = r1chg + user.r1;
				if (itemp >= 0)
				user.r1 += r1chg;
				else {
					error_log("[resource_error][uid=%u]", uid);
					LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
		if (updateinfo["resources"].size() > 1)
		{
			if (Json::GetInt(updateinfo["resources"][(unsigned)1], "d", r2chg))
			{
				itemp = r2chg + user.r2;
				if (itemp >= 0)
				user.r2 += r2chg;
				else {
					error_log("[resource_error][uid=%u]", uid);
					LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
		if (updateinfo["resources"].size() > 2)
		{
			if (Json::GetInt(updateinfo["resources"][(unsigned)2], "d", r3chg))
			{
				itemp = r3chg + user.r3;
				if (itemp >= 0)
				user.r3 += r3chg;
				else {
					error_log("[resource_error][uid=%u]", uid);
					LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
		if (updateinfo["resources"].size() > 3)
		{
			if (Json::GetInt(updateinfo["resources"][(unsigned)3], "d", r4chg))
			{
				itemp = r4chg + user.r4;
				if (itemp >= 0)
				user.r4 += r4chg;
				else {
					error_log("[resource_error][uid=%u]", uid);
					LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
		if (updateinfo["resources"].size() > 4)
		{
			if (Json::GetInt(updateinfo["resources"][(unsigned)4], "d", r5chg))
			{
				itemp = r5chg + user.r5;
				if (itemp >= 0)
				user.r5 += r5chg;
				else {
					error_log("[resource_error][uid=%u]", uid);
					LOGIC_ERROR_RETURN_MSG("resource_error");
				}
			}
		}
	}
	Json::Reader reader;
	Json::FastWriter writer;
	if (updateinfo.isMember("soldier"))
	{
		user.soldier = writer.write(updateinfo["soldier"]);
	}
	if (updateinfo.isMember("stats"))
	{
		Json::Value old;
		reader.parse(user.user_stat, old);
		ret = checkUserStats(old,updateinfo["stats"]);
		if(ret != 0)
		{
			error_log("[user_stat error][uid=%u,ret=%u]", uid,ret);
			LOGIC_ERROR_RETURN_MSG("user_stat error");
		}
		user.user_stat = writer.write(updateinfo["stats"]);
	}
	if (updateinfo.isMember("tech"))
	{
		Json::Value OldUserTech;
		reader.parse(user.user_tech, OldUserTech);

		ret = checkUserTech(OldUserTech,updateinfo["tech"]);
		if(ret != 0)
		{
			error_log("[user_tech error][uid=%u,ret=%u]", uid,ret);
			LOGIC_ERROR_RETURN_MSG("user_tech error");
		}

		if (updateinfo["tech"]["godE"].isObject()) {
			if (updateinfo["tech"]["godE"]["qlE"].isArray()) {
				if (OldUserTech["godE"]["qlE"].isArray()) {
					qleold = OldUserTech["godE"]["qlE"][0u].asInt();
					qlechg = updateinfo["tech"]["godE"]["qlE"][0u].asInt();
					qleres = qlechg + qleold;
					if(qleres < 0){
						error_log("[resource_error][uid=%u]", uid);
						LOGIC_ERROR_RETURN_MSG("resource_error");
					}
					updateinfo["tech"]["godE"]["qlE"][0u] = qleres;
				}
			}
		}
		if(updateinfo["tech"].size())
			user.user_tech = writer.write(updateinfo["tech"]);
	}

	ret = logicUser.SetUser(uid, user);
	if (ret != 0)
	return ret;
	RESOURCE_LOG("[updt change][uid=%u,gcbaschg=%d,gcbase=%u,gcbuychg=%d,gcbuy=%u,r1chg=%d,r1=%u,r2chg=%d,r2=%u,r3chg=%d,r3=%u,r4chg=%d,r4=%u,r5chg=%d,r5=%u,addprosper=%d,prosper=%u,addbs=%d,bs=%u,qlechg=%d,qle=%d,expchg=%u,exp=%u,lvl=%u]",
			uid,gcbaschg,user.gcbase,gcbuychg,user.gcbuy,r1chg,user.r1,r2chg,user.r2,r3chg,user.r3,r4chg,user.r4,r5chg,user.r5,addprosper,user.prosper,addbs,user.battle_spirits,qlechg,qleres,0,user.point,user.level);
	return 0;
}

int CLogicArchive::SaveRefresh(unsigned uid, const string ipStr,
		Json::Value &refreshinfo) {
	if (!IsValidUid(uid)) {
		return 0;
	}

	if (refreshinfo.isNull())
		return 0;

	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUser(uid, user);
	if (ret != 0)
		return ret;

	string ip = ipStr;
	user.cdn = 0;
	user.refresh = 0;
	user.memory = 0;

	int cdn = 0, refresh = 0, memory = 0;
	if (Json::GetInt(refreshinfo, "cdn", cdn)) {
		user.cdn = cdn;
	}
	if (Json::GetInt(refreshinfo, "refresh", refresh)) {
		user.refresh = refresh;
	}
	if (Json::GetInt(refreshinfo, "memory", memory)) {
		user.memory = memory;
	}

	SPEED_LOG("%s|%u|%u|%u|%u", ip.c_str(), uid, cdn, refresh, memory);

	return logicUser.SetUser(uid, user);

}

int CLogicArchive::GetActivityTime(Json::Value &data) {
	unsigned beginTime = 0;
	unsigned endTime = 0;
	unsigned version = 0;
	string platform;

	Json::Value activityTime = Json::Value(Json::arrayValue);
	if (Config::GetUIntValue(beginTime, "twice_reward_b")
			&& Config::GetUIntValue(endTime, "twice_reward_e")) {
		activityTime[0u] = beginTime;
		activityTime[1u] = endTime;
		data["wolong"] = activityTime;
	}

	if (Config::GetUIntValue(beginTime, CONFIG_SUNCE_BEGIN_TS)
			&& Config::GetUIntValue(endTime, CONFIG_SUNCE_END_TS)) {
		activityTime[0u] = beginTime;
		activityTime[1u] = endTime;
		data["sunce"] = activityTime;
	}

	if (Config::GetUIntValue(beginTime, CONFIG_PAY_TOTAL_BEGIN_TS)
			&& Config::GetUIntValue(endTime, CONFIG_PAY_TOTAL_END_TS)
			&& Config::GetValue(platform, CONFIG_PAY_TOTAL_PF)) {
		activityTime[0u] = beginTime;
		activityTime[1u] = endTime;
		activityTime[2u] = platform;
		data["paytotal"] = activityTime;
	}

	if (Config::GetUIntValue(beginTime, CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS)
			&& Config::GetUIntValue(endTime, CONFIG_PAY_OF_HUNDRED_DAYS_END_TS)
			&& Config::GetValue(platform, CONFIG_PAY_OF_HUNDRED_DAYS_PF)) {
		activityTime[0u] = beginTime;
		activityTime[1u] = endTime + 24*60*60;
		activityTime[2u] = platform;
		data["lebusishu"] = activityTime;
	}

	if (Config::GetUIntValue(beginTime, CONFIG_POINTS_BEGIN_TS)
			&& Config::GetUIntValue(endTime, CONFIG_POINTS_END_TS)
			&& Config::GetValue(platform, CONFIG_POINTS_PF)
			&& Config::GetUIntValue(version, CONFIG_POINTS_VERSION)) {
		activityTime[0u] = beginTime;
		activityTime[1u] = endTime;
		activityTime[2u] = platform;
		activityTime[3u] = version;
		data["jug"] = activityTime;
	}

	if (Config::GetUIntValue(beginTime, CONFIG_PAYRANK_BEGIN_TS)
			&& Config::GetUIntValue(endTime, CONFIG_PAYRANK_END_TS)
			&& Config::GetUIntValue(version, CONFIG_PAYRANK_VERSION)
			&& Config::GetValue(platform, CONFIG_PAYRANK_PF)) {
		activityTime[0u] = beginTime;
		activityTime[1u] = endTime;
		activityTime[2u] = platform;
		activityTime[3u] = version;
		data["points"] = activityTime;
	}

	if (Config::GetUIntValue(beginTime, CONFIG_MERRYSOULS_BEGIN_TS)
			&& Config::GetUIntValue(endTime, CONFIG_MERRYSOULS_END_TS)
			&& Config::GetValue(platform, CONFIG_MERRYSOULS_PF)
			&& Config::GetUIntValue(version, CONFIG_MERRYSOULS_VERSION)) {
		activityTime[0u] = beginTime;
		activityTime[1u] = endTime;
		activityTime[2u] = platform;
		activityTime[3u] = version;
		data["zhongqiu"] = activityTime;
	}


	return 0;
}

int CLogicArchive::checkUserStats(const Json::Value &old, const Json::Value &now)
{
	if(old.isMember("stars"))
	{
		if(!now.isMember("stars"))
			return 1;
		if(old["stars"].asUInt() > now["stars"].asUInt())
			return 2;
	}
	return 0;
}
int CLogicArchive::checkUserTech(const Json::Value &old, const Json::Value &now)
{
	if(old.isMember("godE") && old["godE"].isObject() && old["godE"]["godE"].isArray())
	{
		if(now.isMember("godE") && now["godE"].isObject() && now["godE"]["godE"].isArray())
		{
			for(unsigned i=0;i<old["godE"]["godE"].size();++i)
			{
				if(old["godE"]["godE"][i].isArray())
				{
					if(now["godE"]["godE"].size() > i && now["godE"]["godE"][i].isArray())
					{
						for(unsigned j=0;j<old["godE"]["godE"][i].size();++j)
						{
							if(now["godE"]["godE"][i].size()>j)
							{
								if(old["godE"]["godE"][i][j] > now["godE"]["godE"][i][j])
									return 4;
							}
							else
								return 3;
						}
					}
					else
						return 2;
				}
			}
		}
		else
			return 1;
	}

	if(old.isMember("part") && old["part"].isArray())
	{
		if(now.isMember("part") && now["part"].isArray())
		{
			for(unsigned i=0;i<old["part"].size();++i)
			{
				if(now["part"].size() > i)
				{
					if(old["part"][i] > now["part"][i])
						return 7;
				}
				else
					return 6;
			}
		}
		else
			return 5;
	}

	if(old.isMember("soul"))
	{
		if(!now.isMember("soul"))
			return 8;
		if(old["soul"].asUInt() > now["soul"].asUInt())
			return 9;
	}

	return 0;
}
