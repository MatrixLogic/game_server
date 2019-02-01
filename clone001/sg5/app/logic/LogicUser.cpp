#include "LogicUser.h"

int CLogicUser::AddUser(unsigned uid, PlatformType platform, unsigned inviteUid)
{
	const Json::Value *pInitJson = 0;
	int ret = CLogicArchive::GetInitArchive(pInitJson);
	if (ret != 0)
		return ret;

	DataUser user;
	user.uid = uid;
	user.kingdom = 0;
	user.register_platform = OpenPlatform::GetPlatform()->GetPlatformByPF();
	user.register_time = Time::GetGlobalTime();
	user.invite_uid = inviteUid;
	user.last_login_platform = OpenPlatform::GetPlatform()->GetPlatformByPF();
	user.last_login_time = Time::GetGlobalTime();
	user.last_active_time = Time::GetGlobalTime();
	user.login_times = 1;
	user.login_days = 1;
	user.invite_count = 0;
	user.today_invite_count = 0;
	user.status = US_ACTIVE;
	user.type = UT_NORMAL;
	user.level = 0;
	user.point = 0;
	user.tutorial_stage = 0;
	user.r1 = NEW_USER_R1;
	user.r1_max = NEW_USER_R1_MAX;
	user.r2 = NEW_USER_R2;
	user.r2_max = NEW_USER_R2_MAX;
	user.r3 = NEW_USER_R3;
	user.r3_max = NEW_USER_R3_MAX;
	user.r4 = NEW_USER_R4;
	user.r4_max = NEW_USER_R4_MAX;
	user.r5 = NEW_USER_R5;
	user.r5_max = NEW_USER_R5_MAX;
	user.last_save_time = Time::GetGlobalTime();
	user.tribute_time = 0;	//封号到到期时间
	user.protected_time = Time::GetGlobalTime() + NEW_USER_PROTECT;
	user.gcbase = NEW_DAILY_GCBASE;
	user.newgcbase = DAILY_NEW_GCBASE;
	user.gcbuy = 0;
	user.last_save_uid = user.uid;
	user.last_breath_time = 0;
	user.bit_info = 0;
	user.alliance_id = ALLIANCE_ID_NULL;
	user.damage = 0;
	user.prosper = 0;
	user.accCharge = 0;
	user.ext = 0;
	user.lastseq = 0;
	user.battle_spirits = 0;
	user.viplevel = 0;
	user.mainpos = 0;

	Json::FastWriter writer;
	if (pInitJson->isMember("trainQ"))
	{
		user.trainQ = writer.write((*pInitJson)["trainQ"]);
	}
	if (pInitJson->isMember("skillQ"))
	{
		user.skillQ = writer.write((*pInitJson)["skillQ"]);
	}
	if (pInitJson->isMember("barrackQ"))
	{
		user.barrackQ = writer.write((*pInitJson)["barrackQ"]);
	}
	if (pInitJson->isMember("buildQ"))
	{
		user.buildQ = writer.write((*pInitJson)["buildQ"]);
	}
	if (pInitJson->isMember("soldier"))
	{
		user.soldier = writer.write((*pInitJson)["soldier"]);
	}
	if (pInitJson->isMember("soldierlevel"))
	{
		user.soldierlevel = writer.write((*pInitJson)["soldierlevel"]);
	}
	CDataUser dbUser;
	ret = dbUser.AddUser(user);
	if (ret != 0)
	{
		fatal_log("[AddUser fail][ret=%d,platform=%d]", ret, platform);
		DB_ERROR_RETURN_MSG("db_add_user_fail");
	}

	CLogicPay logicPay;
	ret = logicPay.InitailizePay(uid);
	if (ret != 0)
		return ret;

	CLogicBuilding logicBuilding;
	ret = logicBuilding.InitializeBuilding(uid);
	if (ret != 0)
		return ret;

	CLogicHero logicHero;
	ret = logicHero.InitializeHero(uid);
	if (ret != 0)
		return ret;

	CLogicEquipment logicEquip;
	ret = logicEquip.InitializeEquipment(uid);
	if (ret != 0)
		return ret;

	CLogicUserData logicUserData;
	ret = logicUserData.InitializeUserData(uid);
	if (ret != 0)
		return ret;

	CLogicTribute logicTribute;
	ret = logicTribute.InitializeTribute(uid);
	if (ret != 0)
		return ret;

	return 0;
}

int CLogicUser::ProcessInvite(unsigned uid, unsigned inviteUid)
{
	if (inviteUid == 0)
		return 0;
	CDataUser dbUser;
	DataUser user;
	int ret = dbUser.GetUserLimit(inviteUid, user);
	if(ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[GetUserLimit fail][ret=%,uid=%u,inviteUid=%u]", ret,uid,inviteUid);
		DB_ERROR_RETURN_MSG("db_get_user_fail");
	}
	user.invite_count++;
	user.today_invite_count++;
	ret = dbUser.SetUserLimit(inviteUid, user);
	if(ret != 0)
	{
		error_log("[SetUserLimit fail][ret=%,uid=%u,inviteUid=%u]", ret,uid,inviteUid);
		DB_ERROR_RETURN_MSG("db_set_user_fail");
	}
	return 0;
}

int CLogicUser::SetUser(unsigned uid, DataUser &user)
{
	UpdateLevel(user);
	CDataUser dbUser;
	int ret = dbUser.SetUser(uid, user);
	if(ret != 0)
	{
		error_log("[SetUser fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("db_set_user_fail");
	}
	return 0;
}

int CLogicUser::SetUserLimit(DataUser &user)
{
	return SetUserLimit(user.uid, user);
}

int CLogicUser::SetUserLimit(unsigned uid, DataUser &user)
{
	UpdateLevel(user);
	CDataUser dbUser;
	int ret = dbUser.SetUserLimit(uid, user);
	if (ret != 0)
	{
		error_log("[SetUserLimit fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("db_set_user_fail");
	}
	return 0;
}

int CLogicUser::GetUser(unsigned uid, DataUser &user)
{
	CDataUser dbUser;
	int ret = dbUser.GetUser(uid, user);
	if(ret != 0)
	{
		error_log("[GetUser fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("db_get_user_fail");
	}
	int vipleve = 0;
	TransformVipLevel(user.accCharge, vipleve);
	if (vipleve != user.viplevel)
	{
		user.viplevel = vipleve;
		ret = dbUser.SetUser(uid,user);
		if (0 != ret)
		{
			error_log("Set user faile! ret=%d,uid=%u", ret, uid);
		}
		CLogicUpdates logicUpdates;
		Json::Value updates;
		updates.resize(1);
		updates[(unsigned)0]["s"] = "VIPPROMOTION";
		updates[(unsigned)0]["vip_level"] = vipleve;
		updates[(unsigned)0]["ts"] = (unsigned)time(0);
		ret = logicUpdates.AddUpdates(uid, updates);
		if (0 != ret)
		{
			error_log("AddUpdates failed! uid=%u,ret=%d", uid,ret);
		}
	}

	return 0;
}

int CLogicUser::GetUserLimit(unsigned uid, DataUser &user)
{
	CDataUser dbUser;
	int ret = dbUser.GetUserLimit(uid, user);
	if(ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[GetUserLimit fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("db_get_user_fail");
	}
	int vipleve = 0;
	TransformVipLevel(user.accCharge, vipleve);
	if (vipleve != user.viplevel)
	{
		user.viplevel = vipleve;
		ret = dbUser.SetUserLimit(uid,user);
		if (0 != ret)
		{
			error_log("Set user faile! ret=%d,uid=%u", ret, uid);
		}
		CLogicUpdates logicUpdates;
		Json::Value updates;
		updates.resize(1);
		updates[(unsigned)0]["s"] = "VIPPROMOTION";
		updates[(unsigned)0]["vip_level"] = vipleve;
		updates[(unsigned)0]["ts"] = (unsigned)time(0);
		ret = logicUpdates.AddUpdates(uid, updates);
		if (0 != ret)
		{
			error_log("AddUpdates failed! uid=%u,ret=%d", uid,ret);
		}
	}
	return 0;
}

int CLogicUser::RemoveUser(unsigned uid)
{
	DataUser user;
	int ret = GetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	user.status = US_REMOVE;
	ret = SetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	return 0;
}

int CLogicUser::UpdateLogin(unsigned uid, PlatformType platform, unsigned loginTime)
{
	if (loginTime == 0)
		loginTime = Time::GetGlobalTime();
	DataUser user;
	int ret = GetUserLimit(uid, user);
	if (ret != 0)
	{
		return ret;
	}
	int inter = CTime::GetDayInterval(user.last_login_time, loginTime);
	if (inter == 1)
		user.login_days++;
	else if (inter > 1)
		user.login_days = 1;
	if (inter > 0)
	{
		if (user.tutorial_stage < TOTORIAL_FINISH)
			user.gcbase = NEW_DAILY_GCBASE;
//		else
//		{
//			if (user.gcbase > GCBASE_LIMIT)
//				user.gcbase = GCBASE_LIMIT;
//		}
		user.newgcbase = DAILY_NEW_GCBASE;
		user.today_invite_count = 0;
		CLogicTribute logicTri;
		logicTri.DaliyTribute(uid, user.invite_count);

	}
	user.last_login_platform = OpenPlatform::GetPlatform()->GetPlatformByPF();;
	user.last_login_time = loginTime;
	user.login_times++;

	return SetUserLimit(uid, user);
}

int CLogicUser::UpdateActiveTime(unsigned uid, unsigned last_active_time, int ext)
{
	unsigned now = Time::GetGlobalTime();
	if (CTime::GetDayInterval(last_active_time, now) != 0)
	{
		ext = 0;
	}
	else if (IsOnlineUser(last_active_time))
	{
		ext += now - last_active_time;
	}
	CDataUser dbUser;
	int ret = dbUser.SetActiveTimeAndExt(uid, now, ext);
	if(ret != 0)
	{
		error_log("[SetActiveTimeAndExt fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("db_set_user_fail");
	}

	return 0;
}

int CLogicUser::UpdateLastSaveUid(unsigned uid, unsigned last_save_uid)
{
	CDataUser dbUser;
	int ret = dbUser.SetLastSaveUid(uid, last_save_uid);
	if(ret != 0)
	{
		error_log("[SetLastSaveUid fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("db_set_user_fail");
	}

	return 0;
}

int CLogicUser::UpdateProtectedTime(unsigned uid, unsigned protected_time)
{
	if(protected_time == 0)
	{
		protected_time = Time::GetGlobalTime();
	}
	CDataUser dbUser;
	int ret = dbUser.SetProtectedTime(uid, protected_time);
	if(ret != 0)
	{
		error_log("[SetProtectedTime fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("db_set_user_fail");
	}

	return 0;
}

int CLogicUser::UpdateBitInfo(unsigned uid, unsigned bit_info)
{
	CDataUser dbUser;
	int ret = dbUser.SetBitInfo(uid, bit_info);
	if(ret != 0)
	{
		error_log("[SetBitInfo fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("db_set_user_fail");
	}

	return 0;
}
int CLogicUser::ChangeResource(unsigned uid, int r1, int r2, int r3, int r4, int r5)
{
	int ret;
	DataUser user;
	ret = GetUserLimit(uid, user);
	if(ret != 0)
	{
		return ret;
	}
	bool change = false;

#define CHANGE_RESOURCE(res)	\
	if(res != 0)	\
	{	\
		if(res < 0 && user.res < (unsigned)(-res))	\
		{	\
			error_log("[not_enough_resource][uid=%u,resource=%u,change=%d]", uid, user.res, res);	\
			ERROR_RETURN_MSG(R_ERR_DATA_LIMIT, "not_enough_resource");	\
		}	\
		else	\
		{	\
			user.res += res;	\
		}	\
		change = true;	\
	}	\

	CHANGE_RESOURCE(r1);
	CHANGE_RESOURCE(r2);
	CHANGE_RESOURCE(r3);
	CHANGE_RESOURCE(r4);
	CHANGE_RESOURCE(r5);
	if(change)
	{
		ret = SetUserLimit(user);
	}
	return ret;
}

bool CLogicUser::GetFlag(unsigned bit_info, UserFlag flag)
{
	USER_BIT_INFO info;
	info.v = bit_info;
	unsigned res = 0;
	switch(flag)
	{
	case UF_BE_ATTACKED:
		res = info.s.beAttacked;break;
	case UF_BE_CAN_ONLINE_ATTACKED:
		res = info.s.canOnlineAttacked;break;
	default:break;
	}
	return (res == 1?true:false);
}

void CLogicUser::SetFlag(unsigned &bit_info, UserFlag flag, bool value)
{
	USER_BIT_INFO info;
	info.v = bit_info;
	switch(flag)
	{
	case UF_BE_ATTACKED:
		info.s.beAttacked = (value?1:0);break;
	case UF_BE_CAN_ONLINE_ATTACKED:
		info.s.canOnlineAttacked = (value?1:0);break;
	default:break;
	}
	bit_info = info.v;
}

int CLogicUser::UpdateLevel(DataUser &user)
{

	return 0;
}

int CLogicUser::UpdateBreathTime(unsigned uid, unsigned breathTime)
{
	if(breathTime == 0)
	{
		breathTime = Time::GetGlobalTime();
	}
	CDataUser dbUser;
	int ret = dbUser.SetBreathTime(uid, breathTime);
	if(ret != 0)
	{
		error_log("[SetBreathTime fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("db_set_user_fail");
	}

	return 0;
}

int CLogicUser::UpdateActiveExtBreathTime(unsigned uid, unsigned last_active_time, int ext)
{
	unsigned now = Time::GetGlobalTime();
	if (CTime::GetDayInterval(last_active_time, now) != 0)
	{
		ext = 0;
	}
	else if (IsOnlineUser(last_active_time))
	{
		ext += now - last_active_time;
	}
	CDataUser dbUser;
	int ret = dbUser.SetActiveExtBreathTime(uid, now, ext, now);
	if(ret != 0)
	{
		error_log("[UpdateActiveExtBreathTime fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("db_set_user_fail");
	}

	return 0;
}

int CLogicUser::GetUserFlag(unsigned uid, Json::Value &userFlag)
{
	CDataUser userDB;
	string user_flag;
	int ret = userDB.GetUserFlag(uid, user_flag);
	if (ret != 0)
	{
		error_log("[GetUserFlag fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("get_user_fail");
	}
	if (user_flag.empty())
		user_flag = "{}";
	Json::Reader reader;
	if (!reader.parse(user_flag, userFlag))
	{
		error_log("[paser user_flag fail][uid=%u]", uid);
		DB_ERROR_RETURN_MSG("user_flag_error");
	}
	return 0;
}

int CLogicUser::SetUserFlag(unsigned uid, const Json::Value &userFlag)
{
	Json::FastWriter writer;
	string user_flag = writer.write(userFlag);
	CDataUser userDB;
	int ret = userDB.SetUserFlag(uid, user_flag);
	if (ret != 0)
	{
		error_log("[SetUserFlag fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("set_user_fail");
	}
	return 0;
}


int  CLogicUser::GetMainpos(unsigned uid, unsigned &mainpos)
{
	CDataUser userDB;
	int ret = userDB.GetMainPos(uid,mainpos);
	if (ret != 0)
	{
		error_log("[SetMainpos fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("set_user_fail");
	}
	return 0;
}
int  CLogicUser::SetMainpos(unsigned uid, const  unsigned mainpos)
{
	CDataUser userDB;
	int ret = userDB.SetMainPos(uid,mainpos);
	if (ret != 0)
	{
		error_log("[GetMainpos fail][ret=%d,uid=%u]", ret, uid);
		DB_ERROR_RETURN_MSG("set_user_fail");
	}
	return 0;
}

int CLogicUser::TransformVipLevel(int accChargeTotal, int &vipLevel)
{
	if(OpenPlatform::GetType() == PT_VN)
	{
		if (accChargeTotal < 100)
		{
			vipLevel = 0;
		}
		else if (accChargeTotal>=100 && accChargeTotal<500)
		{
			vipLevel = 1;
		}
		else if (accChargeTotal>=500 && accChargeTotal<1000)
		{
			vipLevel = 2;
		}
		else if (accChargeTotal>=1000 && accChargeTotal<5000)
		{
			vipLevel = 3;
		}
		else if (accChargeTotal>=5000 && accChargeTotal<10000)
		{
			vipLevel = 4;
		}
		else if (accChargeTotal>=10000 && accChargeTotal<20000)
		{
			vipLevel = 5;
		}
		else if (accChargeTotal>=20000 && accChargeTotal<50000)
		{
			vipLevel = 6;
		}
		else if (accChargeTotal>=50000 && accChargeTotal<100000)
		{
			vipLevel = 7;
		}
		else if (accChargeTotal>=100000 && accChargeTotal<150000)
		{
			vipLevel = 8;
		}
		else if (accChargeTotal>=150000)
		{
			vipLevel = 9;
		}
	}
	else
	{
		if (accChargeTotal < 100)
		{
			vipLevel = 0;
		}
		else if (accChargeTotal>=100 && accChargeTotal<500)
		{
			vipLevel = 1;
		}
		else if (accChargeTotal>=500 && accChargeTotal<1000)
		{
			vipLevel = 2;
		}
		else if (accChargeTotal>=1000 && accChargeTotal<3000)
		{
			vipLevel = 3;
		}
		else if (accChargeTotal>=3000 && accChargeTotal<5000)
		{
			vipLevel = 4;
		}
		else if (accChargeTotal>=5000 && accChargeTotal<10000)
		{
			vipLevel = 5;
		}
		else if (accChargeTotal>=10000 && accChargeTotal<30000)
		{
			vipLevel = 6;
		}
		else if (accChargeTotal>=30000 && accChargeTotal<50000)
		{
			vipLevel = 7;
		}
		else if (accChargeTotal>=50000 && accChargeTotal<100000)
		{
			vipLevel = 8;
		}
		else if (accChargeTotal>=100000)
		{
			vipLevel = 9;
		}
	}
	return 0;
}

int CLogicUser::GetUserClass(unsigned uid, int &type)
{
	int ret = 0;
	DataUser user;
	ret = GetUserLimit(uid,user);
	if (0 != ret)
		return ret;
	if (user.type == UT_NOT_REAL)
	{
		type = US_CLASS_NOT_REAL;
		return R_SUCCESS;
	}

	if (user.level<20)
	{
		type = US_CLASS_LOW_LEVEL;
	}
	else if (user.level>=20 && user.level<=30)
	{
		if (user.accCharge>=5000)
		{
			type = US_CLASS_PAY_ONE;
		}
		else if (user.accCharge>=1000 && user.accCharge <5000)
		{
			type = US_CLASS_PAY_TOW;
		}
		else if (user.accCharge>0 && user.accCharge<1000)
		{
			type = US_CLASS_PAY_THREE;
		}
		else
		{
			CLogicAttack logicAttack;
			vector<DataAttack> attacks;
			int attactNumber = 0;
			ret = logicAttack.GetAttackHistory(uid, attacks);
			if (0 == ret)
			{
				for (vector<DataAttack>::iterator itr=attacks.begin(); itr!=attacks.end();++itr)
				{
					if (itr->attack_uid == uid)
					{
						++attactNumber;
					}
				}
			}
			if (attactNumber>=5)
			{
				type = US_CLASS_LIKE_WAR;
			}
			else if (user.login_days > 3)
			{
				type = US_CLASS_ACTIVE;
			}
			else if (user.alliance_id != 0)
			{
				type = US_CLASS_IN_ALLIANCE;
			}
			else
			{
				type = US_CLASS_OTHER;
			}
		}
	}
	else if (user.level>30)
	{
		if (user.accCharge>=1000)
		{
			type = US_CLASS_PAY_ONE;
		}
		else if (user.accCharge>=1000 && user.accCharge <10000)
		{
			type = US_CLASS_PAY_TOW;
		}
		else if (user.accCharge>0 && user.accCharge<1000)
		{
			type = US_CLASS_PAY_THREE;
		}
		else
		{
			CLogicAttack logicAttack;
			vector<DataAttack> attacks;
			int attactNumber = 0;
			ret = logicAttack.GetAttackHistory(uid, attacks);
			if (0 == ret)
			{
				for (vector<DataAttack>::iterator itr=attacks.begin(); itr!=attacks.end();++itr)
				{
					if (itr->attack_uid == uid)
					{
						++attactNumber;
					}
				}
			}
			if (attactNumber>=20)
			{
				type = US_CLASS_LIKE_WAR;
			}
			else if (user.login_days > 7)
			{
				type = US_CLASS_ACTIVE;
			}
			else if (user.alliance_id != 0)
			{
				type = US_CLASS_IN_ALLIANCE;
			}
			else
			{
				type = US_CLASS_OTHER;
			}
		}
	}

	return R_SUCCESS;
}
