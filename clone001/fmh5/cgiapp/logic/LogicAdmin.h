#ifndef __LOGIC_ADMIN_H__
#define __LOGIC_ADMIN_H__

#include "LogicInc.h"

enum ADMIN_LEVEL
{
	ADMIN_LEVEL_0 = 0,		//只能查看
	ADMIN_LEVEL_1 = 1,		//能修改部分数据
	ADMIN_LEVEL_9 = 9,		//能改金币
	ADMIN_LEVEL_ALL = 10	//最高权限，能改金币，能导档
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

enum GMEnum
{
	gm_none 	= 0,

	gm_admin 	= 1,
	gm_th 		= 2,
	gm_forbid 	= 3,
	gm_white 	= 4,
	gm_gift		= 5,
	gm_ip		= 6,
	gm_s_gift	= 7,

	gm_max,
};

class CLogicAdmin
{
public:
	int GetLevel(const string &name);
	int CheckLevel(const string &name, ADMIN_LEVEL needLevel);
	int Login(const string &name, const string &passwd, string &skey);
	int LoginCheck(string const &name,string const &skey);
	int AutoLogin(const string &name, string &skey);
	int ModifyPassword(const string &name, const string &password, const string &newpwd);
	int CheckSession(const string &name, const string &skey);
	bool CheckIp(string const &host);

	int BroadCastByTool(string message, int repeats = 0, int interval = 0, string serverid="");
	int BroadCast(string message, int repeats = 0, int interval = 0, string serverid="");
	int AddForbidUser(unsigned uid,unsigned forbidts, string serverid="", string splatform="2");
	int SetFlag(string &openid, unsigned flag);
	int CheckGM(string &openid, unsigned &flag);

	int ChangeBlockTs(const string &openid,unsigned block_time, const string &close_reason);
	int GetBlockTs(const string &openid, Json::Value &data);

	int AddResourceOrEquip(unsigned uid, unsigned type, unsigned count, string reason, unsigned eqid);

	static void Log(const string &admin, const string &op, const string &reason,
			unsigned uid, const string &value, const string &detail = "");
	static void AddCheckLog(const string &admin, const string &ip, const string &op, string &reason, unsigned uid, unsigned id, int count);

	string GetActTime();
	string SetActTime(unsigned id, unsigned bts, unsigned ets, unsigned ver);
private:
	static map<string, AdminInfo> adminList;
	static map<string, AdminSession> adminSession;
	static int UpdateAdminList();
};

#endif
