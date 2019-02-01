#include "LogicInc.h"
#include "LogicCustomServiceAdmin.h"

string CreateCode()
{
	string skey = "";
	//srand(time(NULL) + uid);	//not need
	int range = 62;
	for (int i = 0; i < CODE_SIZE ; i++ )
	{
		int c = rand() % range;
		if(c >= 36)
		{
			c -= 36;
			//skey += CTrans::ITOS(c) ;
			skey += (char)('a' + c);
		}else if(c >= 26)
		{	c -= 26;
			skey += (char)('0' + c);
		}else
		{
			skey += (char)('A' + c);
		}
	}
	return skey;
}
class CCgiAdmin : public CCgiBase
{
public:
	CCgiAdmin() : CCgiBase("admin")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
	}

	CGI_SIMPLE_ACTION_MAP_BEGIN
	CGI_SET_ACTION_MAP("login", Login)
	CGI_SET_ACTION_MAP("logincheck", LoginCheck)
	CGI_SET_ACTION_MAP("autologin", AutoLogin)
	CGI_SET_ACTION_MAP("modifypwd", ModifyPwd)
	CGI_SET_ACTION_MAP("querypay",QueryPay)
	CGI_SET_ACTION_MAP("changecash", ChangeCash)
	CGI_SET_ACTION_MAP("changecoins", ChangeCoins)
	CGI_SET_ACTION_MAP("querybasic", QueryBasic)
	CGI_SET_ACTION_MAP("queryuser", QueryUser)
	CGI_SET_ACTION_MAP("changegc", ChangeGcbase)
	CGI_SET_ACTION_MAP("changer1", ChangeR1)
	CGI_SET_ACTION_MAP("changer2", ChangeR2)
	CGI_SET_ACTION_MAP("changer3", ChangeR3)
	CGI_SET_ACTION_MAP("changer4", ChangeR4)
	CGI_SET_ACTION_MAP("changer5", ChangeR5)
	CGI_SET_ACTION_MAP("changebs", ChangeBattleSpirits)
	CGI_SET_ACTION_MAP("changets", ChangeTutorialStage)
	CGI_SET_ACTION_MAP("changewf", ChangeWfStatus)
	CGI_SET_ACTION_MAP("chgprosper", ChangeProsper)
	CGI_SET_ACTION_MAP("chgblockts", ChangeBlockTs)
	CGI_SET_ACTION_MAP("export", ExportArchive)
	CGI_SET_ACTION_MAP("import", ImportArchive)
	CGI_SET_ACTION_MAP("broadcast", BroadCast)
	CGI_SET_ACTION_MAP("forbidchat", ForbidChat)
	CGI_SET_ACTION_MAP("getexchargenumber", GetExchangeCode)
	CGI_SET_ACTION_MAP("addmysticalequip", AddMysticalEquip)
	CGI_SET_ACTION_MAP("delmysticalequip", DelMysticalEquip)
	CGI_SET_ACTION_MAP("querymysticalequips", QueryMysticalEquips)
	CGI_SET_ACTION_MAP("queryactivitytime", QueryActivityTime)
	CGI_SET_ACTION_MAP("changeactivitytime", ChangeActivityTime)
//	CGI_SET_ACTION_MAP("changegamemanager", ChageGMList)                  //
	CGI_SET_ACTION_MAP("changeblocktsbyopenid", changeBlockTsByOpenId)
	CGI_SET_ACTION_MAP("queryblocktsbyopenid", QueryBlockTsByOpenId)
	CGI_SET_ACTION_MAP("changePoint", ChangePoint)
	CGI_SET_ACTION_MAP("queryinvitelist", QueryInviteList)
	CGI_SET_ACTION_MAP("addacccharge", AddAccCharge)
	CGI_SET_ACTION_MAP("changepo",ChangeExperence)
	CGI_SET_ACTION_MAP("querygamemanager",QueryGM)
	CGI_SET_ACTION_MAP("addgamemanager",AddGM)
	CGI_SET_ACTION_MAP("delgamemanager",DelGM)
	CGI_SET_ACTION_MAP("query_th",Query_th)
	CGI_SET_ACTION_MAP("add_th",Add_th)
	CGI_SET_ACTION_MAP("del_th",Del_th)
	CGI_SET_ACTION_MAP("addattack",AddAttack)
	CGI_SET_ACTION_MAP("querybuilding", QueryBuilding)
	CGI_SET_ACTION_MAP("updatebuilding",UpdateBuilding)
	CGI_SET_ACTION_MAP("changeprotecttime",ChangeProtectTime)
	CGI_SET_ACTION_MAP("copyarchive", CopyArchive)
	CGI_SET_ACTION_MAP("changestars", ChangeStars)    //官职
	CGI_SET_ACTION_MAP("changesoul", ChangeSoul)     //将灵
	CGI_SET_ACTION_MAP("changeqle", ChangeqlE)     //将灵


	CGI_ACTION_MAP_END

	int Login()
	{
		int ret = 0;
		string name = CCGIIn::GetCGIStr("username");
		string password = CCGIIn::GetCGIStr("password");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		string skey;
		int level = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.Login(name, password, skey);
			if (ret != 0)
				return ret;
			m_jsonResult["username"] = name;
			m_jsonResult["skey"] = skey;
		}
		else
		{
			CLogicAdmin logicAdmin;
			ret = logicAdmin.Login(name, password, skey);
			if (ret != 0)
				return ret;

			level = logicAdmin.GetLevel(name);
			m_jsonResult["username"] = name;
			m_jsonResult["skey"] = skey;
			m_jsonResult["level"] = level;
		}

		CGI_SEND_LOG("action=login&name=%s&skey=%s&level=%u", name.c_str(), skey.c_str(),level);
		return 0;
	}

	int LoginCheck(){
			string name = CCGIIn::GetCGIStr("username");
			string skey = CCGIIn::GetCGIStr("skey");
			CLogicAdmin logicAdmin;
			int ret = logicAdmin.LoginCheck(name,skey);
			if(ret!=0)
			{
				return ret;
			}
			return 0 ;
		}

	int AutoLogin()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		if (name.empty() || skey.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int level = logicAdmin.AutoLogin(name, skey);
		m_jsonResult["level"] = level;

		CGI_SEND_LOG("action=autologin&level=%u", level);
		return 0;
	}

	int ModifyPwd()
	{
		string name = CCGIIn::GetCGIStr("username");
		string password = CCGIIn::GetCGIStr("password");
		string newpwd = CCGIIn::GetCGIStr("newpwd");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || newpwd.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.ChangePwd(name, password, newpwd);
			if (0 != ret) return ret;
		}
		else
		{
			CLogicAdmin logicAdmin;
			ret = logicAdmin.ModifyPassword(name, password, newpwd);
			if (ret != 0)
				return ret;
		}
		CGI_SEND_LOG("action=modifypwd&name=%s", name.c_str());
		return 0;
	}

	int QueryPay()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (0 != ret) return ret;
		}
		else
		{
			CLogicAdmin logicAdmin;
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}

		CLogicPay logicPay;
		DataPay pay;
		ret = logicPay.GetPay(uid, pay);
		if (ret != 0)
			return ret;

		m_jsonResult["uid"] = uid;
		m_jsonResult["cash"] = pay.cash;
		m_jsonResult["coins"] = pay.coins;

		CGI_SEND_LOG("action=querypay&name=%s&skey=%s&cash=%u&coins=%u",
				name.c_str(), skey.c_str(), pay.cash, pay.coins);
		return 0;
	}

	int ChangeCash()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int cash = CCGIIn::GetCGIInt("cash");
		string reason = CCGIIn::GetCGIStr("reason");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || cash == CCGIIn::CGI_INT_ERR)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
			if (ret != 0)
				return ret;
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}
		DataPay pay;
		ret = logicAdmin.ChangePay(uid, cash, 0, pay);
		if (ret != 0)
			return ret;

		m_jsonResult["balance"] = pay.cash;
		CGI_SEND_LOG("action=changecash&name=%s&uid=%u&cash=%d&balance=%u", name.c_str(), uid, cash, pay.cash);

		if (1 != custom)
		{
			CLogicAdmin::Log(name, "changecash", reason, uid, CTrans::ITOS(cash));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "changecash", reason, uid, CTrans::ITOS(cash));
		}
		return 0;
	}

	int ChangeCoins()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int coins = CCGIIn::GetCGIInt("coins");
		string reason = CCGIIn::GetCGIStr("reason");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || coins == CCGIIn::CGI_INT_ERR)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
			if (ret != 0)
				return ret;
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}
		DataPay pay;
		ret = logicAdmin.ChangePay(uid, 0, coins, pay);
		if (ret != 0)
			return ret;

		m_jsonResult["balance"] = pay.coins;
		CGI_SEND_LOG("action=changecash&name=%s&uid=%u&coins=%d&balance=%u", name.c_str(), uid, coins, pay.coins);
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "changecoins", reason, uid, CTrans::ITOS(coins));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "changecoins", reason, uid, CTrans::ITOS(coins));
		}
		return 0;
	}

	int QueryBasic()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		if (name.empty() || skey.empty() || openid.empty()
				|| tpt <= PT_UNKNOW || tpt >= PT_MAX)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		PlatformType pt = static_cast<PlatformType>(tpt);
		CLogicAdmin logicAdmin;
		int ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
			return ret;

		DataUserBasic userBasic;
		ret = logicAdmin.QueryUserBasicInfo(openid, pt, userBasic);
		if (ret != 0)
			return ret;
		m_jsonResult["uid"] = userBasic.uid;
		m_jsonResult["platform"] = userBasic.platform;
		m_jsonResult["is_register_platform"] = userBasic.is_register_platform;
		m_jsonResult["open_id"] = userBasic.open_id;
		m_jsonResult["name"] = userBasic.name;
		m_jsonResult["gender"] = userBasic.gender;
		m_jsonResult["nation"] = userBasic.nation;
		m_jsonResult["province"] = userBasic.province;
		m_jsonResult["city"] = userBasic.city;
		m_jsonResult["figure_url"] = userBasic.figure_url;
		m_jsonResult["vip_type"] = userBasic.vip_type;
		m_jsonResult["vip_level"] = userBasic.vip_level;
		m_jsonResult["fnum"] = userBasic.fnum;

		CGI_SEND_LOG("action=querybasic&name=%s&openid=%s&pt=%d", name.c_str(), openid.c_str(), pt);
		return 0;
	}

	int QueryUser()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}

		DataUser user;
		ret = logicAdmin.QueryUser(uid, user);
		if (ret != 0)
			return ret;

		int donate = 0;
		string allianceData("");
		uint64_t alliancePoint = 0;
		Json::Value ext_data;
		DataAlliance allianceDB;
		CLogicAlliance logicAlliance;
		DataAllianceMember allianceMemberDB;

		if(user.alliance_id == 0)
		{
			allianceData = "无联盟";
			alliancePoint = 0;
			donate = 0;
		}else{
			ret = logicAlliance.GetAlliance(user.alliance_id, allianceDB);
			if(ret != 0)
				return ret;
			ret = logicAlliance.GetMember(user.alliance_id, uid, allianceMemberDB);
			if(ret != 0)
				return ret;
			Json::FromString(ext_data, allianceMemberDB.extra_data);
			Json::GetInt(ext_data, "donate", donate);
			allianceData = allianceDB.name;
			alliancePoint = allianceMemberDB.point;

		}

		m_jsonResult["uid"]					= user.uid;
		m_jsonResult["kingdom"]				= user.kingdom;
		m_jsonResult["register_platform"]	= user.register_platform;
		m_jsonResult["register_time"]		= user.register_time;
		m_jsonResult["invite_uid"]			= user.invite_uid;
		m_jsonResult["last_login_platform"]	= user.last_login_platform;
		m_jsonResult["last_login_time"]		= user.last_login_time;
		m_jsonResult["last_active_time"]	= user.last_active_time;
		m_jsonResult["login_times"]			= user.login_times;
		m_jsonResult["login_days"]			= user.login_days;
		m_jsonResult["invite_count"]		= user.invite_count;
		m_jsonResult["alliance"]			= allianceData;
		m_jsonResult["alliance_point"]		= Convert::UInt64ToString(alliancePoint);
		m_jsonResult["donate"]				= donate;
		m_jsonResult["today_invite_count"]	= user.today_invite_count;
		m_jsonResult["status"]				= user.status;
		m_jsonResult["type"]				= user.type;
		m_jsonResult["level"]				= user.level;
		m_jsonResult["point"]				= Convert::UInt64ToString(user.point);
		m_jsonResult["tutorial_stage"]		= user.tutorial_stage;
		m_jsonResult["r1"]					= user.r1;
		m_jsonResult["r1_max"]				= user.r1_max;
		m_jsonResult["r2"]					= user.r2;
		m_jsonResult["r2_max"]				= user.r2_max;
		m_jsonResult["r3"]					= user.r3;
		m_jsonResult["r3_max"]				= user.r3_max;
		m_jsonResult["r4"]					= user.r4;
		m_jsonResult["r4_max"]				= user.r4_max;
		m_jsonResult["r5"]					= user.r5;
		m_jsonResult["r5_max"]				= user.r5_max;
		m_jsonResult["last_save_time"]		= user.last_save_time;
		m_jsonResult["alliance_id"]			= user.alliance_id;
		m_jsonResult["protected_time"]		= user.protected_time;
		m_jsonResult["last_save_uid"]		= user.last_save_uid;
		m_jsonResult["last_breath_time"]	= user.last_breath_time;
		m_jsonResult["gcbase"]				= user.gcbase;
		m_jsonResult["newgcbase"]			= user.newgcbase;
		m_jsonResult["gcbuy"]				= user.gcbuy;
		m_jsonResult["bit_info"]			= user.bit_info;
		m_jsonResult["user_stat"]			= user.user_stat;
		m_jsonResult["user_flag"]			= user.user_flag;
		m_jsonResult["tech"]			= user.user_tech;
		m_jsonResult["barrackQ"]			= user.barrackQ;
		m_jsonResult["soldier"]				= user.soldier;
		m_jsonResult["soldierlevel"]		= user.soldierlevel;
		m_jsonResult["wf_status"]			= user.wf_status;
		m_jsonResult["buildQ"]				= user.buildQ;
		m_jsonResult["skillQ"]				= user.skillQ;
		m_jsonResult["trainQ"]				= user.trainQ;
		m_jsonResult["prosper"]				= user.prosper;
		m_jsonResult["acccharge"]			= user.accCharge;
		m_jsonResult["block_time"]			= user.tribute_time;
		m_jsonResult["close_reason"]		= user.close_reason;
		m_jsonResult["battle_spirits"]		= user.battle_spirits;
		m_jsonResult["cdn"]                 = user.cdn;
		m_jsonResult["kingdom"]             = user.kingdom;
		m_jsonResult["refresh"]             = user.refresh;
		m_jsonResult["memory"]              = user.memory;

		CGI_SEND_LOG("action=queryuser&name=%s&uid=%u", name.c_str(), uid);
		return 0;
	}

	int ChangeGcbase()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int gcbase = CCGIIn::GetCGIInt("gcbase");
		string reason = CCGIIn::GetCGIStr("resonforclose");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || reason.empty() || gcbase == CCGIIn::CGI_INT_ERR)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
			if (ret != 0)
				return ret;
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}

		unsigned balance = 0;
		ret = logicAdmin.ChangeGcbase(uid, gcbase, balance);
		if (ret != 0)
			return ret;

		m_jsonResult["gcbase"] = balance;
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "changegcbase", reason, uid, CTrans::ITOS(gcbase));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "changegcbase", reason, uid, CTrans::ITOS(gcbase));
		}

		CGI_SEND_LOG("action=changegc&name=%s&balance=%u", name.c_str(), balance);
		return 0;
	}

    //经验值
	int ChangeExperence()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int point = CCGIIn::GetCGIInt("point");
		string reason = CCGIIn::GetCGIStr("resonforclose");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || reason.empty() || point == CCGIIn::CGI_INT_ERR)
		{
			PARAM_ERROR_RETURN_MSG("param_error0");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
			if (ret != 0)
				return ret;
		}
		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error1");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error2");
			}
		}

		unsigned balance = 0;
		ret = logicAdmin.ChangePoint1(uid, point, balance);
		if (ret != 0)
			return ret;

		m_jsonResult["point"] = balance;

		if (1 != custom)
		{
			CLogicAdmin::Log(name, "changeexperience", reason, uid, CTrans::ITOS(point));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "changeexperience", reason, uid, CTrans::ITOS(point));
		}
		CGI_SEND_LOG("action=changepo&name=%s&balance=%u", name.c_str(), balance);
		return 0;

	}

	int ChangePoint()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		int point = CCGIIn::GetCGIInt("point");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		string reason = CCGIIn::GetCGIStr("resonforclose");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || reason.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
			if (ret != 0)
				return ret;
		}

		if (!IsValidUid(uid))
		{
			PARAM_ERROR_RETURN_MSG("uid");
		}
		CLogicUser logicUser;
		DataUser user;
		CLogicAlliance logicAlliance;
		ret = logicUser.GetUserLimit(uid,user);
		if(0 == ret && IsAllianceId(user.alliance_id))
		{
			ret = logicAlliance.AddPoint(user.alliance_id,uid,point);
			if (0 != ret)
			{
				LOGIC_ERROR_RETURN_MSG("add_point_fail");
			}
		}
		else
		{
			LOGIC_ERROR_RETURN_MSG("add_point_fail");
		}
		DataAllianceMember allianceMemberDB;
		ret = logicAlliance.GetMember(user.alliance_id, uid, allianceMemberDB);
		if(ret == 0)
		{
			m_jsonResult["alliance_point"] = (unsigned)allianceMemberDB.curr_point;
		}

		if (1 != custom)
		{
			CLogicAdmin::Log(name, "changepoint", reason, uid, CTrans::ITOS(point));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "changepoint", reason, uid, CTrans::ITOS(point));
		}
		return 0;
	}

	int ChangeR1()  //改变粮食的
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int r1 = CCGIIn::GetCGIInt("r1");
		string reason = CCGIIn::GetCGIStr("resonforclose");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || reason.empty() || r1 == CCGIIn::CGI_INT_ERR)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
			if (ret != 0)
				return ret;
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}

		unsigned balance = 0;
		ret = logicAdmin.ChangeR1(uid, r1, balance);
		if (ret != 0)
			return ret;

		m_jsonResult["r1"] = balance;
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "changer1", reason, uid, CTrans::ITOS(r1));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "changer1", reason, uid, CTrans::ITOS(r1));
		}
		CGI_SEND_LOG("action=changer1&name=%s&balance=%u", name.c_str(), balance);
		return 0;
	}

	int ChangeR2()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int r2 = CCGIIn::GetCGIInt("r2");
		string reason = CCGIIn::GetCGIStr("resonforclose");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || reason.empty() || r2 == CCGIIn::CGI_INT_ERR)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
			if (ret != 0)
				return ret;
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}

		unsigned balance = 0;
		ret = logicAdmin.ChangeR2(uid, r2, balance);
		if (ret != 0)
			return ret;

		m_jsonResult["r2"] = balance;

		if (1 != custom)
		{
			CLogicAdmin::Log(name, "changer2", reason, uid, CTrans::ITOS(r2));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "changer2", reason, uid, CTrans::ITOS(r2));
		}
		CGI_SEND_LOG("action=changer2&name=%s&balance=%u", name.c_str(), balance);
		return 0;
	}

	int ChangeR3()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int r3 = CCGIIn::GetCGIInt("r3");
		string reason = CCGIIn::GetCGIStr("resonforclose");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || reason.empty() || r3 == CCGIIn::CGI_INT_ERR)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			int ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
			if (ret != 0)
				return ret;
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}

		unsigned balance = 0;
		ret = logicAdmin.ChangeR3(uid, r3, balance);
		if (ret != 0)
			return ret;

		m_jsonResult["r3"] = balance;
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "changer3", reason, uid, CTrans::ITOS(r3));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "changer3", reason, uid, CTrans::ITOS(r3));
		}
		CGI_SEND_LOG("action=changer3&name=%s&balance=%u", name.c_str(), balance);
		return 0;
	}

	int ChangeR4()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int r4 = CCGIIn::GetCGIInt("r4");
		string reason = CCGIIn::GetCGIStr("resonforclose");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || reason.empty() || r4 == CCGIIn::CGI_INT_ERR)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
			if (ret != 0)
				return ret;
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}

		unsigned balance = 0;
		ret = logicAdmin.ChangeR4(uid, r4, balance);
		if (ret != 0)
			return ret;

		m_jsonResult["r4"] = balance;
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "changer4", reason, uid, CTrans::ITOS(r4));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "changer4", reason, uid, CTrans::ITOS(r4));
		}
		CGI_SEND_LOG("action=changer4&name=%s&balance=%u", name.c_str(), balance);
		return 0;
	}

	int ChangeR5()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int r5 = CCGIIn::GetCGIInt("r5");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || r5 == CCGIIn::CGI_INT_ERR)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
			if (ret != 0)
				return ret;
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}

		unsigned balance = 0;
		ret = logicAdmin.ChangeR5(uid, r5, balance);
		if (ret != 0)
			return ret;

		m_jsonResult["r5"] = balance;
		CGI_SEND_LOG("action=changer5&name=%s&balance=%u", name.c_str(), balance);
		return 0;
	}

	int ChangeBattleSpirits()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int battleSpirits = CCGIIn::GetCGIInt("battle_spirits");
		string reason = CCGIIn::GetCGIStr("resonforclose");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || reason.empty() || battleSpirits == CCGIIn::CGI_INT_ERR)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
			if (ret != 0)
				return ret;
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}

		unsigned balance = 0;
		ret = logicAdmin.ChangeBattleSpirits(uid, battleSpirits, balance);
		if (ret != 0)
			return ret;

		m_jsonResult["battle_spirits"] = balance;
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "changebs", reason, uid, CTrans::ITOS(battleSpirits));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "changebs", reason, uid, CTrans::ITOS(battleSpirits));
		}
		CGI_SEND_LOG("action=ChangeBattleSpirits&name=%s&balance=%u", name.c_str(), balance);
		return 0;
	}

	int ChangeTutorialStage()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int tutorial_stage = CCGIIn::GetCGIInt("tutorial_stage");
		string reason = CCGIIn::GetCGIStr("resonforclose");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || reason.empty() || tutorial_stage == CCGIIn::CGI_INT_ERR)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
			if (ret != 0)
				return ret;
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}

		ret = logicAdmin.ChangeTutorialStage(uid, tutorial_stage);
		if (ret != 0)
			return ret;

		m_jsonResult["tutorial_stage"] = tutorial_stage;
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "changets", reason, uid, CTrans::ITOS(tutorial_stage));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "changets", reason, uid, CTrans::ITOS(tutorial_stage));
		}
		CGI_SEND_LOG("action=changets&name=%s&tutorial_stage=%d", name.c_str(), tutorial_stage);
		return 0;
	}

	int ChangeWfStatus()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		string wf_status = CCGIIn::GetCGIStr("wf_status");
		string reason = CCGIIn::GetCGIStr("resonforclose");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || reason.empty() || wf_status.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
			if (ret != 0)
				return ret;
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}

		ret = logicAdmin.ChangeWfStatus(uid, wf_status);
		if (ret != 0)
			return ret;

		m_jsonResult["wf_status"] = wf_status;
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "changewf", reason, uid, wf_status);
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "changewf", reason, uid, wf_status);
		}
		CGI_SEND_LOG("action=changewf&name=%s&wf_status=%s", name.c_str(), wf_status.c_str());
		return 0;
	}

	int ChangeProsper()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int prosper = CCGIIn::GetCGIInt("prosper");
		string reason = CCGIIn::GetCGIStr("resonforclose");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || reason.empty() || prosper == CCGIIn::CGI_INT_ERR)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
			if (ret != 0)
				return ret;
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}

		unsigned balance = 0;
		ret = logicAdmin.ChangeProsper(uid, prosper, balance);
		if (ret != 0)
			return ret;

		m_jsonResult["prosper"] = balance;
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "chgprosper", reason, uid, CTrans::ITOS(prosper));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "chgprosper", reason, uid, CTrans::ITOS(prosper));
		}
		CGI_SEND_LOG("action=chgprosper&name=%s&balance=%u", name.c_str(), balance);
		return 0;
	}

	int ChangeBlockTs()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		string blockts = CCGIIn::GetCGIStr("blockts");
		string close_reason = CCGIIn::GetCGIStr("resonforclose");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || blockts.empty() || close_reason.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
			if (ret != 0)
				return ret;
		}

		unsigned block_time = (unsigned)CTime::ParseDate(blockts);
		if (block_time == 0)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}

		ret = logicAdmin.ChangeBlockTs(uid, block_time, close_reason);
		if (ret != 0)
			return ret;

		m_jsonResult["block_time"] = block_time;
		m_jsonResult["close_reason"] = close_reason;
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "lockuser", close_reason, uid, CTrans::ITOS(block_time));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "lockuser", close_reason, uid, CTrans::ITOS(block_time));
		}
		CGI_SEND_LOG("action=chgblockts&name=%s&blockts=%s&block_time=%uclose_reason=%s", name.c_str(), blockts.c_str(), block_time, close_reason.c_str());
		return 0;
	}

	int ExportArchive()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		if (name.empty() || skey.empty() || (int)uid == CCGIIn::CGI_INT_ERR)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
			return ret;

		Json::FastWriter writer;
		Json::Value data;
		ret = logicAdmin.ExportArchive(uid, data);
		if (ret != 0)
			return ret;

		ExportData = data;
		m_jsonResult["uid"] = uid;
		m_jsonResult["data"] = writer.write(data);
		CGI_SEND_LOG("action=export&name=%s&uid=%u", name.c_str(), uid);
		return 0;
	}

	int ImportArchive()
	{
		string innerip = "192";
		vector<string> rlt;
		String::Split(m_ipstr, '.', rlt);
		if (rlt[0] == innerip)
		{
			string name = CCGIIn::GetCGIStr("username");
			string skey = CCGIIn::GetCGIStr("skey");
			unsigned uid = CCGIIn::GetCGIInt("uid");
			string sdata = CCGIIn::GetCGIStr("data");
			string reason = CCGIIn::GetCGIStr("reason");
			if (name.empty() || skey.empty() || (int) uid == CCGIIn::CGI_INT_ERR
					|| sdata.empty()) {
				PARAM_ERROR_RETURN_MSG("param_error");
			}

			CLogicAdmin logicAdmin;
			int ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
			if (ret != 0)
				return ret;

			Json::Reader reader;
			Json::Value data;
			if (!reader.parse(sdata, data)) {
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			unsigned userid = 0;
			if (!Json::GetUInt(data, "userid", userid) || userid != uid) {
				PARAM_ERROR_RETURN_MSG("param_error");
			}

			ret = logicAdmin.ImportArchive(uid, data);
			if (ret != 0)
				return ret;
			m_jsonResult["uid"] = uid;
			sdata = Json::ToString(ExportData);
			CGI_SEND_LOG("action=import&name=%s&uid=%u", name.c_str(), uid);
			CLogicAdmin::Log(name, "import", reason, uid, "", sdata);

			return 0;
		}
		return -1;
	}
	int BroadCast()
	{
		int ret = 0;
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		CLogicAdmin logicAdmin;
		ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
			return ret;
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
		if (ret != 0)
			return ret;

		string message = CCGIIn::GetCGIStr("content");
		int repeats = CCGIIn::GetCGIInt("repeats");
		unsigned interval = CCGIIn::GetCGIInt("interval");
		string serverid = CCGIIn::GetCGIStr("serverid");

		if (serverid.empty())
			Config::GetValue(serverid, CONFIG_SERVER_ID);

		vector<string> rlt;
		String::Split(serverid, '-', rlt);
		if (rlt.size() >= 2) {
			unsigned begin = Convert::StringToUInt(rlt[0]);
			unsigned end = Convert::StringToUInt(rlt[1]);
			if (end < begin) {
				error_log("BroadCast error!");
				return ret;
			}

			for (unsigned i = begin; i <= end; ++i) {
				string serverstr = Convert::UIntToString(i);
				ret = logicAdmin.BroadCast(message, repeats, interval,serverstr);
				sleep(3);
				if (0 != ret) {
					error_log("BroadCast error!");
				}
			}
		} else {
			ret = logicAdmin.BroadCast(message, repeats, interval, serverid);
			if (0 != ret)
				error_log("BroadCast error!");
			return ret;
		}

		CGI_SEND_LOG("action=broadcast&repeats = %d&interval = %d&message=%s",repeats,interval,message.c_str());
		return 0;
	}
	int ForbidChat()
	{
		CLogicAdmin logicAdmin;
		int ret = 0;
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
			return ret;
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
		if (ret != 0)
			return ret;

		unsigned uid = CCGIIn::GetCGIInt("uid");
		string deadline = CCGIIn::GetCGIStr("deadline");
		string serverid = CCGIIn::GetCGIStr("serverid");

		unsigned forbidts= (unsigned) CTime::ParseDate(deadline);
		//unsigned block_time = (unsigned) CTime::ParseDate(blockts);
		if (forbidts == 0) {
			PARAM_ERROR_RETURN_MSG("time_format_error");
		}
		info_log("uid = %u,forbidts = %u,deadline =%s",uid,forbidts,deadline.c_str());

		ret  = logicAdmin.AddForbidUser(uid,forbidts,serverid);
		if(ret != 0)
		{
			return ret;
		}
		return 0;
	}
	int GetExchangeCode()
	{
		Json::Value data;
		Json::FastWriter writer;
		//ofstream ofile;
		//unsigned pid = CCGIIn::GetCGIInt("spreadid");
		string type = CCGIIn::GetCGIStr("type");
		if(type.empty())
		{
			type="01";
		}
		int count = CCGIIn::GetCGIInt("count");
		if(!count)
			count = 10;
		int ret = 0;
		DataExchangeCode dataExchangeCode;
		CLogicExchangeCode logicExchangeCode;
		//uint64_t exchangeCode;
		string code;
		int i = 0;
		dataExchangeCode.uid = atoi(Config::GetValue(CONFIG_UID_MIN).c_str());
		dataExchangeCode.type = atoi(type.c_str());
		dataExchangeCode.gentime = Time::GetGlobalTime();
		dataExchangeCode.deadline = dataExchangeCode.gentime + (86400 * 3);
		dataExchangeCode.usetime = 0;
		for(i = 0; i < count; i++)
		{
			code = CreateCode();
			dataExchangeCode.code = code;
			ret = logicExchangeCode.AddExchangeCode(dataExchangeCode);
			if(ret != 0)
			{
				continue;
			}
			data[i]=type + code;
		}
		m_jsonResult["data"] = data;
		return 0;
	}
	int AddMysticalEquip()
	{
		unsigned eid = CCGIIn::GetCGIInt("eid");
		unsigned nums = CCGIIn::GetCGIInt("nums");
		unsigned min = CCGIIn::GetCGIInt("minPrice");
		unsigned max = CCGIIn::GetCGIInt("maxPrice");
		unsigned ratio = CCGIIn::GetCGIInt("ratio");
		if (   (int)eid == CCGIIn::CGI_INT_ERR
			|| (int)nums == CCGIIn::CGI_INT_ERR
			|| (int)min == CCGIIn::CGI_INT_ERR
			|| (int)max == CCGIIn::CGI_INT_ERR
			|| (int)ratio == CCGIIn::CGI_INT_ERR
			)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CDataMysticalShop dataMysticalShop;
		int ret = dataMysticalShop.Init(Config::GetValue(CONFIG_MYSTICALSHOP_PATH));
		if (ret != 0)
		{
			return ret;
		}
		ret = dataMysticalShop.AddEquip(eid, nums, min, max, ratio);
		if (ret != 0)
		{
			return ret;
		}

		return 0;
	}

	int DelMysticalEquip()
	{
		unsigned eid = CCGIIn::GetCGIInt("eid");
		if ((int)eid == CCGIIn::CGI_INT_ERR )
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CDataMysticalShop dataMysticalShop;
		int ret = dataMysticalShop.Init(Config::GetValue(CONFIG_MYSTICALSHOP_PATH));
		if (ret != 0)
		{
			return ret;
		}
		ret = dataMysticalShop.DeleteEquip(eid);
		if (ret != 0)
		{
			return ret;
		}

		return 0;
	}

	int QueryMysticalEquips()
	{
		vector<DataEquipItem> vecDataEquips;
		CDataMysticalShop dataMysticalShop;
		int ret = dataMysticalShop.Init(Config::GetValue(CONFIG_MYSTICALSHOP_PATH));
		if (ret != 0)
		{
			return ret;
		}
		vecDataEquips.clear();
		ret = dataMysticalShop.QueryEquips(vecDataEquips);
		if (ret != 0)
		{
			return ret;
		}
		int count = vecDataEquips.size();
		for (int i = 0; i < count; ++i)
		{
			m_jsonResult["mysticalEquips"][i]["eid"] = vecDataEquips[i].eid;
			m_jsonResult["mysticalEquips"][i]["nums"] = vecDataEquips[i].nums;
			m_jsonResult["mysticalEquips"][i]["min"] = vecDataEquips[i].min;
			m_jsonResult["mysticalEquips"][i]["max"] = vecDataEquips[i].max;
			m_jsonResult["mysticalEquips"][i]["ratio"] = vecDataEquips[i].ratio;
		}
		return 0;
	}

	int QueryActivityTime()
	{
#define GET_UINT_VALUE(name,Jname) \
		ret = logicAdmin.GetActivityTime(name,value); \
		if (0 == ret) \
		{ \
			m_jsonResult[Jname] = value; \
		}

		string valuestring;

#define GET_STRING_VALUE(name,Jname) \
		ret = logicAdmin.GetPlatform(name,valuestring); \
		if (0 == ret) \
		{ \
			m_jsonResult[Jname] = valuestring; \
		}

		int ret = 0;
		CLogicAdmin logicAdmin;
		unsigned value = 0;
		GET_UINT_VALUE(CONFIG_PAY_TOTAL_BEGIN_TS, "PayTotalBeginTs");
		GET_UINT_VALUE(CONFIG_PAY_TOTAL_END_TS, "PayTotalEndTs");
		GET_UINT_VALUE(CONFIG_POINTS_BEGIN_TS, "PointRankBeginTs");
		GET_UINT_VALUE(CONFIG_POINTS_END_TS, "PointRankEndTs");
//		GET_UINT_VALUE(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS, "HundredDaysActivityRankBeginTs");   //百日庆典
//		GET_UINT_VALUE(CONFIG_PAY_OF_HUNDRED_DAYS_END_TS, "HundredDaysActivityRankEndTs");
		GET_UINT_VALUE(CONFIG_POINTS_VERSION, "PointRankVersion");
		GET_UINT_VALUE(CONFIG_PAYRANK_BEGIN_TS, "PayRankBeginTs");
		GET_UINT_VALUE(CONFIG_PAYRANK_END_TS, "PayRankEndTs");
		GET_UINT_VALUE(CONFIG_PAYRANK_VERSION, "PayRankVersion");
		GET_UINT_VALUE("twice_reward_b","ZhuGeLiangB");
		GET_UINT_VALUE("twice_reward_e","ZhuGeLiangE");
		GET_UINT_VALUE(CONFIG_SUNCE_BEGIN_TS,"SunCeB");
		GET_UINT_VALUE(CONFIG_SUNCE_END_TS,"SunCeE");
		GET_UINT_VALUE(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS,"HundBegin");
		GET_UINT_VALUE(CONFIG_PAY_OF_HUNDRED_DAYS_END_TS,"HundEnd");
		GET_UINT_VALUE(CONFIG_MERRYSOULS_BEGIN_TS,"MerrySoulsbeginB");
		GET_UINT_VALUE(CONFIG_MERRYSOULS_END_TS,"MerrySoulsendE");
		GET_UINT_VALUE(CONFIG_MERRYSOULS_VERSION,"MerrySoulsVersion");

		GET_STRING_VALUE(CONFIG_MERRYSOULS_PF,"MerrySoulsPF");
		GET_STRING_VALUE(CONFIG_PAY_OF_HUNDRED_DAYS_PF,"HundDayPF");
		GET_STRING_VALUE(CONFIG_PAY_TOTAL_PF,"PayTotalPF");
		GET_STRING_VALUE(CONFIG_POINTS_PF,"PointsPF");
		GET_STRING_VALUE(CONFIG_PAYRANK_PF,"PayRankPF");
		return 0;
	}
	int ChangeActivityTime()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		if (name.empty() || skey.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
			return ret;
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
		if (ret != 0)
			return ret;

		string PayTotalBeginTs = CCGIIn::GetCGIStr("paytotalbegints");
		string PayTotalEndTs = CCGIIn::GetCGIStr("paytotalendts");
		string PointRankBeginTs = CCGIIn::GetCGIStr("pointrankbegints");
		string PointRankEndTs = CCGIIn::GetCGIStr("pointrankendts");
		string PointRankVersion = CCGIIn::GetCGIStr("pointrankv");
		string PayRankBeginTs = CCGIIn::GetCGIStr("payrankbegints");
		string PayRankEndTs = CCGIIn::GetCGIStr("payrankendts");
		string PayRankVersion = CCGIIn::GetCGIStr("payrankv");
		string ZhuGeLiangB = CCGIIn::GetCGIStr("zhugeliangb");
		string ZhuGeLiangE = CCGIIn::GetCGIStr("zhugeliange");
		string SunCeB = CCGIIn::GetCGIStr("sunceb");
		string SunCeE = CCGIIn::GetCGIStr("suncee");
		string HundPaybegin = CCGIIn::GetCGIStr("hundbegin");
		string HundPayend = CCGIIn::GetCGIStr("hundend");
		string MerrySoulsbegin = CCGIIn::GetCGIStr("merrysoulsbegin");
		string MerrySoulsend = CCGIIn::GetCGIStr("merrysoulsend");
		string MerrySoulsVersion = CCGIIn::GetCGIStr("merrysoulsv");

		string PayTotal_pf = CCGIIn::GetCGIStr("paytotalpf");
		string PointRank_pf = CCGIIn::GetCGIStr("pointspf");
		string PayRank_pf = CCGIIn::GetCGIStr("payrankpf");
		string Hund_pf = CCGIIn::GetCGIStr("hundpf");
		string MerrySouls_pf = CCGIIn::GetCGIStr("merrysoulspf");

#define CHANGE_CONFIG_VALUE(name,value) \
		if(!value.empty()) \
		{ \
			unsigned timeValue = (unsigned)CTime::ParseDate(value); \
			ret = logicAdmin.ChangeActivityTime(name, timeValue); \
			if (0 != ret) \
				return ret; \
			else \
				m_jsonResult[#value] = timeValue; \
		}\

#define CHANGE_CONFIG_PF_VALUE(name,value) \
		if(!value.empty()) \
		{ \
			ret = logicAdmin.ChangePlatform(name, value); \
			if (0 != ret) \
				return ret; \
			else \
				m_jsonResult[#value] = value; \
		}\

		CHANGE_CONFIG_VALUE(CONFIG_PAY_TOTAL_BEGIN_TS, PayTotalBeginTs);
		CHANGE_CONFIG_VALUE(CONFIG_PAY_TOTAL_END_TS, PayTotalEndTs);
		CHANGE_CONFIG_VALUE(CONFIG_POINTS_BEGIN_TS, PointRankBeginTs);
		CHANGE_CONFIG_VALUE(CONFIG_POINTS_END_TS, PointRankEndTs);
		CHANGE_CONFIG_VALUE(CONFIG_PAYRANK_BEGIN_TS, PayRankBeginTs);
		CHANGE_CONFIG_VALUE(CONFIG_PAYRANK_END_TS, PayRankEndTs);
		CHANGE_CONFIG_VALUE("twice_reward_b",ZhuGeLiangB);
		CHANGE_CONFIG_VALUE("twice_reward_e",ZhuGeLiangE);
		CHANGE_CONFIG_VALUE(CONFIG_SUNCE_BEGIN_TS, SunCeB);
		CHANGE_CONFIG_VALUE(CONFIG_SUNCE_END_TS, SunCeE);
		CHANGE_CONFIG_VALUE(CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS, HundPaybegin);
		CHANGE_CONFIG_VALUE(CONFIG_PAY_OF_HUNDRED_DAYS_END_TS, HundPayend);
		CHANGE_CONFIG_VALUE(CONFIG_MERRYSOULS_BEGIN_TS,MerrySoulsbegin);
		CHANGE_CONFIG_VALUE(CONFIG_MERRYSOULS_END_TS,MerrySoulsend);

		CHANGE_CONFIG_PF_VALUE(CONFIG_MERRYSOULS_PF,MerrySouls_pf);
		CHANGE_CONFIG_PF_VALUE(CONFIG_PAY_OF_HUNDRED_DAYS_PF,Hund_pf);
		CHANGE_CONFIG_PF_VALUE(CONFIG_PAY_TOTAL_PF,PayTotal_pf);
		CHANGE_CONFIG_PF_VALUE(CONFIG_POINTS_PF,PointRank_pf);
		CHANGE_CONFIG_PF_VALUE(CONFIG_PAYRANK_PF,PayRank_pf);



		if (!PointRankVersion.empty())
		{
			unsigned timeValue = Convert::StringToUInt(PointRankVersion);
			ret = logicAdmin.ChangeActivityTime(CONFIG_POINTS_VERSION, timeValue);
			if (0 != ret)
				return ret;
			m_jsonResult["PointRankVersion"] = timeValue;
		}

		if (!PayRankVersion.empty())
		{
			unsigned timeValue = Convert::StringToUInt(PayRankVersion);
			ret = logicAdmin.ChangeActivityTime(CONFIG_PAYRANK_VERSION, timeValue);
			if (0 != ret)
				return ret;
			m_jsonResult["PayRankVersion"] = timeValue;
		}

		if (!MerrySoulsVersion.empty())
		{
			unsigned timeValue = Convert::StringToUInt(MerrySoulsVersion);
			ret = logicAdmin.ChangeActivityTime(CONFIG_MERRYSOULS_VERSION, timeValue);
			if (0 != ret)
				return ret;
			m_jsonResult["MerrySoulsVersion"] = timeValue;
		}

		return 0;
	}

	int ChageGMList()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		if (name.empty() || skey.empty() || openid.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
			return ret;
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
		if (ret != 0)
			return ret;

		ret = logicAdmin.AddGm(openid);
		if (0 != ret)
		{
			REFUSE_RETURN_MSG("ChageGMList_fail");
		}
		return 0;
	}

	int QueryGM()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
//		string openid = CCGIIn::GetCGIStr("openid");
		Json::Value data;
		if (name.empty() || skey.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
			return ret;
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
		if (ret != 0)
			return ret;

		ret = logicAdmin.QueryGM(data);
		if (0 != ret)
		{
			REFUSE_RETURN_MSG("ChageGMList_fail");
		}

		m_jsonResult["gmlist"] = data;
		return 0;
	}

	int AddGM()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		if (name.empty() || skey.empty() || openid.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
			return ret;
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
		if (ret != 0)
			return ret;

		ret = logicAdmin.AddGm(openid);
		if (0 != ret)
		{
			REFUSE_RETURN_MSG("ChageGMList_fail");
		}
		CLogicAdmin::Log(name, "add_GM", openid,time(0),skey,"");
		return 0;
	}

	int DelGM()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		if (name.empty() || skey.empty() || openid.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
			return ret;
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
		if (ret != 0)
			return ret;

		ret = logicAdmin.DelGm(openid);
		if (0 != ret)
		{
			REFUSE_RETURN_MSG("ChageGMList_fail");
		}
		CLogicAdmin::Log(name, "del_GM", openid,88,skey,"");
		return 0;
	}

	int Query_th()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		int custom = CCGIIn::GetCGIInt("custom");
		Json::Value data;
		if (name.empty() || skey.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
			if (ret != 0)
				return ret;
		}

		ret = logicAdmin.Query_th(data);
		if (0 != ret)
		{
			REFUSE_RETURN_MSG("Query_th_List_fail");
		}

		m_jsonResult["thlist"] = data;
		return 0;
	}

	int Add_th()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || openid.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
			if (ret != 0)
				return ret;
		}

		ret = logicAdmin.Add_th(openid);
		if (0 != ret)
		{
			REFUSE_RETURN_MSG("ChageTHList_fail");
		}

		if (1 != custom)
		{
			CLogicAdmin::Log(name, "add_th", openid, time(0), skey, "");
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "add_th", openid, time(0), skey, "");
		}
		return 0;

	}

	int Del_th()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || openid.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
			if (ret != 0)
				return ret;
		}

		ret = logicAdmin.Del_th(openid);
		if (0 != ret)
		{
			REFUSE_RETURN_MSG("ChageTHList_fail");
		}
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "del_th", openid,99,skey,"");
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "del_th", openid, 99, skey, "");
		}
		return 0;
	}

	int changeBlockTsByOpenId()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		string blockts = CCGIIn::GetCGIStr("blockts");
		string close_reason = CCGIIn::GetCGIStr("resonforclose");
		if (name.empty() || skey.empty() || openid.empty() || close_reason.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
			return ret;
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
		if (ret != 0)
			return ret;

		unsigned block_time = (unsigned)CTime::ParseDate(blockts);
		ret = logicAdmin.ChangeBlockTs(openid,block_time,close_reason);
		if (0 != ret)
		{
			REFUSE_RETURN_MSG("ChangeBlockTs");
		}
		return 0;
	}
	int QueryBlockTsByOpenId()
	{
		string openid = CCGIIn::GetCGIStr("openid");
		if (openid.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;

		int ret = logicAdmin.GetBlockTs(openid,m_jsonResult);
		if (0 != ret)
		{
			REFUSE_RETURN_MSG("QueryBlockTsByOpenId");
		}
		return 0;
	}


	int QueryInviteList()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		if (name.empty() || skey.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicAdmin logicAdmin;
		int ret = logicAdmin.CheckSession(name, skey);
		if (ret != 0)
			return ret;
		ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
		if (ret != 0)
			return ret;
		ret = logicAdmin.GetInviteList(uid,m_jsonResult["invitelist"]);
		if (ret != 0)
			return ret;
		return 0;
	}

	int AddAccCharge()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int acc = CCGIIn::GetCGIInt("acc");
		string accs = CCGIIn::GetCGIStr("acc");
		string reason = CCGIIn::GetCGIStr("resonforclose");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || reason.empty() || (int) uid == CCGIIn::CGI_INT_ERR
				|| acc == CCGIIn::CGI_INT_ERR) {
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
			if (ret != 0)
				return ret;
		}

		ret = logicAdmin.AddAccCharge(uid, acc);
		if(ret)
			return ret;

		CGI_SEND_LOG("action=AddAccCharge&name=%s&uid=%u", name.c_str(), uid);
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "AddAccCharge", reason, uid, accs, CTrans::ITOS(acc));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "AddAccCharge", reason, uid, accs, CTrans::ITOS(acc));
		}
		return 0;
	}

	int AddAttack()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		unsigned attackuid = CCGIIn::GetCGIInt("attackuid");
		unsigned defenceuid = CCGIIn::GetCGIInt("defenceuid");
		int res = CCGIIn::GetCGIInt("resources");
		string ress = CCGIIn::GetCGIStr("resources");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || (int) attackuid == CCGIIn::CGI_INT_ERR
				|| res == CCGIIn::CGI_INT_ERR) {
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
			if (ret != 0)
				return ret;
		}

		ret = logicAdmin.AddAttack(attackuid, defenceuid,res);
		if(ret)
			return ret;

		CGI_SEND_LOG("action=AddAttack&name=%s&uid=%u", name.c_str(), attackuid);

		if (1 != custom)
		{
			CLogicAdmin::Log(name, "AddAttack", "", attackuid, ress, CTrans::ITOS(defenceuid));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "AddAttack", "", attackuid, ress, CTrans::ITOS(defenceuid));
		}
		return 0;
	}

	int QueryBuilding()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		unsigned uid = CCGIIn::GetCGIUin("uid");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty()) {
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
			if (ret != 0)
				return ret;
		}
		CLogicBuilding logiBuilding;
		ret = logiBuilding.GetBuilding(uid, 0, m_jsonResult["data"], true);
		if (0 != ret)
			return ret;
		return 0;
	}

	int UpdateBuilding()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		int id = CCGIIn::GetCGIInt("bid");
		int level = CCGIIn::GetCGIInt("blv");
		string reason = CCGIIn::GetCGIStr("reason");
		unsigned uid = CCGIIn::GetCGIUin("uid");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty()) {
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
			if (ret != 0)
				return ret;
		}

		ret = logicAdmin.UpdateBuildingLevel(uid, id, level);
		if (0 != ret)
			return ret;
		CGI_SEND_LOG("action=UpdateBuildingLevel&name=%s&uid=%u,id=%d,level=%d",
				name.c_str(), uid, id, level);
		string value = CTrans::ITOS(id) + ":" + CTrans::ITOS(level);
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "UpdateBuildingLevel", reason, uid, value);
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "UpdateBuildingLevel", reason, uid, value);
		}
		return 0;
	}
	int ChangeProtectTime()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		string protime = CCGIIn::GetCGIStr("protecttime");
		string change_reason = CCGIIn::GetCGIStr("resonforclose");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || protime.empty() || change_reason.empty())
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_ALL);
			if (ret != 0)
				return ret;
		}

		unsigned pro_time = (unsigned)CTime::ParseDate(protime);
		if (pro_time == 0)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}

		ret = logicAdmin.ChangeProtectTs(uid, pro_time);
		if (ret != 0)
			return ret;

		m_jsonResult["protect_time"] = pro_time;
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "chgpro_time", change_reason, uid, CTrans::ITOS(pro_time));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "chgpro_time", change_reason, uid, CTrans::ITOS(pro_time));
		}
		CGI_SEND_LOG("action=changeprotecttime&name=%s&protime=%s&pro_time=%change_reason=%s", name.c_str(), protime.c_str(), pro_time, change_reason.c_str());

		return 0;

	}

	int CopyArchive()
	{
		int ret = 0;
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		unsigned fromuid = CCGIIn::GetCGIInt("fromuid");
		unsigned touid = CCGIIn::GetCGIInt("touid");
		string reason = CCGIIn::GetCGIStr("reason");
		if (name.empty() || skey.empty())
		{
			PARAM_ERROR_RETURN_MSG("param");
		}
		CLogicCustomServiceAdmin logicCustomServiceAdmin;
		ret = logicCustomServiceAdmin.CheckSession(name, skey);
		if (ret != 0)
			return ret;
		ret = logicCustomServiceAdmin.CopyArchive(fromuid,touid);
		if (0 != ret)
		{
			return ret;
		}
		CLogicCustomServiceAdmin::Log(name, "CopyArchive", reason, touid, CTrans::ITOS(fromuid));
		CGI_SEND_LOG("action=CopyArchive&name=%s&fromuid=%u&touid=%u&reason=%s",
				name.c_str(), fromuid, touid, reason.c_str());
		return 0;
	}

	int ChangeStars()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int stars = CCGIIn::GetCGIInt("stars");
		string reason = CCGIIn::GetCGIStr("resonforclose");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || reason.empty() || stars == CCGIIn::CGI_INT_ERR)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
			if (ret != 0)
				return ret;
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}

		unsigned balance = 0;
		ret = logicAdmin.ChangeStars(uid, stars, balance);
		if (ret != 0)
			return ret;

		m_jsonResult["stars"] = balance;
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "changestars", reason, uid, CTrans::ITOS(stars));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "changestars", reason, uid, CTrans::ITOS(stars));
		}

		CGI_SEND_LOG("action=changestars&name=%s&balance=%u", name.c_str(), balance);
		return 0;
	}

	int ChangeSoul()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int soul = CCGIIn::GetCGIInt("soul");
		string reason = CCGIIn::GetCGIStr("resonforclose");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || reason.empty() || soul == CCGIIn::CGI_INT_ERR)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
			if (ret != 0)
				return ret;
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}

		unsigned balance = 0;
		ret = logicAdmin.ChangeSoul(uid, soul, balance);
		if (ret != 0)
			return ret;

		m_jsonResult["soul"] = balance;
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "changesoul", reason, uid, CTrans::ITOS(soul));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "changesoul", reason, uid, CTrans::ITOS(soul));
		}

		CGI_SEND_LOG("action=changesoul&name=%s&balance=%u", name.c_str(), balance);
		return 0;
	}

	int ChangeqlE()
	{
		string name = CCGIIn::GetCGIStr("username");
		string skey = CCGIIn::GetCGIStr("skey");
		string openid = CCGIIn::GetCGIStr("openid");
		int tpt = CCGIIn::GetCGIInt("platform");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int qle = CCGIIn::GetCGIInt("qle");
		string reason = CCGIIn::GetCGIStr("resonforclose");
		int custom = CCGIIn::GetCGIInt("custom");
		if (name.empty() || skey.empty() || reason.empty() || qle == CCGIIn::CGI_INT_ERR)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		CLogicAdmin logicAdmin;
		int ret = 0;
		if (1 == custom)
		{
			CLogicCustomServiceAdmin logicCustomServiceAdmin;
			ret = logicCustomServiceAdmin.CheckSession(name, skey);
			if (ret != 0) return ret;
		}
		else
		{
			ret = logicAdmin.CheckSession(name, skey);
			if (ret != 0)
				return ret;
			ret = logicAdmin.CheckLevel(name, ADMIN_LEVEL_1);
			if (ret != 0)
				return ret;
		}

		if (!openid.empty())
		{
			if (tpt <= PT_UNKNOW || tpt >= PT_MAX)
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
			PlatformType pt = static_cast<PlatformType>(tpt);
			CLogicUserBasic userBasic;
			ret = userBasic.GetUid(uid, pt, openid);
			if (ret != 0)
				return ret;
		}
		else
		{
			if (!IsValidUid(uid))
			{
				PARAM_ERROR_RETURN_MSG("param_error");
			}
		}

		unsigned balance = 0;
		ret = logicAdmin.ChangeqlE(uid, qle, balance);
		if (ret != 0)
			return ret;

		m_jsonResult["qle"] = balance;
		if (1 != custom)
		{
			CLogicAdmin::Log(name, "changeqle", reason, uid, CTrans::ITOS(qle));
		}
		else
		{
			CLogicCustomServiceAdmin::Log(name, "changeqle", reason, uid, CTrans::ITOS(qle));
		}

		CGI_SEND_LOG("action=changeqle&name=%s&balance=%u", name.c_str(), balance);
		return 0;
	}


public:
	Json::Value ExportData;
};

CGI_MAIN(CCgiAdmin)
