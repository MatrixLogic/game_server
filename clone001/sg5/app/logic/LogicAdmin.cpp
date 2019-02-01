#include "LogicAdmin.h"
#include "CBroadcast.h"
#include <sys/stat.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

map<string, AdminInfo> CLogicAdmin::adminList;
map<string, AdminSession> CLogicAdmin::adminSession;

int CLogicAdmin::AutoLogin(const string &name, string &skey){
	string url = Config::GetValue(CONFIG_AUTO_LOGIN_URL);
	if(url.empty())
		return 0;
	url += name;
	string response;
	if (!Network::HttpGetRequest(response, url))
		return 0;

	Json::Value result;
	Json::Reader reader;
	if (response.empty() || !reader.parse(response, result)
		|| !result.isMember("skey") || !result["skey"].isString())
		return 0;

	string skey1 = result["skey"].asString();
	if(skey == skey1){
		int level = GetLevel(name);
		if(level){
			AdminSession sess;
			sess.skey = skey;
			sess.activeTime = Time::GetGlobalTime();
			adminSession[name] = sess;

			return level;
		}
	}
	return 0;
}

int CLogicAdmin::UpdateAdminList()
{
	static time_t lastMTime = 0;
	string adminFile = Config::GetValue("admin_list");
	if (adminFile.empty())
	{
		REFUSE_RETURN_MSG("The user not exist");
	}
	struct stat st;
	if (stat(adminFile.c_str(), &st) != 0)
	{
		REFUSE_RETURN_MSG("System error");
	}
	if (lastMTime != st.st_mtime)
	{
		FILE *fp = fopen(adminFile.c_str(), "r");
		if (!fp)
		{
			REFUSE_RETURN_MSG("System error");
		}
		char line[256];
		char *pch, *name, *pwd, *level;
		while (fgets(line, sizeof(line), fp) != NULL)
		{
            name = pwd = level = NULL;
            pch = line;
            while (*pch ==' ' || *pch == '\t') pch++;
            name = pch;
            while (*pch != '\0' && *pch != '\r' && *pch != '\n' && *pch != ' ' && *pch != '\t') pch++;
            if (*pch == '\0' || *pch == '\r' || *pch == '\n') continue;
            *pch++ = '\0';
            while (*pch ==' ' || *pch == '\t') pch++;
            pwd = pch;
            while (*pch != '\0' && *pch != '\r' && *pch != '\n' && *pch != ' ' && *pch != '\t') pch++;
            if (*pch == '\0' || *pch == '\r' || *pch == '\n') continue;
            *pch++ = '\0';
            while (*pch ==' ' || *pch == '\t') pch++;
            level = pch;
            while (*pch != '\0' && *pch != '\r' && *pch != '\n'  && *pch != ' ' && *pch != '\t') pch++;
            if (*pch != '\0') *pch = '\0';

            AdminInfo info;
            info.pwd = pwd;
            info.level = atoi(level);
            adminList[string(name)] = info;
		}
		lastMTime = st.st_mtime;
	}
	return 0;
}

int CLogicAdmin::CheckSession(const string &name, const string &skey)
{
	if (name.empty())
	{
		SESS_ERROR_RETURN_MSG("Name is empty");
	}
	map<string, AdminSession>::iterator it = adminSession.find(name);
	if (it == adminSession.end())
	{
		SESS_ERROR_RETURN_MSG("Session error");
	}
	if (Time::GetGlobalTime() - (it->second).activeTime > 3600)
	{
		SESS_ERROR_RETURN_MSG("Session timeout");
	}
	return 0;
}



int CLogicAdmin::LoginCheck(string const &name,string const &skey){
	map<string, AdminInfo>::iterator it1 = adminList.find(name);
	map<string, AdminSession>::iterator it2 = adminSession.find(name);
	if(it1 == adminList.end()){
		REFUSE_RETURN_MSG("The user not exist");
	}
	if(it2 == adminSession.end()){
		REFUSE_RETURN_MSG("The Session not exist");
	}
	else
	{
		if(it2 -> second.skey != skey)
		{
			REFUSE_RETURN_MSG("The skey not exist");
		}
	}
	return 0;
}

int CLogicAdmin::GetLevel(const string &name)
{
	map<string, AdminInfo>::iterator it = adminList.find(name);
	if (it == adminList.end())
		return ADMIN_LEVEL_0;
	return (it->second).level;
}

int CLogicAdmin::CheckLevel(const string &name, ADMIN_LEVEL needLevel)
{
	if (GetLevel(name) < needLevel)
	{
		REFUSE_RETURN_MSG("Insufficient permissions");
	}
	return 0;
}

int CLogicAdmin::Login(const string &name, const string &passwd, string &skey)
{
	int ret = UpdateAdminList();
	if (ret != 0)
		return ret;
	map<string, AdminInfo>::iterator it = adminList.find(name);
	if (it == adminList.end())
	{
		REFUSE_RETURN_MSG("The user not exist");
	}
	if ((it->second).pwd != passwd)
	{
		REFUSE_RETURN_MSG("Passwd error");
	}

	AdminSession sess;
	sess.skey = Session::CreateSessionKey(0);
	sess.activeTime = Time::GetGlobalTime();
	adminSession[name] = sess;

	skey = sess.skey;
	return 0;
}

int CLogicAdmin::ModifyPassword(const string &name, const string &password, const string &newpwd)
{
	int ret = UpdateAdminList();
	if (ret != 0)
		return ret;
	map<string, AdminInfo>::iterator it = adminList.find(name);
	if (it == adminList.end())
	{
		REFUSE_RETURN_MSG("The user not exist");
	}
	if ((it->second).pwd != password)
	{
		REFUSE_RETURN_MSG("Passwd error");
	}
	(it->second).pwd = newpwd;
	string adminFile = Config::GetValue("admin_list");
	if (adminFile.empty())
	{
		REFUSE_RETURN_MSG("System error");
	}
	FILE *fp = fopen(adminFile.c_str(), "w");
	if (!fp)
	{
		REFUSE_RETURN_MSG("System error");
	}
	for (it = adminList.begin(); it != adminList.end(); it++)
	{
		fprintf(fp, "%s %s %d\n", (it->first).c_str(), (it->second).pwd.c_str(), (it->second).level);
	}
	fclose(fp);

	return 0;
}

int CLogicAdmin::ChangePay(unsigned uid, int cash, int coins, DataPay &pay)
{
	CLogicPay logicPay;
	int ret = logicPay.ChangePay(uid, cash, coins, pay, "ADMINOP");

	if(ret == 0){
		DataPayHistory payhis;
		payhis.channel = PCT_TEST;
		payhis.channel_pay_id = "0";
		payhis.count = 1;
		payhis.credit = cash + coins;
		payhis.status = PST_OK;
		payhis.type = 0;
		payhis.uid = uid;
		payhis.open_id = "gm";
		ret = logicPay.AddPayHistory(payhis);
		if (ret != 0)
		{
			error_log("[AddPayHistory fail][ret=%d,openid=%s,billno=%s,payitem=%s,amt=%d]",
					ret,"gm","0","0",cash + coins);
		}
	}
	return ret;
}

int CLogicAdmin::QueryUserInfo(unsigned uid, PlatformType pt, DataUserBasic &userBasic, DataUser &user)
{
	CLogicUserBasic logicUserBasic;
	int ret = logicUserBasic.GetUserBasicLimit(uid, pt, userBasic);
	if (ret != 0)
		return ret;
	CLogicUser logicUser;
	ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	return 0;
}

int CLogicAdmin::QueryUserBasicInfo(const string &openid, PlatformType pt, DataUserBasic &userBasic)
{
	CLogicUserBasic logicUserBasic;
	unsigned uid = 0;
	int ret = logicUserBasic.GetUid(uid, pt, openid, false);
	if (ret != 0)
		return ret;
	ret = logicUserBasic.GetUserBasicLimit(uid, pt, userBasic);
	if (ret != 0)
		return ret;
	return 0;
}

int CLogicAdmin::QueryUser(unsigned uid, DataUser &user)
{
	CLogicUser logicUser;
	return logicUser.GetUser(uid, user);
}

int CLogicAdmin::ChangeStars(unsigned uid, int stars, unsigned &balance)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUser(uid,user);
	if(ret != 0 )
	{
		return ret;
	}

	string user_stat = user.user_stat;
	Json::Value stat;
	Json::FromString(stat, user_stat);
	int old_stars = 0;
	if(!Json::GetInt(stat, "stars", old_stars))
	{
		return R_ERR_PARAM;
	}

	balance = old_stars + stars;

	stat["stars"] = balance;
	string new_stat =Json::ToString(stat);
	user.user_stat = new_stat;
	ret = logicUser.SetUser(uid,user);
	if(ret != 0 )
	{
		return ret;
	}
	return 0;

}

int CLogicAdmin::ChangeSoul(unsigned uid, int soul, unsigned &balance)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUser(uid,user);
	if(ret != 0 )
	{
		return ret;
	}

	string user_tech = user.user_tech;
	Json::Value tech;
	Json::FromString(tech, user_tech);
	int old_tech = 0;
	if(!Json::GetInt(tech, "soul", old_tech))
	{
		return R_ERR_PARAM;
	}

	balance = old_tech + soul;

	tech["soul"] = balance;
	string new_tech =Json::ToString(tech);
	user.user_tech = new_tech;
	ret = logicUser.SetUser(uid,user);
	if(ret != 0 )
	{
		return ret;
	}
	return 0;
}

int CLogicAdmin::ChangeqlE(unsigned uid, int qle, unsigned &balance)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUser(uid,user);
	if(ret != 0 )
	{
		return ret;
	}

	string user_tech = user.user_tech;
	Json::Value tech;
	Json::FromString(tech, user_tech);
	int old_tech = tech["godE"]["qlE"][0u].asInt();

	balance = old_tech + qle;

	tech["godE"]["qlE"][0u] = balance;

	string new_tech =Json::ToString(tech);
	user.user_tech = new_tech;
	ret = logicUser.SetUser(uid,user);
	if(ret != 0 )
	{
		return ret;
	}
	return 0;
}

int CLogicAdmin::ChangeGcbase(unsigned uid, int gcbase, unsigned &balance)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	int temp = gcbase + user.gcbase;
	if (temp < 0)
	{
		LOGIC_ERROR_RETURN_MSG("balance less than zero");
	}
	user.gcbase = temp;
	ret = logicUser.SetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	balance = user.gcbase;
	return 0;
}


//经验值
int CLogicAdmin::ChangePoint1(unsigned uid, int point, unsigned &balance)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	int temp = point + user.point;
	if (temp < 0)
	{
		LOGIC_ERROR_RETURN_MSG("balance less than zero");
	}
	user.point = temp;
	ret = logicUser.SetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	balance = user.point;
	return 0;
}

int CLogicAdmin::ChangeR1(unsigned uid, int r1, unsigned &balance)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	int temp = r1 + user.r1;
	if (temp < 0)
	{
		LOGIC_ERROR_RETURN_MSG("balance less than zero");
	}
	user.r1 = temp;
	ret = logicUser.SetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	balance = user.r1;
	return 0;
}

int CLogicAdmin::ChangeR2(unsigned uid, int r2, unsigned &balance)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	int temp = r2 + user.r2;
	if (temp < 0)
	{
		LOGIC_ERROR_RETURN_MSG("balance less than zero");
	}
	user.r2 = temp;
	ret = logicUser.SetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	balance = user.r2;
	return 0;
}

int CLogicAdmin::ChangeR3(unsigned uid, int r3, unsigned &balance)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	int temp = r3 + user.r3;
	if (temp < 0)
	{
		LOGIC_ERROR_RETURN_MSG("balance less than zero");
	}
	user.r3 = temp;
	ret = logicUser.SetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	balance = user.r3;
	return 0;
}

int CLogicAdmin::ChangeR4(unsigned uid, int r4, unsigned &balance)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	int temp = r4 + user.r4;
	if (temp < 0)
	{
		LOGIC_ERROR_RETURN_MSG("balance less than zero");
	}
	user.r4 = temp;
	ret = logicUser.SetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	balance = user.r4;
	return 0;
}

int CLogicAdmin::ChangeR5(unsigned uid, int r5, unsigned &balance)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	int temp = r5 + user.r5;
	if (temp < 0)
	{
		LOGIC_ERROR_RETURN_MSG("balance less than zero");
	}
	user.r5 = temp;
	ret = logicUser.SetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	balance = user.r5;
	return 0;
}

int CLogicAdmin::ChangeBattleSpirits(unsigned uid, int battleSpirits, unsigned &balance)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	int temp = battleSpirits + user.battle_spirits;
	if (temp < 0)
	{
		LOGIC_ERROR_RETURN_MSG("balance less than zero");
	}
	user.battle_spirits = temp;
	ret = logicUser.SetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	balance = user.battle_spirits;
	return 0;
}

int CLogicAdmin::ChangeTutorialStage(unsigned uid, int tutorial_stage)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	user.tutorial_stage = tutorial_stage;
	ret = logicUser.SetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	return 0;
}

int CLogicAdmin::ChangeWfStatus(unsigned uid, const string &wf_status)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUser(uid, user);
	if (ret != 0)
		return ret;
	Json::Value wf;
	Json::Reader reader;
	if (wf_status.empty() || !reader.parse(wf_status, wf))
	{
		PARAM_ERROR_RETURN_MSG("wf_status format error");
	}
	Json::FastWriter writer;
	user.wf_status = writer.write(wf);
	ret = logicUser.SetUser(uid, user);
	if (ret != 0)
		return ret;
	return 0;
}

int CLogicAdmin::ChangeProsper(unsigned uid, int prosper, unsigned &balance)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	int temp = prosper + user.prosper;
	if (temp < 0)
	{
		LOGIC_ERROR_RETURN_MSG("balance less than zero");
	}
	user.prosper = temp;
	ret = logicUser.SetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	balance = user.prosper;
	return 0;
}

int CLogicAdmin::ChangeBlockTs(unsigned uid, unsigned block_time, const string &close_reason)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	user.tribute_time = block_time;
	user.close_reason = close_reason;
	ret = logicUser.SetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	return 0;
}

int CLogicAdmin::ChangeProtectTs(unsigned uid, unsigned pro_time)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	user.protected_time = pro_time;
	ret = logicUser.SetUserLimit(uid, user);
	if (ret != 0)
		return ret;
	return 0;
}

int CLogicAdmin::ChangeBlockTs(const string &openid,unsigned block_time, const string &close_reason)
{
	int ret = 0;

	vector<unsigned> uids;
	CDataUserMapping dbUserMapping;
	ret = dbUserMapping.GetAllMapping(openid,uids);
	if(ret == 0){
		for(int i=0;i<uids.size();++i)
			AddForbidUser(uids[i],block_time);
			sleep(3);
	}

	CDataBlackUser dbBlackUser;
	DataBlackUser blackUser;
	ret = dbBlackUser.GetBlackUser(openid, blackUser);
	if (0 == ret)
	{
		blackUser.block_time = block_time;
		blackUser.close_reason = close_reason;
		ret = dbBlackUser.SetBlackUser(blackUser);
		if (0 != ret)
		{
			error_log("[SetBlackUser fail][ret=%d,openid=%s]", ret,openid.c_str());
			DB_ERROR_RETURN_MSG("set_black_user_fail!");
		}
		return 0;
	}
	else if (R_ERR_NO_DATA == ret)
	{
		blackUser.open_id = openid;
		blackUser.close_reason = close_reason;
		blackUser.block_time = block_time;
		ret = dbBlackUser.AddBlackUser(blackUser);
		if (0 != ret)
		{
			error_log("[AddBlackUser fail][ret=%d,openid=%s]", ret,openid.c_str());
			DB_ERROR_RETURN_MSG("add_black_user_fail!");
		}
		return 0;
	}
	return R_ERROR;
}

int CLogicAdmin::GetBlockTs(const string &openid, Json::Value &data)
{
	CDataBlackUser dbBlackUser;
	DataBlackUser blackUser;
	int ret = dbBlackUser.GetBlackUser(openid, blackUser);
	if (0 == ret)
	{
		data["blockts"] = blackUser.block_time;
		data["close_reson"] = blackUser.close_reason;
		return 0;
	}
	else if (R_ERR_NO_DATA == ret)
	{
		data["blockts"] = 0;
		data["close_reson"] = "";
		return 0;
	}
	return ret;
}

int CLogicAdmin::ExportArchive(unsigned uid, Json::Value &result)
{
	Json::Reader reader;
	result["userid"] = uid;

	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUser(uid, user);
	if (ret != 0)
		return ret;
	result["kingdom"] = user.kingdom;
	result["invite_count"] = user.invite_count;
	result["today_invite_count"] = user.today_invite_count;
	result["level"] = user.level;
	result["exp"] = Convert::UInt64ToString(user.point);
	result["tutorialstage"] = user.tutorial_stage;
	result["resources"].resize(5);
	result["resources"][(unsigned)0]["c"] = user.r1;
	result["resources"][(unsigned)0]["m"] = user.r1_max;
	result["resources"][(unsigned)1]["c"] = user.r2;
	result["resources"][(unsigned)1]["m"] = user.r2_max;
	result["resources"][(unsigned)2]["c"] = user.r3;
	result["resources"][(unsigned)2]["m"] = user.r3_max;
	result["resources"][(unsigned)3]["c"] = user.r4;
	result["resources"][(unsigned)3]["m"] = user.r4_max;
	result["resources"][(unsigned)4]["c"] = user.r5;
	result["resources"][(unsigned)4]["m"] = user.r5_max;
	result["currencys"][(unsigned)0] = user.gcbase;
	result["currencys"][(unsigned)1] = user.gcbuy;
	result["currencys"][(unsigned)2] = user.prosper;
	result["lasttime"] = user.last_save_time;
	result["newgcbase"] = user.newgcbase;
	result["protected_time"] = user.protected_time;
	result["acccharge"] = user.accCharge;
	result["lastseq"] = user.lastseq;
	if (!user.user_stat.empty() && !reader.parse(user.user_stat, result["stats"]))
	{
		//DB_ERROR_RETURN_MSG("parse_stats_fail");
	}
	if (!user.user_flag.empty() && !reader.parse(user.user_flag, result["userflag"]))
	{
		//DB_ERROR_RETURN_MSG("parse_user_flag_fail");
	}
	if (!user.user_tech.empty() && !reader.parse(user.user_tech, result["tech"]))
	{
		DB_ERROR_RETURN_MSG("parse_user_tech_fail");
	}
	if (!user.barrackQ.empty() && !reader.parse(user.barrackQ, result["barrackQ"]))
	{
		//DB_ERROR_RETURN_MSG("parse_barrackQ_fail");
	}
	if (!user.soldier.empty() && !reader.parse(user.soldier, result["soldier"]))
	{
		//DB_ERROR_RETURN_MSG("parse_soldier_fail");
	}
	if (!user.soldierlevel.empty() && !reader.parse(user.soldierlevel, result["soldierlevel"]))
	{
		//DB_ERROR_RETURN_MSG("parse_soldierlevel_fail");
	}
	if (!user.wf_status.empty() && !reader.parse(user.wf_status, result["wf_status"]))
	{
		//DB_ERROR_RETURN_MSG("parse_wf_status_fail");
	}
	if (!user.buildQ.empty() && !reader.parse(user.buildQ, result["buildQ"]))
	{
		//DB_ERROR_RETURN_MSG("parse_buildQ_fail");
	}
	if (!user.skillQ.empty() && !reader.parse(user.skillQ, result["skillQ"]))
	{
		//DB_ERROR_RETURN_MSG("parse_skillQ_fail");
	}
	if (!user.trainQ.empty() && !reader.parse(user.trainQ, result["trainQ"]))
	{
		//DB_ERROR_RETURN_MSG("parse_trainQ_fail");
	}
	if (!user.alliance_tech.empty() && !reader.parse(user.alliance_tech, result["alliance_tech"]))
	{
		//DB_ERROR_RETURN_MSG("alliance_tech");
	}
	if (!user.vipstat.empty() && !reader.parse(user.vipstat, result["game_vip_state"]))
	{
		//DB_ERROR_RETURN_MSG("game_vip_state");
	}
	if (!user.glory.empty() && !reader.parse(user.glory,result["md"]))
	{
		//DB_ERROR_RETURN_MSG("md");
	}

	//building
	CLogicBuilding logicBuiding;
	ret = logicBuiding.GetBuilding(uid,0, result["baseop"],true); //导出主城建筑
	//if (ret != 0)
	//	return ret;
	//hero
	CLogicHero logicHero;
	ret = logicHero.GetHero(uid, result["hero"]);
	//if (ret != 0)
	//	return ret;
	//equipment
	CLogicEquipment logicEquip;
	ret = logicEquip.GetEquipment(uid, result["equipment"]);
	//if (ret != 0)
	//	return ret;
	CLogicQuest logicQuest;
	ret = logicQuest.GetQuest(uid, result["quest"]);
	//if (ret != 0)
	//	return ret;
	CLogicGate logicGate;
	ret = logicGate.GetGate(uid, result["gate"]);
	//if (ret != 0)
	//	return ret;
	DataUserData userData;
	CLogicUserData logicUserData;
	ret = logicUserData.GetUserData(uid, userData);
	//if (ret != 0)
	//	return ret;
	if (!userData.npcattack.empty() && !reader.parse(userData.npcattack, result["npcattack"]))
	{
		//DB_ERROR_RETURN_MSG("npcattack_parse_fail");
	}
	if (!userData.gift.empty() && !reader.parse(userData.gift, result["gift"]))
	{
		//DB_ERROR_RETURN_MSG("gift_parse_fail");
	}
	if (!userData.dailyquest.empty() && !reader.parse(userData.dailyquest, result["dailyquest"]))
	{
		//DB_ERROR_RETURN_MSG("dailyquest_parse_fail");
	}
	CLogicWeapon logicWeapon;
	ret = logicWeapon.GetWeapon(uid, result);
	if (0 != ret)
	{
		//DB_ERROR_RETURN_MSG("get godfb fail!");
	}
	return 0;
}

int CLogicAdmin::ImportArchive(unsigned uid, const Json::Value &data)
{
	Json::FastWriter writer;
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUser(uid, user);
	if (ret != 0)
		return ret;
	DataUserData userData;
	CLogicUserData logicUserData;
	ret = logicUserData.GetUserData(uid, userData);
	if (ret != 0)
		return ret;
	// user
	if (data.isMember("stats"))
	{
		user.user_stat = writer.write(data["stats"]);
	}
	if (data.isMember("userflag"))
	{
		user.user_flag = writer.write(data["userflag"]);
	}
	if (data.isMember("tech"))
	{
		user.user_tech = writer.write(data["tech"]);
	}
	if (data.isMember("barrackQ"))
	{
		user.barrackQ = writer.write(data["barrackQ"]);
	}
	if (data.isMember("soldier"))
	{
		user.soldier = writer.write(data["soldier"]);
	}
	if (data.isMember("soldierlevel"))
	{
		user.soldierlevel = writer.write(data["soldierlevel"]);
	}
	if (data.isMember("wf_status"))
	{
		user.wf_status = writer.write(data["wf_status"]);
	}
	if (data.isMember("buildQ"))
	{
		user.buildQ = writer.write(data["buildQ"]);
	}
	if (data.isMember("skillQ"))
	{
		user.skillQ = writer.write(data["skillQ"]);
	}
	if (data.isMember("trainQ"))
	{
		user.trainQ = writer.write(data["trainQ"]);
	}
	if (data.isMember("alliance_tech"))
	{
		user.alliance_tech = writer.write(data["alliance_tech"]);
	}
	if (data.isMember("game_vip_state"))
	{
		user.vipstat = writer.write(data["game_vip_state"]);
	}
	if (data.isMember("md"))
	{
		user.glory = writer.write(data["md"]);
	}

	Json::GetUInt(data, "kingdom", user.kingdom);
	Json::GetInt(data, "invite_count", user.invite_count);
	Json::GetInt(data, "today_invite_count", user.today_invite_count);
	Json::GetInt(data, "tutorialstage", user.tutorial_stage);
	Json::GetInt(data, "level", user.level);
	Json::GetUInt64(data, "exp", user.point);
	Json::GetUInt(data, "gcbase", user.gcbase);
	Json::GetUInt(data, "newgcbase", user.newgcbase);
	Json::GetUInt(data, "gcbuy", user.gcbuy);
	Json::GetUInt(data, "lasttime", user.last_save_time);
	Json::GetUInt(data, "protected_time", user.protected_time);
	Json::GetUInt(data, "prosper", user.prosper);
	Json::GetUInt(data, "acccharge", user.accCharge);
	if (Json::IsArray(data, "currencys"))
	{
		if (data["currencys"].size() > 0)
		{
			Json::GetUInt(data["currencys"], (unsigned)0, user.gcbase);
		}
		if (data["currencys"].size() > 1)
		{
			Json::GetUInt(data["currencys"], (unsigned)1, user.gcbuy);
		}
		if (data["currencys"].size() > 2)
		{
			Json::GetUInt(data["currencys"], (unsigned)2, user.prosper);
		}
	}
	if (Json::IsArray(data, "resources"))
	{
		if (data["resources"].size() > 0)
		{
			Json::GetUInt(data["resources"][(unsigned)0], "c", user.r1);
			Json::GetUInt(data["resources"][(unsigned)0], "m", user.r1_max);
		}
		if (data["resources"].size() > 1)
		{
			Json::GetUInt(data["resources"][(unsigned)1], "c", user.r2);
			Json::GetUInt(data["resources"][(unsigned)1], "m", user.r2_max);
		}
		if (data["resources"].size() > 2)
		{
			Json::GetUInt(data["resources"][(unsigned)2], "c", user.r3);
			Json::GetUInt(data["resources"][(unsigned)2], "m", user.r3_max);
		}
		if (data["resources"].size() > 3)
		{
			Json::GetUInt(data["resources"][(unsigned)3], "c", user.r4);
			Json::GetUInt(data["resources"][(unsigned)3], "m", user.r4_max);
		}
		if (data["resources"].size() > 4)
		{
			Json::GetUInt(data["resources"][(unsigned)4], "c", user.r5);
			Json::GetUInt(data["resources"][(unsigned)4], "m", user.r5_max);
		}
	}
	ret = logicUser.SetUser(uid, user);
	if (ret != 0)
		return ret;

	//building
	if (Json::IsArray(data, "baseop"))
	{
		CDataBuilding bdDB;
		CLogicBuilding logicBuilding;
		logicBuilding.RemoveBuilding(uid,0);
		//ret = bdDB.RemoveBuilding(uid);
		if (ret != 0)
		{
			DB_ERROR_RETURN_MSG("rm_building_fail");
		}
		//CLogicBuilding logicBuilding;
		ret = logicBuilding.ImportBuilding(uid, 0,data["baseop"],true);
		if (ret != 0)
			return ret;
	}
	//hero
	if (Json::IsArray(data, "hero"))
	{
		CDataHero heroDB;
		ret = heroDB.RemoveHero(uid);
		if (ret != 0)
		{
			DB_ERROR_RETURN_MSG("rm_hero_fail");
		}
		CLogicHero logicHero;
		ret = logicHero.ImportHero(uid, data["hero"]);
		if (ret != 0)
			return ret;
	}
	//hero
	if (Json::IsArray(data, "equipment"))
	{
		CDataEquipment eqDB;
		ret = eqDB.RemoveEquipment(uid);
		if (ret != 0)
		{
			DB_ERROR_RETURN_MSG("rm_equipment_fail");
		}
		CLogicEquipment logicEquip;
		ret = logicEquip.UpdateEquipment(uid, data["equipment"]);
		if (ret != 0)
			return ret;
	}
	//quest
	if (Json::IsArray(data, "quest"))
	{
		CDataQuest questDB;
		ret = questDB.RemoveQuest(uid);
		if (ret != 0)
		{
			DB_ERROR_RETURN_MSG("rm_quest_fail");
		}
		CLogicQuest logicQuest;
		ret = logicQuest.UpdateQuest(uid, data["quest"]);
		if (ret != 0)
			return ret;
	}
	//gate
	if (Json::IsArray(data, "gate"))
	{
		CDataGate gateDB;
		ret = gateDB.RemoveGate(uid);
		if (ret != 0)
		{
			DB_ERROR_RETURN_MSG("rm_gate_fail");
		}
		CLogicGate logicGate;
		ret = logicGate.UpdateGate(uid, data["gate"]);
		if (ret != 0)
			return ret;
	}
	//user data
	if (data.isMember("npcattack"))
	{
		ret = logicUserData.SetNpcattack(uid, data["npcattack"]);
		if (ret != 0)
			return ret;
	}
	if (data.isMember("gift"))
	{
		ret = logicUserData.SetGift(uid, data["gift"]);
		if (ret != 0)
			return ret;
	}
	if (data.isMember("dailyquest"))
	{
		ret = logicUserData.SetDailyQuest(uid, data["dailyquest"]);
		if (ret != 0)
			return ret;
	}
	if (data.isMember("godfb"))
	{
		CLogicWeapon logicWeapon;
		logicWeapon.SetWeapon(uid, data["godfb"]);
	}
	return 0;
}

void CLogicAdmin::Log(const string &admin, const string &op, const string &reason,
		unsigned uid, const string &value, const string &detail)
{
	string dir;
	if (!Config::GetValue(dir, ADMIN_OP_LOG_DIR) || dir.empty())
		return;
	if (dir[dir.length()-1] != '/') dir += "/";
	char buf[256];
	time_t now;
	time( &now );
	struct tm *ptm;
	ptm = localtime(&now);
	snprintf(buf, sizeof(buf), "%sop_%04d%02d%02d.log", dir.c_str(), ptm->tm_year+1900,
			ptm->tm_mon+1, ptm->tm_mday);
	FILE *fp = fopen(buf, "a");
	if (!fp) return;
	fprintf(fp, "%04d%02d%02d %02d:%02d:%02d|%s|%s|%s|%u|%s\n", ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,
			ptm->tm_hour,ptm->tm_min,ptm->tm_sec,admin.c_str(),op.c_str(),reason.c_str(),uid,value.c_str());
	fclose(fp);
	if (!detail.empty())
	{
		snprintf(buf, sizeof(buf), "%sdetail_%04d%02d%02d.log", dir.c_str(), ptm->tm_year+1900,
				ptm->tm_mon+1, ptm->tm_mday);
		fp = fopen(buf, "a");
		if (!fp) return;
		fprintf(fp, "%04d%02d%02d %02d:%02d:%02d|%s|%s|%u|%s\n", ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,
				ptm->tm_hour,ptm->tm_min,ptm->tm_sec,admin.c_str(),op.c_str(),uid,detail.c_str());
		fclose(fp);
	}
}

int CLogicAdmin::BroadCast(string message, int repeats, int interval,string serverid)
{
	string splatform = Convert::UIntToString(OpenPlatform::GetType());
	string tcpserver = Config::GetValue(CONFIG_TCP_HOST);
	if(serverid.empty())
		Config::GetValue(serverid, CONFIG_SERVER_ID);


	CPacket1 cpacket;
	cpacket.UserId = 1999999999;
	cpacket.ProtocolType = PROTOCOL_TYPE_LOGIN;
	cpacket.MainProtocolType = 1;
	cpacket.SubProtocolType = 1;
	cpacket.PacketType = PT_CSRequest;
	cpacket.ProtocolBody.Clear();

	CLoginSend cloginsend;
	cloginsend.name = "{\"name\":\"系统管理员\",\"hname\":\"\",\"hlvl\":0}";
	cloginsend.platform = (byte)splatform[0];
	error_log("platform = %u, name = %s, server=%s",cloginsend.platform, cloginsend.name.c_str(),serverid.c_str());

	if (!cloginsend.Encode(&cpacket.ProtocolBody))
	{
		error_log(" body encode failed");
		return R_ERR_DATA;
	}

	CStaticBuffer<1000> buffer;
	buffer.Clear();
	if (!cpacket.Encode(&buffer))
	{
		error_log(" packet encode failed");
		return R_ERR_DATA;
	}

	CSocket socket;
	struct sockaddr_in server;
	if (!socket.Socket(SOCK_STREAM, 0, AF_INET))
	{
		error_log("create socket error");
		return R_ERROR;
	}

	size_t pos = tcpserver.find(':');
	string host = tcpserver.substr(0, pos);
	string port = tcpserver.substr(pos + 1, tcpserver.length() - pos - 1);
	socket.Socket(SOCK_STREAM, 0, AF_INET);
	server.sin_addr.s_addr = inet_addr(host.c_str());
	server.sin_port = htons(atoi(port.c_str()));
	server.sin_family = AF_INET;
	memset(server.sin_zero, 0x00, sizeof(server.sin_zero));

	if (!socket.Connect((struct sockaddr *) &server,sizeof(struct sockaddr)))
	{
		error_log("connect socket error,addr = %s, port=%s,errno = %d, errmsg = %s",
				host.c_str(),port.c_str(),errno, strerror(errno));
		socket.Close();
		return R_ERR_REFUSE;
	}

	struct timeval timeout;
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	if (!socket.SetSockOpt(SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout,
			sizeof(struct timeval))
			|| !socket.SetSockOpt(SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout,
					sizeof(struct timeval)))
		error_log("set time out wrong");

	string firstpack = "tgw_l7_forward\r\nHost: " + tcpserver + " serverid:"
			+ serverid + "\r\n\r\n";
	int firstpackLen = firstpack.length();
	byte packbyte[firstpackLen];
	memset(packbyte, 0x00, firstpackLen);
	memcpy(packbyte, firstpack.c_str(), firstpackLen);
	if (!(socket.Send(packbyte, firstpackLen, 0))) {
		error_log("send socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}

	if (!(socket.Send(buffer.GetConstBuffer(), buffer.GetSize(), 0)))
	{
		error_log("send socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}

	unsigned int recvsize = 0;
	buffer.Clear();
	recvsize = socket.Receive(buffer.GetNativeBuffer(),
	buffer.GetFreeCapacity());
	if (recvsize < 0)
	{
		error_log("receive socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}

	buffer.SetSize(recvsize);
	CPacket1 loginreply;
	if (!loginreply.Decode(&buffer))
	{
		error_log("cpacket decode error");
		socket.Close();
		return R_ERR_DATA;
	}

	CLoginReceive cloginrec;
	if (!cloginrec.Decode(&(loginreply.ProtocolBody)))
	{
		error_log("decode error");
		socket.Close();
		return R_ERR_DATA;
	}

	info_log("clogicrec.sessionkey = %u", cloginrec.SessionKey);

	CPacket1 cpacketChartRequest;
	cpacketChartRequest = loginreply;
	cpacketChartRequest.ProtocolType = PROTOCOL_TYPE_CHAT_SUBMIT;
	cpacketChartRequest.MainProtocolType = 4;
	cpacketChartRequest.SubProtocolType = 1;
	cpacketChartRequest.PacketType = PT_CSRequest;
	cpacketChartRequest.Key = cloginrec.SessionKey;
	cpacketChartRequest.UserId = 1999999999;

	CChatRequest chatrequest;
	chatrequest.Message = message;
	chatrequest.ToId = 10000005;
	chatrequest.ToType = 1;
	buffer.Clear();
	chatrequest.Encode(&cpacketChartRequest.ProtocolBody);
	cpacketChartRequest.Encode(&buffer);
	if (!(socket.Send(buffer.GetConstBuffer(), buffer.GetSize(), 0)))
	{
		error_log("chart send socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}

	if (repeats > 1)
	{
		CStaticBuffer<100> UpdateSavebuffer;
		if (fork() == 0)
		{
			int tmpinterval = interval;
			if (fork() == 0)
			{
				int sendsize;
				cpacketChartRequest.ProtocolType = PROTOCOL_TYPE_REFRESH;
				cpacketChartRequest.ProtocolBody.Clear();
				cpacketChartRequest.ProtocolBody.Append((byte *)"\0",1);
				while (--repeats)
				{
					cpacketChartRequest.Encode(&UpdateSavebuffer);
					while (tmpinterval > UPDATE_SAVE_INTERVAL)
					{
						sleep(UPDATE_SAVE_INTERVAL);
						tmpinterval -= UPDATE_SAVE_INTERVAL;
						sendsize = (socket.Send(UpdateSavebuffer.GetConstBuffer(),UpdateSavebuffer.GetSize(), 0));
						if (sendsize < 0)
						{
							error_log("updatesave  error sendsize = %d",sendsize);
							exit(-1);
						}
					}
					sleep(tmpinterval);
					sendsize = socket.Send(buffer.GetConstBuffer(),buffer.GetSize(), 0);
					if(sendsize < 0 )
					{
						error_log("chart send socket error,sendsize = %d",sendsize);
						exit(-1);
					}
					info_log("auto to send success sendsize = %d",sendsize);
					tmpinterval = interval;
				}
				socket.Close();
				exit(0);
			}
			socket.Close();
			exit(0);
		}
		sleep(2);
	}

	buffer.Clear();
	recvsize = socket.Receive(buffer.GetNativeBuffer(), buffer.GetFreeCapacity());
	if (recvsize < 0) {
		error_log("receive socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}
	buffer.SetSize(recvsize);

	CPacket1 cpacketChartReply;
	if (!cpacketChartReply.Decode(&buffer))
	{
		error_log("cpacket decode error");
		socket.Close();
		return R_ERR_DATA;
	}
	if (cpacketChartReply.ProtocolResult != 0) {
		error_log("protocol error");
		return R_ERROR;
	}

	return R_SUCCESS;
}

int CLogicAdmin::AddForbidUser(unsigned uid, unsigned forbidts, string serverid, string splatform) {
	string tcpserver = Config::GetValue(CONFIG_TCP_HOST);

	int serveridint = (uid - UID_MIN) / 500000 + 1;
	serveridint = MainConfig::GetMergedDomain(serveridint);
	if(!serveridint)
		serveridint = (uid - UID_MIN) / 500000 + 1;
	else
		serverid = Convert::IntToString(serveridint);
	if(serverid.empty())
		serverid = Convert::IntToString(serveridint);

	CPacket1 cpacket;
	cpacket.UserId = 1999999999;
	cpacket.ProtocolType = PROTOCOL_TYPE_LOGIN;
	cpacket.MainProtocolType = 1;
	cpacket.SubProtocolType = 1;
	cpacket.PacketType = PT_CSRequest;
	cpacket.ProtocolBody.Clear();

	CLoginSend cloginsend;
	cloginsend.name = "{\"name\":\"系统管理员\",\"hname\":\"\",\"hlvl\":0}";
	cloginsend.platform = (byte) splatform[0];
	error_log("platform = %u, name = %s, server=%s",cloginsend.platform, cloginsend.name.c_str(),serverid.c_str());

	if (!cloginsend.Encode(&cpacket.ProtocolBody)) {
		error_log(" body encode failed");
		return R_ERR_DATA;
	}

	CStaticBuffer<1000> buffer;
	buffer.Clear();
	if (!cpacket.Encode(&buffer)) {
		error_log(" packet encode failed");
		return R_ERR_DATA;
	}

	CSocket socket;
	struct sockaddr_in server;
	if (!socket.Socket(SOCK_STREAM, 0, AF_INET)) {
		error_log("create socket error");
		return R_ERROR;
	}

	//string tcpserver = Config::GetValue(CONFIG_TCP_SERVER);
	size_t pos = tcpserver.find(':');
	string hostname = tcpserver.substr(0, pos);
	string port = tcpserver.substr(pos + 1, tcpserver.length() - pos - 1);
	bool isIp = CheckIp(hostname);
	string host = isIp ? hostname : string(inet_ntoa(*(struct in_addr*) *(gethostbyname(hostname.c_str())->h_addr_list)));
	socket.Socket(SOCK_STREAM, 0, AF_INET);
	server.sin_addr.s_addr = inet_addr(host.c_str());
	server.sin_port = htons(atoi(port.c_str()));
	server.sin_family = AF_INET;
	memset(server.sin_zero, 0x00, sizeof(server.sin_zero));

	if (!socket.Connect((struct sockaddr *) &server, sizeof(struct sockaddr))) {
		error_log("connect socket error,addr = %s, port=%s,errno = %d, errmsg = %s",
				host.c_str(),port.c_str(),errno, strerror(errno));
		socket.Close();
		return R_ERR_REFUSE;
	}

	struct timeval timeout;
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	if (!socket.SetSockOpt(SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout,
			sizeof(struct timeval))
			|| !socket.SetSockOpt(SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout,
					sizeof(struct timeval)))
		error_log("set time out wrong");

	//if (isIp && serverid != "") {
	string firstpack = "tgw_l7_forward\r\nHost: " + tcpserver + " serverid:"
			+ serverid + "\r\n\r\n";
	int firstpackLen = firstpack.length();
	byte packbyte[firstpackLen];
	memset(packbyte, 0x00, firstpackLen);
	memcpy(packbyte, firstpack.c_str(), firstpackLen);
	if (!(socket.Send(packbyte, firstpackLen, 0))) {
		error_log("send socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}
	//}
	if (!(socket.Send(buffer.GetConstBuffer(), buffer.GetSize(), 0))) {
		error_log("send socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}

	unsigned int recvsize = 0;
	buffer.Clear();
	recvsize = socket.Receive(buffer.GetNativeBuffer(),
			buffer.GetFreeCapacity());
	if (recvsize < 0) {
		error_log("receive socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}

	buffer.SetSize(recvsize);
	CPacket1 loginreply;
	if (!loginreply.Decode(&buffer)) {
		error_log("cpacket decode error");
		socket.Close();
		return R_ERR_DATA;
	}

	CLoginReceive cloginrec;
	if (!cloginrec.Decode(&(loginreply.ProtocolBody))) {
		error_log("decode error");
		socket.Close();
		return R_ERR_DATA;
	}

	error_log("clogicrec.sessionkey = %u", cloginrec.SessionKey);

	CPacket1 cpacketForbidChartRequest;
	cpacketForbidChartRequest = loginreply;
	cpacketForbidChartRequest.ProtocolType = PROTOCOL_TYPE_FORBID_CHAT;
	cpacketForbidChartRequest.MainProtocolType = 5;
	cpacketForbidChartRequest.SubProtocolType = 1;
	cpacketForbidChartRequest.PacketType = PT_CSRequest;
	cpacketForbidChartRequest.Key = cloginrec.SessionKey;
	cpacketForbidChartRequest.UserId = 1999999999;

	CChatRequest chatrequest;
	CForbidChatRequest forbidrequest;
	//	forbidrequest.Message = "222232121";
	forbidrequest.ForbidUid = uid;
	forbidrequest.ForbidTs = forbidts;
	//	chatrequest.Message = "whut_psl";
	//	chatrequest.ToId = uid;
	//	chatrequest.ToType = 1;
	buffer.Clear();
	cpacketForbidChartRequest.ProtocolBody.Clear();
	forbidrequest.Encode(&cpacketForbidChartRequest.ProtocolBody);
	error_log("message = %s",forbidrequest.ToString().c_str());
	cpacketForbidChartRequest.Encode(&buffer);
	if (!(socket.Send(buffer.GetConstBuffer(), buffer.GetSize(), 0))) {
		error_log("chart send socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}
	//
	//	buffer.Clear();
	//	recvsize = 0;
	recvsize = socket.Receive(buffer.GetNativeBuffer(),
			buffer.GetFreeCapacity());
	if (recvsize <= 0) {
		error_log("receive socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}

	buffer.SetSize(recvsize);

	CPacket1 cpacketChartReply;
	if (!cpacketChartReply.Decode(&buffer)) {
		error_log("cpacket decode error");
		socket.Close();
		return R_ERR_DATA;
	}
	if (cpacketChartReply.ProtocolResult != 0) {
		error_log("protocol error");
		return R_ERROR;
	}

	return 0;
}

int CLogicAdmin::Query_th(Json::Value &buffer)
{
	CDataGM *pData = GetDataGM();
	if (NULL == pData)
	{
		error_log("GetDataGM faile");
		return R_ERR_DATA;
	}

	int ret = pData->Query(buffer, gm_th);

	return 0;
}

int CLogicAdmin::Add_th(string &openid)
{
	CDataGM *pData = GetDataGM();
	if (NULL == pData)
	{
		error_log("GetDataGM faile");
		return R_ERR_DATA;
	}

	int ret = pData->Add(openid, gm_th);

	return 0;
}

int CLogicAdmin::Del_th(string &openid)
{
	CDataGM *pData = GetDataGM();
	if (NULL == pData)
	{
		error_log("GetDataGM faile");
		return R_ERR_DATA;
	}

	int ret = pData->Del(openid, gm_th);

	return 0;

}

int CLogicAdmin::QueryGM(Json::Value &buffer)
{
	CDataGM *pData = GetDataGM();
	if (NULL == pData)
	{
		error_log("GetDataGM faile");
		return R_ERR_DATA;
	}

	int ret = pData->Query(buffer, gm_admin);

	return 0;
}
int CLogicAdmin::DelGm(string &openid)
{
	CDataGM *pData = GetDataGM();
	if (NULL == pData)
	{
		error_log("GetDataGM faile");
		return R_ERR_DATA;
	}

	int ret = pData->Del(openid, gm_admin);

	return 0;
}

int CLogicAdmin::AddGm(string &openid)
{
	CDataGM *pData = GetDataGM();
	if (NULL == pData)
	{
		error_log("GetDataGM faile");
		return R_ERR_DATA;
	}

	int ret = pData->Add(openid, gm_admin);

	return 0;
}

int CLogicAdmin::CheckGM(string &openid, unsigned &flag)
{
	CDataGM *pData = GetDataGM();
	if (NULL == pData)
	{
		error_log("GetDataGM faile");
		return R_ERR_DATA;
	}

	int ret = pData->Check(openid, flag);

	return 0;
}

int CLogicAdmin::ChangeActivityTime(const string &name, unsigned value)
{
	string configFilePath;
	int serverid = 0;
	Config::GetDomain(serverid);

	if (!Config::GetAppConfigFilePath(serverid, configFilePath) || configFilePath.empty())
	{
		LOGIC_ERROR_RETURN_MSG("serverpath");
	}

	CMarkupSTL xmlConf;
	if(!xmlConf.Load(configFilePath.c_str()))
	{
		LOGIC_ERROR_RETURN_MSG("config_file");
	}
	if(!xmlConf.FindElem("configs") )
	{
		LOGIC_ERROR_RETURN_MSG("FindElem_fail");
	}
	if (!xmlConf.IntoElem())
	{
		LOGIC_ERROR_RETURN_MSG("IntoElem");
	}

	while(xmlConf.FindElem("config"))
	{
		string tempname = xmlConf.GetAttrib("name");
		if (tempname == name)
		{
			if (name == CONFIG_POINTS_BEGIN_TS || name == CONFIG_POINTS_END_TS)
			{
				string tempValue = xmlConf.GetAttrib("name");
				if (value != Convert::StringToUInt(tempValue))
				{
					CLogicPointsRank logicPointsRank;
					logicPointsRank.CleanRankList();
				}
			}

			if(!xmlConf.SetAttrib("value", value))
			{
				error_log("set config %s fail",name);
				DATA_ERROR_RETURN_MSG(name);
			}

			if (!xmlConf.Save(configFilePath.c_str()))
			{
				REFUSE_RETURN_MSG("save_file_fail");
			}
			break;
		}
	}

	return 0;
}

int CLogicAdmin::ChangePlatform(const string &name, const string &value)
{
	string configFilePath;
	int serverid = 0;
	Config::GetDomain(serverid);

	if (!Config::GetAppConfigFilePath(serverid, configFilePath) || configFilePath.empty())
	{
		LOGIC_ERROR_RETURN_MSG("serverpath");
	}

	CMarkupSTL xmlConf;
	if(!xmlConf.Load(configFilePath.c_str()))
	{
		LOGIC_ERROR_RETURN_MSG("config_file");
	}
	if(!xmlConf.FindElem("configs") )
	{
		LOGIC_ERROR_RETURN_MSG("FindElem_fail");
	}
	if (!xmlConf.IntoElem())
	{
		LOGIC_ERROR_RETURN_MSG("IntoElem");
	}

	while(xmlConf.FindElem("config"))
	{
		string tempname = xmlConf.GetAttrib("name");
		if (tempname == name)
		{

			if(!xmlConf.SetAttrib("value", value.c_str()))
			{
				error_log("set config %s fail",name);
				DATA_ERROR_RETURN_MSG(name);
			}

			if (!xmlConf.Save(configFilePath.c_str()))
			{
				REFUSE_RETURN_MSG("save_file_fail");
			}
			break;
		}
	}

	return 0;
}

int CLogicAdmin::GetActivityTime(const string &name, unsigned &value)
{
	string configFilePath;
	int serverid = 0;
	Config::GetDomain(serverid);

	if (!Config::GetAppConfigFilePath(serverid, configFilePath) || configFilePath.empty())
	{
		LOGIC_ERROR_RETURN_MSG("serverpath");
	}


	CMarkupSTL xmlConf;
	if(!xmlConf.Load(configFilePath.c_str()))
	{
		LOGIC_ERROR_RETURN_MSG("config_file");
	}
	if(!xmlConf.FindElem("configs") )
	{
		LOGIC_ERROR_RETURN_MSG("configs");
	}
	if (!xmlConf.IntoElem())
	{
		LOGIC_ERROR_RETURN_MSG("IntoElem");
	}
	map<string, string> config_map;
	while(xmlConf.FindElem("config"))
	{
		string config_name = xmlConf.GetAttrib("name");
		string config_value = xmlConf.GetAttrib("value");
		config_map[config_name] = config_value;
	}

	map<string, string>::iterator itr = config_map.find(name);
	if (itr == config_map.end())
	{
		DATA_ERROR_RETURN_MSG(name);
	}
	value = Convert::StringToUInt(itr->second);
	return 0;
}

int CLogicAdmin::GetPlatform(const string &name, string &value)
{
	string configFilePath;
	int serverid = 0;
	Config::GetDomain(serverid);

	if (!Config::GetAppConfigFilePath(serverid, configFilePath) || configFilePath.empty())
	{
		LOGIC_ERROR_RETURN_MSG("serverpath");
	}


	CMarkupSTL xmlConf;
	if(!xmlConf.Load(configFilePath.c_str()))
	{
		LOGIC_ERROR_RETURN_MSG("config_file");
	}
	if(!xmlConf.FindElem("configs") )
	{
		LOGIC_ERROR_RETURN_MSG("configs");
	}
	if (!xmlConf.IntoElem())
	{
		LOGIC_ERROR_RETURN_MSG("IntoElem");
	}
	map<string, string> config_map;
	while(xmlConf.FindElem("config"))
	{
		string config_name = xmlConf.GetAttrib("name");
		string config_value = xmlConf.GetAttrib("value");
		config_map[config_name] = config_value;
	}

	map<string, string>::iterator itr = config_map.find(name);
	if (itr == config_map.end())
	{
		DATA_ERROR_RETURN_MSG(name);
	}
	value = itr->second;
	return 0;
}

int CLogicAdmin::GetInviteList(unsigned uid,Json::Value &data)
{
	int ret = 0;
	CDataInviteMapping mappingdata;
	DataUser userInvited;
	CLogicUser logicuser;
	vector<unsigned> vUid;
	int iCount = 0;
	int level45Count = 0;
	int level25Count = 0;
	int level15Count = 0;
	ret = mappingdata.GetUid(uid, vUid);
	if (0 != ret)
	{
		error_log("[GetUid fail][ret=%d,uid=%u]",ret,uid);
		return ret;
	}
	Json::Value invitList = Json::Value(Json::arrayValue);
	int i = 0;
	iCount = vUid.size();
	for(i = iCount - 1;i >=0;i--)
	{
		ret = logicuser.GetUserLimit(vUid[i],userInvited);
		if (0 != ret)
		{
			continue;
		}
		Json::Value &tempValue = invitList[invitList.size()];
		tempValue["uid"] = vUid[i];
		tempValue["level"] = userInvited.level;
	}
	data = invitList;
	return 0;
}
bool CLogicAdmin::CheckIp(const string &host){
	bool isIp = true;
	for(int i = 0 ; i < host.length();i++)
	{
		isIp = isIp&&((host[i]>='0'&&host[i]<='9')||host[i]=='.');
	}
	return isIp;
}

int CLogicAdmin::AddAccCharge(unsigned uid, int accCharge){
	CDataUser dbUser;
	int ret = dbUser.AddAccCharge(uid, accCharge);
	if(ret)
		error_log("[AddAccCharge fail][ret=%d,uid=%u]",ret,uid);
	return ret;
}

int CLogicAdmin::AddAttack(unsigned uid1, unsigned uid2, unsigned res)
{
	CLogicAttack logicAttack;
	DataAttack attackData;
	attackData.attack_id = 0;
	attackData.attack_uid = uid1;
	attackData.defence_uid = uid2;
	attackData.worldpos = 0;
	attackData.attack_platform = 2;
	attackData.attack_user_name = "uid1";
	attackData.attack_photo = "";
	attackData.attack_alliance_id = 0;
	attackData.attack_alliance_name = "";
	attackData.attack_alliance_flag = 0;
	attackData.defence_platform = 2;
	attackData.defence_user_name = "uid2";
	attackData.defence_photo = "";
	attackData.defence_alliance_id = 0;
	attackData.defence_alliance_name = "";
	attackData.defence_alliance_flag = 0;
	attackData.start_time = Time::GetGlobalTime();
	attackData.end_time = Time::GetGlobalTime() + 3;
	attackData.r1_loot = res;
	attackData.r2_loot = res;
	attackData.r3_loot = res;
	attackData.r4_loot = res;
	attackData.r5_loot = res;
	unsigned dbId = (unsigned)((attackData.attack_uid - 10000000) / 500000 + 1);
	int ret = logicAttack.AddAttack(attackData, dbId);
	return ret;
}

int CLogicAdmin::UpdatesAttack(uint64_t attack_id,unsigned uid1, unsigned uid2, unsigned res)
{
	CDataAttack attackDB;

	DataAttack old_attack;
	uint64_t attackId = attack_id;
    int ret = 0;

	ret = attackDB.GetAttackLimit(attackId, old_attack);
	if (ret != 0)
	{
		error_log("[GetAttackLimit fail][ret=%d,attack_uid=%d,attack_id=%ld]",
				ret,uid1,attackId);
		DB_ERROR_RETURN_MSG("get_attack_fail");
	}
	if (old_attack.attack_uid != uid1 || old_attack.defence_uid != uid2)
	{
		error_log("[attack info mismatch][uid=%u,attackid=%llu,apt=%d,r_apt=%d,duid=%u,r_duid=%u]",
			uid1, attackId, old_attack.attack_platform,
			uid2, old_attack.defence_uid);
		DATA_ERROR_RETURN_MSG("attack_info_error");
	}
	old_attack.end_time = Time::GetGlobalTime();
	old_attack.r1_loot = res;
	old_attack.r2_loot = res;
	old_attack.r3_loot = res;
	old_attack.r4_loot = res;
	old_attack.r5_loot = res;
	ret = attackDB.SetAttack(old_attack);
	if (ret != 0)
	{
		error_log("[SetAttack fail][ret=%d,uid=%d,attack_id=%ld]",
				ret,uid1,uid2);
		DB_ERROR_RETURN_MSG("set_attack_fail");
	}

	return ret;
}

int CLogicAdmin::UpdateBuildingLevel(unsigned uid, int buildingId, int buildingLev)
{
	int ret = 0;
	CLogicBuilding logicBuilding;
	Json::Value data;
	ret = logicBuilding.GetBuilding(uid,0,data,true);
	if (0 != ret)
	{
		return ret;
	}
	CLogicUser logicUser;
	DataUser user;
	ret = logicUser.GetUserLimit(uid, user);
	if (0 != ret)
	{
		return ret;
	}
	if (buildingLev > user.level)
	{
		LOGIC_ERROR_RETURN_MSG("blv_is_too_high");
	}
	bool save = false;
	for (int i=0; i<data.size(); i++)
	{
		if (data[i]["t"].asInt() == buildingId)
		{
			if (data[i].isMember("l"))
			{
				if (data[i]["l"].asInt() != buildingLev)
				{
					data[i]["l"] = buildingLev;
					if (data[i].isMember("cB"))
					{
						data[i].removeMember("cB");
					}
					save = true;
				}
			}
			else
			{
				data[i]["l"] = buildingLev;
				if (data[i].isMember("cB"))
				{
					data[i].removeMember("cB");
				}
				save = true;
			}
		}
	}

	if (save)
	{
		ret = logicBuilding.UpdateBuilding(uid, 0, data, true);
		if (0 != ret)
		{
			return ret;
		}
	}
	return 0;
}
CDataGM* CLogicAdmin::GetDataGM(void)
{
	static CDataGM* data;
	if (data)
	{
		return data;
	}
	data = new CDataGM();

	int ret = data->Init(CONFIG_GM_TH_SHM,sem_gm);
	if (0 != ret)
	{
		error_log("Init data fail!");
		delete data;
		return NULL;
	}
	//debug_log("Init data!");
	return data;
}
