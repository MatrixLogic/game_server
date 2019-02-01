#ifndef DATAUSER_H_
#define DATAUSER_H_

#include "Kernel.h"

struct DataUser
{
	unsigned uid;
	unsigned kingdom;
	int      register_platform;
	unsigned register_time;
	unsigned invite_uid;
	int      last_login_platform;
	unsigned last_login_time;
	unsigned last_active_time;
	unsigned login_times;
	unsigned login_days;
	int      invite_count;
	int      today_invite_count;
	int      status;
	unsigned sstate;
	int      type;
	int      level;
	uint64_t point;
	int      tutorial_stage;
	unsigned r1;
	unsigned r1_max;
	unsigned r2;
	unsigned r2_max;
	unsigned r3;
	unsigned r3_max;
	unsigned r4;
	unsigned r4_max;
	unsigned r5;
	unsigned r5_max;
	unsigned last_save_time;
	unsigned tribute_time;	//封号到到期时间
	unsigned protected_time;
	unsigned last_save_uid;	//最近攻击者的id //20131107 Ralf last_save_uid -> last_load_save_uid : fix for bugs, can save only last load archieve
	unsigned last_breath_time; //20131107 Ralf last_breath_time -> last_attacked_time : fix for online attacking bugs
	unsigned gcbase;
	unsigned mainpos;
	unsigned newgcbase;
	unsigned gcbuy;
	unsigned bit_info;
	unsigned alliance_id;
	int	     damage;
	unsigned prosper;
	unsigned accCharge;
	int      ext;			//累计在线时间
	int      viplevel;
	unsigned lastseq;
	unsigned battle_spirits;
	unsigned cdn;
	unsigned refresh;
	unsigned memory;
	string	 close_reason;	//封號原因
	string   user_stat;
	string   user_flag;
	string   user_tech;
	string   barrackQ;
	string   soldier;
	string   soldierlevel;
	string   wf_status;
	string   buildQ;
	string   skillQ;
	string   trainQ;
	string   glory;    // 存放玩家荣誉等信息
	string   worldres;
	string   vipstat;
	string   alliance_tech; //个人的联盟科技
	// add by aaron
	// string worldpos;
	// add by aaron

	DataUser& operator=(const DataUser &user)
	{
		uid = user.uid;
		kingdom = user.kingdom;
		register_platform = user.register_platform;
		register_time = user.register_time;
		invite_uid = user.invite_uid;
		last_login_platform = user.last_login_platform;
		last_login_time = user.last_login_time;
		last_active_time = user.last_active_time;
		login_times = user.login_times;
		login_days = user.login_days;
		invite_count = user.invite_count;
		today_invite_count = user.today_invite_count;
		status = user.status;
		sstate = user.sstate;
		type = user.type;
		level = user.level;
		point = user.point;
		tutorial_stage = user.tutorial_stage;
		r1 = user.r1;
		r1_max = user.r1_max;
		r2 = user.r2;
		r2_max = user.r2_max;
		r3 = user.r3;
		r3_max = user.r3_max;
		r4 = user.r4;
		r4_max = user.r4_max;
		r5 = user.r5;
		r5_max = user.r5_max;
		last_save_time = user.last_save_time;
		tribute_time = user.tribute_time;	//封号到到期时间
		close_reason = user.close_reason;
		protected_time = user.protected_time;
		last_save_uid = user.last_save_uid;	//最近攻击者的id
		last_breath_time = user.last_breath_time;
		gcbase = user.gcbase;
		mainpos = user.mainpos;
		newgcbase = user.newgcbase;
		gcbuy = user.gcbuy;
		bit_info = user.bit_info;
		alliance_id = user.alliance_id;
		damage = user.damage;
		prosper = user.prosper;
		accCharge = user.accCharge;
		ext = user.ext;			//累计在线时间
		lastseq = user.lastseq;
		battle_spirits = user.battle_spirits;
		user_stat = user.user_stat;
		user_flag = user.user_flag;
		user_tech = user.user_tech;
		barrackQ = user.barrackQ;
		soldier = user.soldier;
		soldierlevel = user.soldierlevel;
		wf_status = user.wf_status;
		buildQ = user.buildQ;
		skillQ = user.skillQ;
		trainQ = user.trainQ;
		glory = user.glory;
		worldres = user.worldres;
		viplevel = user.viplevel;

		vipstat = user.vipstat;
		cdn = user.cdn;
		refresh = user.refresh;
		memory = user.memory;
		alliance_tech = user.alliance_tech;
		//worldpos = user.worldpos
		return *this;
	}

	void copyLimit(const DataUser &user)
	{
		uid = user.uid;
		kingdom = user.kingdom;
		register_platform = user.register_platform;
		register_time = user.register_time;
		invite_uid = user.invite_uid;
		last_login_platform = user.last_login_platform;
		last_login_time = user.last_login_time;
		last_active_time = user.last_active_time;
		login_times = user.login_times;
		login_days = user.login_days;
		invite_count = user.invite_count;
		today_invite_count = user.today_invite_count;
		status = user.status;
		sstate = user.sstate;
		type = user.type;
		level = user.level;
		point = user.point;
		tutorial_stage = user.tutorial_stage;
		r1 = user.r1;
		r1_max = user.r1_max;
		r2 = user.r2;
		r2_max = user.r2_max;
		r3 = user.r3;
		r3_max = user.r3_max;
		r4 = user.r4;
		r4_max = user.r4_max;
		r5 = user.r5;
		r5_max = user.r5_max;
		last_save_time = user.last_save_time;
		tribute_time = user.tribute_time;
		protected_time = user.protected_time;
		last_save_uid = user.last_save_uid;
		last_breath_time = user.last_breath_time;
		gcbase = user.gcbase;
		newgcbase = user.newgcbase;
		mainpos = user.mainpos;
		gcbuy = user.gcbuy;
		bit_info = user.bit_info;
		alliance_id = user.alliance_id;
		damage = user.damage;
		prosper = user.prosper;
		accCharge = user.accCharge;
		ext = user.ext;
		lastseq = user.lastseq;
		battle_spirits = user.battle_spirits;
		glory = user.glory;
		cdn = user.cdn;
		refresh = user.refresh;
		memory = user.memory;
		alliance_tech = user.alliance_tech;
		//worldpos = user.worldpos
	}
};

class CDataUser : public CDataBaseDBC
{
public:
	CDataUser(int table = DB_USER) : CDataBaseDBC(table) {}

public:
	int AddUser(DataUser &user);
	int SetUser(unsigned uid, DataUser &user);
	int GetUser(unsigned uid, DataUser &user);
	int _check_user_stat(DataUser &user);
	int _un_check_user_stat(DataUser &user);
	int RemoveUser(unsigned uid);

	//no include: user_stat
	int SetUserLimit(unsigned uid, const DataUser &user);
	int GetUserLimit(unsigned uid, DataUser &user);

	//int SetActiveTime(unsigned uid, unsigned last_active_time);
	int SetTributeTime(unsigned uid, unsigned tribute_time);
	int SetProtectedTime(unsigned uid, unsigned protected_time);
	int SetLastSaveUid(unsigned uid, unsigned last_save_uid);
	int SetBitInfo(unsigned uid, unsigned bit_info);
	int SetBreathTime(unsigned uid, unsigned last_breath_time);
	int SetActiveExtBreathTime(unsigned uid, unsigned last_active_time, int ext, unsigned last_breath_time);
	int SetAllianceId(unsigned uid, unsigned alliance_id);
	int GetAllianceId(unsigned uid, unsigned &alliance_id);

	int AddProsper(unsigned uid, int prosper);
	int AddAccCharge(unsigned uid, int accCharge);

	int GetUserFlag(unsigned uid, string &user_flag);
	int SetUserFlag(unsigned uid, const string &user_flag);

	//设置最后活跃时间和在线登录时长
	int SetActiveTimeAndExt(unsigned uid, unsigned last_active_time, int ext);

	int AddBattleSpirits(unsigned uid, int battle_spirits);
	int GetMainPos(unsigned uid, unsigned &mainPos);
	int SetMainPos(unsigned uid, const unsigned mainpos);
};

#endif /* DATAUSER_H_ */
