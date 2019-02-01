#include "DataUser.h"

int CDataUser::AddUser(DataUser &user)
{
	DBCREQ_DECLARE(DBC::InsertRequest, user.uid);
	DBCREQ_SET_KEY(user.uid);

	DBCREQ_SET_INT(user, kingdom);
	DBCREQ_SET_INT(user, register_platform);
	DBCREQ_SET_INT(user, register_time);
	DBCREQ_SET_INT(user, invite_uid);
	DBCREQ_SET_INT(user, last_login_platform);
	DBCREQ_SET_INT(user, last_login_time);
	DBCREQ_SET_INT(user, last_active_time);
	DBCREQ_SET_INT(user, login_times);
	DBCREQ_SET_INT(user, login_days);
	DBCREQ_SET_INT(user, invite_count);
	DBCREQ_SET_INT(user, today_invite_count);
	DBCREQ_SET_INT(user, status);
	DBCREQ_SET_INT(user, sstate);
	DBCREQ_SET_INT(user, type);
	DBCREQ_SET_INT(user, level);
	DBCREQ_SET_INT(user, point);
	DBCREQ_SET_INT(user, tutorial_stage);
	DBCREQ_SET_INT(user, r1);
	DBCREQ_SET_INT(user, r1_max);
	DBCREQ_SET_INT(user, r2);
	DBCREQ_SET_INT(user, r2_max);
	DBCREQ_SET_INT(user, r3);
	DBCREQ_SET_INT(user, r3_max);
	DBCREQ_SET_INT(user, r4);
	DBCREQ_SET_INT(user, r4_max);
	DBCREQ_SET_INT(user, r5);
	DBCREQ_SET_INT(user, r5_max);
	DBCREQ_SET_INT(user, last_save_time);
	DBCREQ_SET_INT(user, tribute_time);
	DBCREQ_SET_INT(user, protected_time);
	DBCREQ_SET_INT(user, last_save_uid);
	DBCREQ_SET_INT(user, last_breath_time);
	DBCREQ_SET_INT(user, gcbase);
	DBCREQ_SET_INT(user, mainpos);
	DBCREQ_SET_INT(user, newgcbase);
	DBCREQ_SET_INT(user, gcbuy);
	DBCREQ_SET_INT(user, bit_info);
	DBCREQ_SET_INT(user, alliance_id);
	DBCREQ_SET_INT(user, damage);
	DBCREQ_SET_INT(user, prosper);
	DBCREQ_SET_INT(user, accCharge);
	DBCREQ_SET_INT(user, ext);
	DBCREQ_SET_INT(user, lastseq);
	DBCREQ_SET_INT(user, battle_spirits);
	DBCREQ_SET_INT(user, viplevel);
	DBCREQ_SET_INT(user, cdn);
	DBCREQ_SET_INT(user, refresh);
	DBCREQ_SET_INT(user, memory);
	DBCREQ_SET_STR(user, close_reason);
	DBCREQ_SET_STR(user, user_stat);
	DBCREQ_SET_STR(user, user_flag);
	DBCREQ_SET_STR(user, user_tech);
	DBCREQ_SET_STR(user, barrackQ);
	DBCREQ_SET_STR(user, soldier);
	DBCREQ_SET_STR(user, soldierlevel);
	DBCREQ_SET_STR(user, wf_status);
	DBCREQ_SET_STR(user, buildQ);
	DBCREQ_SET_STR(user, skillQ);
	DBCREQ_SET_STR(user, trainQ);
	DBCREQ_SET_STR(user, glory);
	DBCREQ_SET_STR(user, worldres);
	DBCREQ_SET_STR(user, vipstat);
	DBCREQ_SET_STR(user, alliance_tech);
	DBCREQ_EXEC;
	return 0;
}

int CDataUser::SetUser(unsigned uid, DataUser &user)
{
	_check_user_stat(user);

	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_INT(user, kingdom);
	DBCREQ_SET_INT(user, register_platform);
	DBCREQ_SET_INT(user, register_time);
	DBCREQ_SET_INT(user, invite_uid);
	DBCREQ_SET_INT(user, last_login_platform);
	DBCREQ_SET_INT(user, last_login_time);
	//DBCREQ_SET_INT(user, last_active_time);	//这个与在线时间ext在另一个方法时单独修改
	DBCREQ_SET_INT(user, login_times);
	DBCREQ_SET_INT(user, login_days);
	DBCREQ_SET_INT(user, invite_count);
	DBCREQ_SET_INT(user, today_invite_count);
	DBCREQ_SET_INT(user, status);
	DBCREQ_SET_INT(user, sstate);
	DBCREQ_SET_INT(user, type);
	DBCREQ_SET_INT(user, level);
	DBCREQ_SET_INT(user, point);
	DBCREQ_SET_INT(user, tutorial_stage);
	DBCREQ_SET_INT(user, r1);
	DBCREQ_SET_INT(user, r1_max);
	DBCREQ_SET_INT(user, r2);
	DBCREQ_SET_INT(user, r2_max);
	DBCREQ_SET_INT(user, r3);
	DBCREQ_SET_INT(user, r3_max);
	DBCREQ_SET_INT(user, r4);
	DBCREQ_SET_INT(user, r4_max);
	DBCREQ_SET_INT(user, r5);
	DBCREQ_SET_INT(user, r5_max);
	DBCREQ_SET_INT(user, last_save_time);
	DBCREQ_SET_INT(user, tribute_time);
	DBCREQ_SET_INT(user, protected_time);
	DBCREQ_SET_INT(user, last_save_uid);
	DBCREQ_SET_INT(user, last_breath_time);
	DBCREQ_SET_INT(user, gcbase);
	DBCREQ_SET_INT(user, mainpos);
	DBCREQ_SET_INT(user, newgcbase);
	DBCREQ_SET_INT(user, gcbuy);
	//DBCREQ_SET_INT(user, bit_info);		//修改这个变量用另一个单独的方法
	DBCREQ_SET_INT(user, alliance_id);
	DBCREQ_SET_INT(user, damage);
	DBCREQ_SET_INT(user, prosper);
	DBCREQ_SET_INT(user, accCharge);
	//DBCREQ_SET_INT(user, ext);			//另一个单独的方法修改它
	DBCREQ_SET_INT(user, lastseq);
	DBCREQ_SET_INT(user, battle_spirits);
	DBCREQ_SET_INT(user, viplevel);
	DBCREQ_SET_INT(user, cdn);
	DBCREQ_SET_INT(user, refresh);
	DBCREQ_SET_INT(user, memory);
	DBCREQ_SET_STR(user, close_reason);
	DBCREQ_SET_STR(user, user_stat);
	DBCREQ_SET_STR(user, user_flag);
	DBCREQ_SET_STR(user, user_tech);
	DBCREQ_SET_STR(user, barrackQ);
	DBCREQ_SET_STR(user, soldier);
	DBCREQ_SET_STR(user, soldierlevel);
	DBCREQ_SET_STR(user, wf_status);
	DBCREQ_SET_STR(user, buildQ);
	DBCREQ_SET_STR(user, skillQ);
	DBCREQ_SET_STR(user, trainQ);
	DBCREQ_SET_STR(user, glory);
	DBCREQ_SET_STR(user, worldres);
	DBCREQ_SET_STR(user, vipstat);
	DBCREQ_SET_STR(user, alliance_tech);

	DBCREQ_EXEC;
	return 0;
}

int CDataUser::GetUser(unsigned uid, DataUser &user)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(kingdom);
	DBCREQ_NEED(register_platform);
	DBCREQ_NEED(register_time);
	DBCREQ_NEED(invite_uid);
	DBCREQ_NEED(last_login_platform);
	DBCREQ_NEED(last_login_time);
	DBCREQ_NEED(last_active_time);
	DBCREQ_NEED(login_times);
	DBCREQ_NEED(login_days);
	DBCREQ_NEED(invite_count);
	DBCREQ_NEED(today_invite_count);
	DBCREQ_NEED(status);
	DBCREQ_NEED(sstate);
	DBCREQ_NEED(type);
	DBCREQ_NEED(level);
	DBCREQ_NEED(point);
	DBCREQ_NEED(tutorial_stage);
	DBCREQ_NEED(r1);
	DBCREQ_NEED(r1_max);
	DBCREQ_NEED(r2);
	DBCREQ_NEED(r2_max);
	DBCREQ_NEED(r3);
	DBCREQ_NEED(r3_max);
	DBCREQ_NEED(r4);
	DBCREQ_NEED(r4_max);
	DBCREQ_NEED(r5);
	DBCREQ_NEED(r5_max);
	DBCREQ_NEED(last_save_time);
	DBCREQ_NEED(tribute_time);
	DBCREQ_NEED(protected_time);
	DBCREQ_NEED(last_save_uid);
	DBCREQ_NEED(last_breath_time);
	DBCREQ_NEED(gcbase);
	DBCREQ_NEED(mainpos);
	DBCREQ_NEED(newgcbase);
	DBCREQ_NEED(gcbuy);
	DBCREQ_NEED(bit_info);
	DBCREQ_NEED(alliance_id);
	DBCREQ_NEED(damage);
	DBCREQ_NEED(prosper);
	DBCREQ_NEED(accCharge);
	DBCREQ_NEED(ext);
	DBCREQ_NEED(lastseq);
	DBCREQ_NEED(battle_spirits);
	DBCREQ_NEED(user_stat);
	DBCREQ_NEED(user_flag);
	DBCREQ_NEED(user_tech);
	DBCREQ_NEED(barrackQ);
	DBCREQ_NEED(soldier);
	DBCREQ_NEED(soldierlevel);
	DBCREQ_NEED(wf_status);
	DBCREQ_NEED(buildQ);
	DBCREQ_NEED(skillQ);
	DBCREQ_NEED(trainQ);
	DBCREQ_NEED(glory);
	DBCREQ_NEED(close_reason);
	DBCREQ_NEED(worldres);
	DBCREQ_NEED(viplevel);
	DBCREQ_NEED(vipstat);
	DBCREQ_NEED(cdn);
	DBCREQ_NEED(refresh);
	DBCREQ_NEED(memory);
	DBCREQ_NEED(alliance_tech);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(user, uid);
	DBCREQ_GET_INT(user, kingdom);
	DBCREQ_GET_INT(user, register_platform);
	DBCREQ_GET_INT(user, register_time);
	DBCREQ_GET_INT(user, invite_uid);
	DBCREQ_GET_INT(user, last_login_platform);
	DBCREQ_GET_INT(user, last_login_time);
	DBCREQ_GET_INT(user, last_active_time);
	DBCREQ_GET_INT(user, login_times);
	DBCREQ_GET_INT(user, login_days);
	DBCREQ_GET_INT(user, invite_count);
	DBCREQ_GET_INT(user, today_invite_count);
	DBCREQ_GET_INT(user, status);
	DBCREQ_GET_INT(user, sstate);
	DBCREQ_GET_INT(user, type);
	DBCREQ_GET_INT(user, level);
	DBCREQ_GET_INT(user, point);
	DBCREQ_GET_INT(user, tutorial_stage);
	DBCREQ_GET_INT(user, r1);
	DBCREQ_GET_INT(user, r1_max);
	DBCREQ_GET_INT(user, r2);
	DBCREQ_GET_INT(user, r2_max);
	DBCREQ_GET_INT(user, r3);
	DBCREQ_GET_INT(user, r3_max);
	DBCREQ_GET_INT(user, r4);
	DBCREQ_GET_INT(user, r4_max);
	DBCREQ_GET_INT(user, r5);
	DBCREQ_GET_INT(user, r5_max);
	DBCREQ_GET_INT(user, last_save_time);
	DBCREQ_GET_INT(user, tribute_time);
	DBCREQ_GET_INT(user, protected_time);
	DBCREQ_GET_INT(user, last_save_uid);
	DBCREQ_GET_INT(user, last_breath_time);
	DBCREQ_GET_INT(user, gcbase);
	DBCREQ_GET_INT(user, mainpos);
	DBCREQ_GET_INT(user, newgcbase);
	DBCREQ_GET_INT(user, gcbuy);
	DBCREQ_GET_INT(user, bit_info);
	DBCREQ_GET_INT(user, alliance_id);
	DBCREQ_GET_INT(user, damage);
	DBCREQ_GET_INT(user, prosper);
	DBCREQ_GET_INT(user, accCharge);
	DBCREQ_GET_INT(user, ext);
	DBCREQ_GET_INT(user, lastseq);
	DBCREQ_GET_INT(user, battle_spirits);
	DBCREQ_GET_STR(user, user_stat);
	DBCREQ_GET_STR(user, user_flag);
	DBCREQ_GET_STR(user, user_tech);
	DBCREQ_GET_STR(user, barrackQ);
	DBCREQ_GET_STR(user, soldier);
	DBCREQ_GET_STR(user, soldierlevel);
	DBCREQ_GET_STR(user, wf_status);
	DBCREQ_GET_STR(user, buildQ);
	DBCREQ_GET_STR(user, skillQ);
	DBCREQ_GET_STR(user, trainQ);
	DBCREQ_GET_STR(user, glory);
	DBCREQ_GET_STR(user, close_reason);
	DBCREQ_GET_STR(user, worldres);
	DBCREQ_GET_INT(user, viplevel);
	DBCREQ_GET_STR(user, vipstat);
	DBCREQ_GET_INT(user, cdn);
	DBCREQ_GET_INT(user, refresh);
	DBCREQ_GET_INT(user, memory);
	DBCREQ_GET_STR(user, alliance_tech);

	int ret = _check_user_stat(user);
	if(ret == 0)
		return SetUser(uid,user);
	return 0;
}

int CDataUser::_check_user_stat(DataUser &user)
{
	if(!user.user_tech.empty())
		return 1;
	Json::Reader reader;
	Json::Value stat, tech;
	if(user.user_stat.empty())
		return 2;
	if(!reader.parse(user.user_stat, stat))
		return 3;
	bool flag = false;
	vector<string> techName;
	techName.push_back("godE");
	techName.push_back("_loginTs");
	techName.push_back("part");
	techName.push_back("soul");
	techName.push_back("sskill");
	techName.push_back("combeffect");
	for(vector<string>::iterator it=techName.begin();it!=techName.end();++it)
	{
		if(stat.isMember(*it))
		{
			tech[*it] = stat[*it];
			stat.removeMember(*it);
			flag = true;
		}
	}
	if(flag)
	{
		Json::FastWriter writer;
		user.user_stat = writer.write(stat);
		user.user_tech = writer.write(tech);
		return 0;
	}
	return 4;
}
int CDataUser::_un_check_user_stat(DataUser &user)
{
	Json::Reader reader;
	Json::Value stat, tech;
	if(user.user_tech.empty() || user.user_stat.empty())
		return 1;
	if(!reader.parse(user.user_stat, stat))
		return 2;
	if (!reader.parse(user.user_tech, tech))
		return 3;
	bool flag = false;
	vector<string> techName;
	techName.push_back("godE");
	techName.push_back("_loginTs");
	techName.push_back("part");
	techName.push_back("soul");
	techName.push_back("sskill");
	techName.push_back("combeffect");
	for(vector<string>::iterator it=techName.begin();it!=techName.end();++it)
	{
		if(tech.isMember(*it))
		{
			stat[*it] = tech[*it];
			tech.removeMember(*it);
			flag = true;
		}
	}
	if(flag)
	{
		Json::FastWriter writer;
		user.user_stat = writer.write(stat);
		user.user_tech = writer.write(tech);
		return 0;
	}
	return 4;
}

int CDataUser::RemoveUser(unsigned uid)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_EXEC;
	return 0;
}

int CDataUser::SetUserLimit(unsigned uid, const DataUser &user)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_INT(user, kingdom);
	DBCREQ_SET_INT(user, register_platform);
	DBCREQ_SET_INT(user, register_time);
	DBCREQ_SET_INT(user, invite_uid);
	DBCREQ_SET_INT(user, last_login_platform);
	DBCREQ_SET_INT(user, last_login_time);
	//DBCREQ_SET_INT(user, last_active_time);
	DBCREQ_SET_INT(user, login_times);
	DBCREQ_SET_INT(user, login_days);
	DBCREQ_SET_INT(user, invite_count);
	DBCREQ_SET_INT(user, today_invite_count);
	DBCREQ_SET_INT(user, status);
	DBCREQ_SET_INT(user, sstate);
	DBCREQ_SET_INT(user, type);
	DBCREQ_SET_INT(user, level);
	DBCREQ_SET_INT(user, point);
	DBCREQ_SET_INT(user, tutorial_stage);
	DBCREQ_SET_INT(user, r1);
	DBCREQ_SET_INT(user, r1_max);
	DBCREQ_SET_INT(user, r2);
	DBCREQ_SET_INT(user, r2_max);
	DBCREQ_SET_INT(user, r3);
	DBCREQ_SET_INT(user, r3_max);
	DBCREQ_SET_INT(user, r4);
	DBCREQ_SET_INT(user, r4_max);
	DBCREQ_SET_INT(user, r5);
	DBCREQ_SET_INT(user, r5_max);
	DBCREQ_SET_INT(user, last_save_time);
	DBCREQ_SET_INT(user, tribute_time);
	DBCREQ_SET_INT(user, protected_time);
	DBCREQ_SET_INT(user, last_save_uid);
	DBCREQ_SET_INT(user, last_breath_time);
	DBCREQ_SET_INT(user, gcbase);
	DBCREQ_SET_INT(user, mainpos);
	DBCREQ_SET_INT(user, newgcbase);
	DBCREQ_SET_INT(user, gcbuy);
	//DBCREQ_SET_INT(user, bit_info);
	DBCREQ_SET_INT(user, alliance_id);
	DBCREQ_SET_INT(user, damage);
	DBCREQ_SET_INT(user, prosper);
	DBCREQ_SET_INT(user, accCharge);
	//DBCREQ_SET_INT(user, ext);
	DBCREQ_SET_INT(user, lastseq);
	DBCREQ_SET_INT(user, battle_spirits);
	DBCREQ_SET_STR(user, close_reason);
	DBCREQ_SET_INT(user, viplevel);
	DBCREQ_SET_INT(user, cdn);
	DBCREQ_SET_INT(user, refresh);
	DBCREQ_SET_INT(user, memory);
	DBCREQ_EXEC;
	return 0;
}

int CDataUser::GetUserLimit(unsigned uid, DataUser &user)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(kingdom);
	DBCREQ_NEED(register_platform);
	DBCREQ_NEED(register_time);
	DBCREQ_NEED(invite_uid);
	DBCREQ_NEED(last_login_platform);
	DBCREQ_NEED(last_login_time);
	DBCREQ_NEED(last_active_time);
	DBCREQ_NEED(login_times);
	DBCREQ_NEED(login_days);
	DBCREQ_NEED(invite_count);
	DBCREQ_NEED(today_invite_count);
	DBCREQ_NEED(status);
	DBCREQ_NEED(sstate);
	DBCREQ_NEED(type);
	DBCREQ_NEED(level);
	DBCREQ_NEED(point);
	DBCREQ_NEED(tutorial_stage);
	DBCREQ_NEED(r1);
	DBCREQ_NEED(r1_max);
	DBCREQ_NEED(r2);
	DBCREQ_NEED(r2_max);
	DBCREQ_NEED(r3);
	DBCREQ_NEED(r3_max);
	DBCREQ_NEED(r4);
	DBCREQ_NEED(r4_max);
	DBCREQ_NEED(r5);
	DBCREQ_NEED(r5_max);
	DBCREQ_NEED(last_save_time);
	DBCREQ_NEED(tribute_time);
	DBCREQ_NEED(protected_time);
	DBCREQ_NEED(last_save_uid);
	DBCREQ_NEED(last_breath_time);
	DBCREQ_NEED(gcbase);
	DBCREQ_NEED(mainpos);
	DBCREQ_NEED(newgcbase);
	DBCREQ_NEED(gcbuy);
	DBCREQ_NEED(bit_info);
	DBCREQ_NEED(alliance_id);
	DBCREQ_NEED(damage);
	DBCREQ_NEED(prosper);
	DBCREQ_NEED(accCharge);
	DBCREQ_NEED(ext);
	DBCREQ_NEED(lastseq);
	DBCREQ_NEED(battle_spirits);
	DBCREQ_NEED(close_reason);
	DBCREQ_NEED(viplevel);
	DBCREQ_NEED(cdn);
	DBCREQ_NEED(refresh);
	DBCREQ_NEED(memory);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(user, uid);
	DBCREQ_GET_INT(user, kingdom);
	DBCREQ_GET_INT(user, register_platform);
	DBCREQ_GET_INT(user, register_time);
	DBCREQ_GET_INT(user, invite_uid);
	DBCREQ_GET_INT(user, last_login_platform);
	DBCREQ_GET_INT(user, last_login_time);
	DBCREQ_GET_INT(user, last_active_time);
	DBCREQ_GET_INT(user, login_times);
	DBCREQ_GET_INT(user, login_days);
	DBCREQ_GET_INT(user, invite_count);
	DBCREQ_GET_INT(user, today_invite_count);
	DBCREQ_GET_INT(user, status);
	DBCREQ_GET_INT(user, sstate);
	DBCREQ_GET_INT(user, type);
	DBCREQ_GET_INT(user, level);
	DBCREQ_GET_INT(user, point);
	DBCREQ_GET_INT(user, tutorial_stage);
	DBCREQ_GET_INT(user, r1);
	DBCREQ_GET_INT(user, r1_max);
	DBCREQ_GET_INT(user, r2);
	DBCREQ_GET_INT(user, r2_max);
	DBCREQ_GET_INT(user, r3);
	DBCREQ_GET_INT(user, r3_max);
	DBCREQ_GET_INT(user, r4);
	DBCREQ_GET_INT(user, r4_max);
	DBCREQ_GET_INT(user, r5);
	DBCREQ_GET_INT(user, r5_max);
	DBCREQ_GET_INT(user, last_save_time);
	DBCREQ_GET_INT(user, tribute_time);
	DBCREQ_GET_INT(user, protected_time);
	DBCREQ_GET_INT(user, last_save_uid);
	DBCREQ_GET_INT(user, last_breath_time);
	DBCREQ_GET_INT(user, gcbase);
	DBCREQ_GET_INT(user, mainpos);
	DBCREQ_GET_INT(user, newgcbase);
	DBCREQ_GET_INT(user, gcbuy);
	DBCREQ_GET_INT(user, bit_info);
	DBCREQ_GET_INT(user, alliance_id);
	DBCREQ_GET_INT(user, damage);
	DBCREQ_GET_INT(user, prosper);
	DBCREQ_GET_INT(user, accCharge);
	DBCREQ_GET_INT(user, ext);
	DBCREQ_GET_INT(user, lastseq);
	DBCREQ_GET_INT(user, battle_spirits);
	DBCREQ_GET_STR(user, close_reason);
	DBCREQ_GET_INT(user, viplevel);
	DBCREQ_GET_INT(user, cdn);
	DBCREQ_GET_INT(user, refresh);
	DBCREQ_GET_INT(user, memory);

	return 0;
}

/*int CDataUser::SetActiveTime(unsigned uid, unsigned last_active_time)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_INT_V(last_active_time);
	return 0;
}*/

int CDataUser::SetTributeTime(unsigned uid, unsigned tribute_time)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_INT_V(tribute_time);
	return 0;
}

int CDataUser::SetProtectedTime(unsigned uid, unsigned protected_time)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_INT_V(protected_time);
	return 0;
}

int CDataUser::SetLastSaveUid(unsigned uid, unsigned last_save_uid)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_INT_V(last_save_uid);
	return 0;
}

int CDataUser::SetBitInfo(unsigned uid, unsigned bit_info)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_INT_V(bit_info);
	return 0;
}

int CDataUser::SetBreathTime(unsigned uid, unsigned last_breath_time)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_INT_V(last_breath_time);
	return 0;
}

int CDataUser::SetActiveExtBreathTime(unsigned uid, unsigned last_active_time, int ext, unsigned last_breath_time)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_INT_S(last_active_time);
	DBCREQ_SET_INT_S(ext);
	//20131107 Ralf last_breath_time -> last_attacked_time : fix for online attacking bugs
	//DBCREQ_SET_INT_S(last_breath_time);
	DBCREQ_EXEC;
	return 0;
}

int CDataUser::SetAllianceId(unsigned uid, unsigned alliance_id)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_INT_V(alliance_id);
	return 0;
}

int CDataUser::GetAllianceId(unsigned uid, unsigned &alliance_id)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_GET_INT_V(alliance_id);
	return 0;
}

int CDataUser::AddProsper(unsigned uid, int prosper)
{
	if (prosper == 0) return 0;
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	if (prosper < 0)
	{
		DBCREQ_SET_CONDITION(GE, prosper, 0 - prosper);
	}
	DBCREQ_ADD_INT_V(prosper);
	return 0;
}

int CDataUser::AddAccCharge(unsigned uid, int accCharge)
{
	if (accCharge == 0) return 0;
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	if (accCharge < 0)
	{
		DBCREQ_SET_CONDITION(GE, accCharge, 0 - accCharge);
	}
	DBCREQ_ADD_INT_V(accCharge);
	return 0;
}

int CDataUser::GetUserFlag(unsigned uid, string &user_flag)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_GET_STR_V(user_flag);
	return 0;
}

int CDataUser::SetUserFlag(unsigned uid, const string &user_flag)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_STR_V(user_flag);
	return 0;
}

int CDataUser::SetActiveTimeAndExt(unsigned uid, unsigned last_active_time, int ext)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_INT_S(last_active_time);
	DBCREQ_SET_INT_S(ext);
	DBCREQ_EXEC;
	return 0;
}

int CDataUser::AddBattleSpirits(unsigned uid, int battle_spirits)
{
	if (battle_spirits == 0) return 0;
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	if (battle_spirits < 0)
	{
		DBCREQ_SET_CONDITION(GE, battle_spirits, 0 - battle_spirits);
	}
	DBCREQ_ADD_INT_V(battle_spirits);
	return 0;
}
int CDataUser::GetMainPos(unsigned uid, unsigned &mainpos)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_GET_INT_V(mainpos);
	return 0;
}

int CDataUser::SetMainPos(unsigned uid, const unsigned mainpos)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_INT_V(mainpos);
	return 0;
}
