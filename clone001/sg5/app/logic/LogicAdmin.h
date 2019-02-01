#ifndef __LOGIC_ADMIN_H__
#define __LOGIC_ADMIN_H__

#include "LogicInc.h"

enum ADMIN_LEVEL
{
	ADMIN_LEVEL_0 = 0,		//只能查看
	ADMIN_LEVEL_1 = 1,		//能修改部分数据
	ADMIN_LEVEL_ALL = 9		//最高权限，能改金币，能导档
};

struct AdminSession
{
	string   skey;
	unsigned activeTime;
};

struct AdminInfo
{
	string pwd;
	int level;
};

class CLogicAdmin
{
public:
	static CDataGM* GetDataGM(void);
	int GetLevel(const string &name);
	int CheckLevel(const string &name, ADMIN_LEVEL needLevel);
	int Login(const string &name, const string &passwd, string &skey);
	int LoginCheck(string const &name,string const &skey);
	int AutoLogin(const string &name, string &skey);
	int ModifyPassword(const string &name, const string &password, const string &newpwd);
	int CheckSession(const string &name, const string &skey);
	int ChangePay(unsigned uid, int cash, int coins, DataPay &pay);
	int QueryUserInfo(unsigned uid, PlatformType pt, DataUserBasic &userBasic, DataUser &user);
	int QueryUserBasicInfo(const string &openid, PlatformType pt, DataUserBasic &userBasic);
	int QueryUser(unsigned uid, DataUser &user);
	int ChangeGcbase(unsigned uid, int gcbase, unsigned &balance);
	int ChangeStars(unsigned uid, int stars, unsigned &balance);
	int ChangeSoul(unsigned uid, int soul, unsigned &balance);
	int ChangeqlE(unsigned uid, int qle, unsigned &balance);
	int ChangePoint1(unsigned uid, int experience, unsigned &balance);   //经验值
	int ChangeR1(unsigned uid, int r1, unsigned &balance);
	int ChangeR2(unsigned uid, int r2, unsigned &balance);
	int ChangeR3(unsigned uid, int r3, unsigned &balance);
	int ChangeR4(unsigned uid, int r4, unsigned &balance);
	int ChangeR5(unsigned uid, int r5, unsigned &balance);
	int ChangeTutorialStage(unsigned uid, int tutorial_stage);
	int ChangeWfStatus(unsigned uid, const string &wf_status);
	int ChangeProsper(unsigned uid, int prosper, unsigned &balance);
	int ChangeBlockTs(unsigned uid, unsigned block_time, const string &close_reason);
	int ChangeProtectTs(unsigned uid, unsigned pro_time);
	int ChangeBattleSpirits(unsigned uid, int battleSpirits, unsigned &balance);
	int ExportArchive(unsigned uid, Json::Value &result);
	int ImportArchive(unsigned uid, const Json::Value &data);

	int AddAttack(unsigned uid1,unsigned uid2, unsigned res);
	int UpdatesAttack(uint64_t attack_id,unsigned uid1, unsigned uid2, unsigned res);

	int BroadCast(string message, int repeats = 0, int interval = 0, string serverid="");
	int AddForbidUser(unsigned uid,unsigned forbidts, string serverid="", string splatform="2");

	int AddGm(string &openid);
	int DelGm(string &openid);
	int QueryGM(Json::Value &buffer);
	int CheckGM(string &openid, unsigned &flag);
	int Query_th(Json::Value &buffer);
	int Add_th(string &openid);
	int Del_th(string &openid);

	int ChangePlatform(const string &name, const string &value);
	int ChangeActivityTime(const string &name, unsigned value);
	int GetPlatform(const string &name, string &value);
	int GetActivityTime(const string &name, unsigned &value);
	int ChangeBlockTs(const string &openid,unsigned block_time, const string &close_reason);
	int GetBlockTs(const string &openid, Json::Value &data);
	int GetInviteList(unsigned uid,Json::Value &data);
	bool CheckIp(string const &host);
	int UpdateBuildingLevel(unsigned uid, int buildingId, int buildingLev);
	static void Log(const string &admin, const string &op, const string &reason,
			unsigned uid, const string &value, const string &detail = "");
	int AddAccCharge(unsigned uid, int accCharge);
private:
	static map<string, AdminInfo> adminList;
	static map<string, AdminSession> adminSession;
	static int UpdateAdminList();
};

#endif
