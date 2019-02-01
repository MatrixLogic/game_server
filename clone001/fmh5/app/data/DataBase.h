

#ifndef DATABASE_H_
#define DATABASE_H_
#include "Kernel.h"


struct DataBase{
	uint32_t uid;
	uint32_t register_platform;//注册平台
	uint32_t register_time;//注册时间
	uint32_t last_login_platform;//(上次登录平台)废弃， 作为累计在线秒数
	uint32_t last_login_time;//上次登录时间
	uint32_t login_times;//连续登录次数
	uint32_t login_days;//累计登录天数
	uint32_t last_active_time;//上次活跃时间
	uint32_t last_off_time;//上次下线时间
	uint32_t forbid_ts;//封号到期时间
	char forbid_reason[BASE_FORBID_REASON_LEN];//封号原因
	uint32_t tutorial_stage;//新手教程步骤
	char name[BASE_NAME_LEN];//名字
	char fig[BASE_FIG_LEN];//头像url
	uint64_t exp;//经验
	uint32_t level;//等级
	uint32_t acccharge;//累计充值
	uint32_t viplevel;//vip等级
	uint32_t cash;//二级货币
	uint32_t coin;//代币
	uint32_t first_recharge;  	//首充领取标志。按位表示第一次，第二次首充状态
	uint32_t alliance_id;//加入的联盟
	uint32_t count; //收获次数
	char barrier[BARRIER_LENGTH];   //障碍物状态

	DataBase(){
		uid = 0;
		register_platform = 0;
		register_time = 0;
		last_login_platform = 0;
		last_login_time = 0;
		login_times = 0;
		login_days = 0;
		last_active_time = 0;
		last_off_time = 0;
		forbid_ts = 0;
		tutorial_stage = 0;
		exp = 0;
		level = 0;
		acccharge = 0;
		viplevel = 0;
		cash = 0;
		coin = 0;
		first_recharge = 0;
		alliance_id = 0;
		count = 0;

		memset(forbid_reason, 0, sizeof(forbid_reason));
		memset(name, 0, sizeof(name));
		memset(fig, 0, sizeof(fig));
		memset(barrier, 0, sizeof(barrier));
	}

	void SetMessage(User::Base* msg)
	{
		msg->set_uid(uid);
		msg->set_registerplatform(register_platform);
		msg->set_registertime(register_time);
		msg->set_lastloginplatform(last_login_platform);
		msg->set_lastlogintime(last_login_time);
		msg->set_logintimes(login_times);
		msg->set_logindays(login_days);
		msg->set_lastactivetime(last_active_time);
		msg->set_lastofftime(last_off_time);
		msg->set_forbidts(forbid_ts);
		msg->set_forbidreason(string(forbid_reason));
		msg->set_tutorialstage(tutorial_stage);
		msg->set_name(string(name));
		msg->set_fig(string(fig));
		msg->set_exp(exp);
		msg->set_level(level);
		msg->set_acccharge(acccharge);
		msg->set_viplevel(viplevel);
		msg->set_cash(cash);
		msg->set_coin(coin);
		msg->set_firstrecharge(first_recharge);
		msg->set_allianceid(alliance_id);
		msg->set_barrier(barrier, BARRIER_LENGTH);
	}

	bool IsOnline()
	{
		return last_off_time < last_login_time;
	}

	bool CanOff()
	{
		return IsOnline() && last_active_time + 12 * 3600 < Time::GetGlobalTime();
	}

	bool CanClear()
	{
		return !IsOnline() && last_off_time + 300 < Time::GetGlobalTime();
	}

	void AddExp(int exp_);
};

class CDataBase :public DBCBase<DataBase, DB_BASE>
{
public:
	virtual int Get(DataBase &data);
	virtual int Add(DataBase &data);
	virtual int Set(DataBase &data);
	virtual int Del(DataBase &data);
};

#endif /* DATABASE_H_ */
