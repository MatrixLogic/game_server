#include "LogicAttack.h"

int CLogicAttack::AddAttack(DataAttack &attack, unsigned db)
{
	if (attack.attack_uid == attack.defence_uid)
	{
		REFUSE_RETURN_MSG("attack_self");
	}
	CLogicIdCtrl logicIdCtrl;
	int ret = logicIdCtrl.GetNextId(KEY_ATTACK_ID_CTRL, attack.attack_id, db);
	if(ret != 0)
	{
		return ret;
	}
	attack.start_time = Time::GetGlobalTime();
	attack.end_time = Time::GetGlobalTime();

	CDataAttack attackDB;
	attack.flag = AF_INIT;
	ret = attackDB.AddAttack(attack);
	if (ret != 0)
	{
		error_log("[AddAttack fail][ret=%d,attack_uid=%d,defence_uid=%d]",
				ret,attack.attack_uid,attack.defence_uid);
		DB_ERROR_RETURN_MSG("add_attack_fail");
	}

	CLogicUserInteract logicUserInteract;
	CDataUserAttack userAttackDB;
	DataUserAttack userAttack;
	userAttack.uid = attack.attack_uid;
	userAttack.attack_id = attack.attack_id;
	userAttack.worldpos = attack.worldpos;
	userAttack.opposite_uid = attack.defence_uid;
	userAttack.opposite_platform = attack.defence_platform;
	userAttack.type = AT_ATTACK;
	userAttack.status = AS_NO_READ;
	userAttack.start_time = attack.start_time;
	ret = userAttackDB.AddUserAttack(userAttack);
	if (ret != 0)
	{
		error_log("[AddUserAttack fail][ret=%d,attack_uid=%d,defence_uid=%d]",
				ret,attack.attack_uid,attack.defence_uid);
		DB_ERROR_RETURN_MSG("add_user_attack_fail");
	}

	userAttack.uid = attack.defence_uid;
	userAttack.attack_id = attack.attack_id;
	userAttack.opposite_uid = attack.attack_uid;
	userAttack.worldpos = attack.worldpos;
	userAttack.opposite_platform = attack.attack_platform;
	userAttack.type = AT_DEFENCE;
	userAttack.status = AS_NO_READ;
	userAttack.start_time = attack.start_time;
	ret = userAttackDB.AddUserAttack(userAttack);
	if (ret != 0)
	{
		error_log("[AddUserAttack fail][ret=%d,attack_uid=%d,defence_uid=%d]",
				ret,attack.attack_uid,attack.defence_uid);
		DB_ERROR_RETURN_MSG("add_user_attack_fail");
	}

	logicUserInteract.AddAttack(attack.attack_uid, attack.defence_uid);

	//记录联盟仇恨
	if(attack.attack_alliance_id != attack.defence_alliance_id)
	{
		if(IsAllianceId(attack.attack_alliance_id))
		{
			ret = logicUserInteract.AddAttack(attack.attack_alliance_id, attack.defence_uid);
		}
		if(IsAllianceId(attack.defence_alliance_id))
		{
			ret = logicUserInteract.AddAttack(attack.attack_uid, attack.defence_alliance_id);
		}
		if(IsAllianceId(attack.attack_alliance_id) && IsAllianceId(attack.defence_alliance_id))
		{
			ret = logicUserInteract.AddAttack(attack.attack_alliance_id, attack.defence_alliance_id);
		}
	}

	return 0;
}

int CLogicAttack::UpdateAttack(const DataAttack &attack)
{
	vector<DataUserAttack> userAttacks;
	CDataUserAttack dbUserAttack;
	int ret = dbUserAttack.GetUserAttacks(attack.attack_uid, AS_NO_READ, AT_ATTACK,
			Time::GetGlobalTime() - ATTACK_MAX_TIME, userAttacks);
	if(ret != 0)
	{
		error_log("[GetUserAttacksCondition fail][ret=%d,attack_uid=%d]", ret, attack.attack_uid);
		DB_ERROR_RETURN_MSG("get_user_attack_fail");
	}
	uint64_t attackId = 0;
	for(vector<DataUserAttack>::iterator itr = userAttacks.begin(); itr != userAttacks.end(); itr++)
	{
		if(itr->attack_id > attackId)
		{
			attackId = itr->attack_id;
		}
	}
	if (attackId == 0)
	{
		error_log("[get attackid fail][ret=%d,attack_uid=%d,defence_uid=%d]",
				ret,attack.attack_uid,attack.defence_uid);
		DB_ERROR_RETURN_MSG("not_find_attack");
	}

	CDataAttack attackDB;
	DataAttack old_attack;
	ret = attackDB.GetAttackLimit(attackId, old_attack);
	if (ret != 0)
	{
		error_log("[GetAttackLimit fail][ret=%d,attack_uid=%d,attack_id=%ld]",
				ret,attack.attack_uid,attack.attack_id);
		DB_ERROR_RETURN_MSG("get_attack_fail");
	}
	if (old_attack.attack_uid != attack.attack_uid || old_attack.attack_platform != attack.attack_platform
			 || old_attack.defence_uid != attack.defence_uid)
	{
		error_log("[attack info mismatch][uid=%u,attackid=%llu,apt=%d,r_apt=%d,duid=%u,r_duid=%u]",
			attack.attack_uid, attackId, attack.attack_platform, old_attack.attack_platform,
			attack.defence_uid, old_attack.defence_uid);
		DATA_ERROR_RETURN_MSG("attack_info_error");
	}
	old_attack.end_time = Time::GetGlobalTime();
	old_attack.r1_loot = attack.r1_loot;
	old_attack.r2_loot = attack.r2_loot;
	old_attack.r3_loot = attack.r3_loot;
	old_attack.r4_loot = attack.r4_loot;
	old_attack.r5_loot = attack.r5_loot;
	old_attack.log = attack.log;
	ret = attackDB.SetAttack(old_attack);
	if (ret != 0)
	{
		error_log("[SetAttack fail][ret=%d,uid=%d,attack_id=%ld]",
				ret,attack.attack_uid,attack.attack_id);
		DB_ERROR_RETURN_MSG("set_attack_fail");
	}

	return 0;
}

int CLogicAttack::UpdateAttack(const DataAttack &attack,uint64_t &attackId)
{
	vector<DataUserAttack> userAttacks;
	CDataUserAttack dbUserAttack;
	int ret = dbUserAttack.GetUserAttacks(attack.attack_uid, AS_NO_READ, AT_ATTACK,
			Time::GetGlobalTime() - ATTACK_MAX_TIME, userAttacks);
	if(ret != 0)
	{
		error_log("[GetUserAttacksCondition fail][ret=%d,attack_uid=%d]", ret, attack.attack_uid);
		DB_ERROR_RETURN_MSG("get_user_attack_fail");
	}
	//uint64_t attackId = 0;
	for(vector<DataUserAttack>::iterator itr = userAttacks.begin(); itr != userAttacks.end(); itr++)
	{
		if(itr->attack_id > attackId)
		{
			attackId = itr->attack_id;
		}
	}
	if (attackId == 0)
	{
		error_log("[get attackid fail][ret=%d,attack_uid=%d,defence_uid=%d]",
				ret,attack.attack_uid,attack.defence_uid);
		DB_ERROR_RETURN_MSG("not_find_attack");
	}

	CDataAttack attackDB;
	DataAttack old_attack;
	ret = attackDB.GetAttackLimit(attackId, old_attack);
	if (ret != 0)
	{
		error_log("[GetAttackLimit fail][ret=%d,attack_uid=%d,attack_id=%ld]",
				ret,attack.attack_uid,attack.attack_id);
		DB_ERROR_RETURN_MSG("get_attack_fail");
	}
	if (old_attack.attack_uid != attack.attack_uid || old_attack.attack_platform != attack.attack_platform
			 || old_attack.defence_uid != attack.defence_uid)
	{
		error_log("[attack info mismatch][uid=%u,attackid=%llu,apt=%d,r_apt=%d,duid=%u,r_duid=%u]",
			attack.attack_uid, attackId, attack.attack_platform, old_attack.attack_platform,
			attack.defence_uid, old_attack.defence_uid);
		DATA_ERROR_RETURN_MSG("attack_info_error");
	}
	old_attack.end_time = Time::GetGlobalTime();
	old_attack.r1_loot = attack.r1_loot;
	old_attack.r2_loot = attack.r2_loot;
	old_attack.r3_loot = attack.r3_loot;
	old_attack.r4_loot = attack.r4_loot;
	old_attack.r5_loot = attack.r5_loot;
	old_attack.log = attack.log;
	ret = attackDB.SetAttack(old_attack);
	if (ret != 0)
	{
		error_log("[SetAttack fail][ret=%d,uid=%d,attack_id=%ld]",
				ret,attack.attack_uid,attack.attack_id);
		DB_ERROR_RETURN_MSG("set_attack_fail");
	}
	return 0;
}
int CLogicAttack::GetAttack(uint64_t attackId, DataAttack &attack)
{
	CDataAttack dbAttack;
	int ret = dbAttack.GetAttack(attackId, attack);
	if (ret != 0)
	{
		error_log("[GetAttack fail][ret=%d,attack_id=%d]",ret,attackId);
		DB_ERROR_RETURN_MSG("get_attack_fail");
	}
	return 0;
}

int CLogicAttack::SetAttackRead(unsigned uid, uint64_t attack_id)
{
	CDataUserAttack dbUserAttack;
	int ret = dbUserAttack.SetUserAttackStatus(uid, attack_id, AS_READ);
	if (ret != 0)
	{
		error_log("[SetUserAttackStatus fail][ret=%d,uid=%d,attack_id=%llu]", ret, uid, attack_id);
		DB_ERROR_RETURN_MSG("set_user_attack_fail");
	}
	return 0;
}

int CLogicAttack::GetAttackHistory(unsigned uid, vector<DataAttack> &attacks)
{
	CDataUserAttack userAttackDB;
	vector<DataUserAttack> datas;
	int ret = userAttackDB.GetUserAttacksAfter(uid, Time::GetGlobalTime() - ATTACK_HISTORY_TIME, datas);
	if (ret != 0)
	{
		error_log("[GetUserAttackAfter fail][ret=%d,uid=%d]",ret,uid);
		DB_ERROR_RETURN_MSG("get_user_attack_fail");
	}
	while(datas.size() > MAX_ATTACK_HISTORY)
	{
		size_t minIndex = 0;
		unsigned minTime = datas[0].start_time;
		for(size_t i = 1; i < datas.size(); i++)
		{
			DataUserAttack &userAttack = datas[i];
			if(userAttack.start_time <= minTime)
			{
				if(userAttack.attack_id < datas[minIndex].attack_id)
				{
					minTime = userAttack.start_time;
					minIndex = i;
				}
			}
		}
		datas.erase(datas.begin() + minIndex);
	}
	CDataAttack attackDB;
	for (size_t i = 0; i < datas.size(); i++)
	{
		DataAttack attack;
		ret = attackDB.GetAttackLimit(datas[i].attack_id, attack);
		if (ret != 0)
		{
			error_log("[GetAttackLimit fail][ret=%d,uid=%u,attack_id=%llu]", ret, uid, datas[i].attack_id);
		}
		else
		{
			attacks.push_back(attack);
		}
	}

	userAttackDB.SetUserAttackStatusAfter(uid, Time::GetGlobalTime() - ATTACK_HISTORY_TIME, AS_READ);

	return 0;
}

int CLogicAttack::GetAttackHistory(unsigned uid, unsigned worldpos, vector<DataAttack> &attacks,bool isMainPos)
{
	CDataUserAttack userAttackDB;
	vector<DataUserAttack> datas;
	vector<DataUserAttack> olddatas;

	vector<DataUserAttack> mainattackdatas;
	vector<DataUserAttack> maindefencedatas;


	int ret = 0;
	if(isMainPos && worldpos != 0)
	{
		ret = userAttackDB.GetUserAttacksAfter(uid, 0,Time::GetGlobalTime() - ATTACK_HISTORY_TIME, olddatas);
		//ret =  userAttackDB.GetUserAttacksAfterOnType(uid,0,Time::GetGlobalTime() - ATTACK_HISTORY_TIME,mainattackdatas);

	}
	ret = userAttackDB.GetUserAttacksAfter(uid, worldpos,Time::GetGlobalTime() - ATTACK_HISTORY_TIME, datas);

	if (ret != 0)
	{
		error_log("[GetUserAttackAfter fail][ret=%d,uid=%d]",ret,uid);
		DB_ERROR_RETURN_MSG("get_user_attack_fail");
	}
	if(olddatas.size() > 0)
	{
		datas.insert(datas.end(),olddatas.begin(),olddatas.end());
	}
	while(datas.size() > MAX_ATTACK_HISTORY)
	{
		size_t minIndex = 0;
		unsigned minTime = datas[0].start_time;
		for(size_t i = 1; i < datas.size(); i++)
		{
			DataUserAttack &userAttack = datas[i];
			if(userAttack.start_time <= minTime)
			{
				if(userAttack.attack_id < datas[minIndex].attack_id)
				{
					minTime = userAttack.start_time;
					minIndex = i;
				}
			}
		}
		datas.erase(datas.begin() + minIndex);
	}
	CDataAttack attackDB;
	for (size_t i = 0; i < datas.size(); i++)
	{
		DataAttack attack;
		ret = attackDB.GetAttackLimit(datas[i].attack_id, attack);
		if (ret != 0)
		{
			error_log("[GetAttackLimit fail][ret=%d,uid=%u,attack_id=%llu]", ret, uid, datas[i].attack_id);
		}
		else
		{
			attacks.push_back(attack);
		}
	}
	if(isMainPos && worldpos != 0)
	{
		userAttackDB.SetUserAttackStatusAfter(uid,0,Time::GetGlobalTime() - ATTACK_HISTORY_TIME, AS_READ);

	}
	userAttackDB.SetUserAttackStatusAfter(uid,worldpos,Time::GetGlobalTime() - ATTACK_HISTORY_TIME, AS_READ);

	return 0;
}

int CLogicAttack::GetUnreadAttacks(unsigned uid, AttackType type, vector<DataUserAttack> &attacks)
{
	CDataUserAttack userAttackDB;
	int ret = userAttackDB.GetUserAttacks(uid, AS_NO_READ, type, Time::GetGlobalTime() - ATTACK_HISTORY_TIME, attacks);
	if (ret != 0)
	{
		error_log("[GetUserAttacks fail][ret=%d,uid=%d]",ret,uid);
		DB_ERROR_RETURN_MSG("get_user_attack_fail");
	}
	userAttackDB.SetUserAttackStatusAfter(uid, type, Time::GetGlobalTime() - ATTACK_HISTORY_TIME, AS_READ);
	return 0;
}

int CLogicAttack::GetUnreadAttacks(unsigned uid, unsigned worldpos, AttackType type, vector<DataUserAttack> &attacks,bool isMainPos)
{
	CDataUserAttack userAttackDB;
	vector<DataUserAttack> oldattacks;
	vector<DataUserAttack> newattacks;
	int ret = 0;
	if(isMainPos && worldpos != 0)
	{
		ret = userAttackDB.GetUserAttacks(uid, 0,AS_NO_READ, type, Time::GetGlobalTime() - ATTACK_HISTORY_TIME, oldattacks);
	}

	ret = userAttackDB.GetUserAttacks(uid, worldpos,AS_NO_READ, type, Time::GetGlobalTime() - ATTACK_HISTORY_TIME, attacks);
	if (ret != 0)
	{
		error_log("[GetUserAttacks fail][ret=%d,uid=%d]",ret,uid);
		DB_ERROR_RETURN_MSG("get_user_attack_fail");
	}
	if(oldattacks.size() > 0)
	{
		attacks.insert(attacks.end(),oldattacks.begin(),oldattacks.end());
	}

	if(isMainPos)
	{
		userAttackDB.SetUserAttackStatusAfter(uid, 0,type, Time::GetGlobalTime() - ATTACK_HISTORY_TIME, AS_READ);
	}
	userAttackDB.SetUserAttackStatusAfter(uid,worldpos, type, Time::GetGlobalTime() - ATTACK_HISTORY_TIME, AS_READ);
	return 0;
}

int CLogicAttack::GetConBeAttackedCount(unsigned uid, int &count)
{
	CDataUserAttack dbUserAttack;
	int ret = dbUserAttack.GetUserAttacksCount(uid, Time::GetGlobalTime() - 3600, AT_DEFENCE, count);
	if (ret != 0)
	{
		error_log("[GetUserAttacksCount fail][ret=%d,uid=%d]", ret, uid);
		DB_ERROR_RETURN_MSG("get_user_attack_fail");
	}
	return 0;
}

int CLogicAttack::SetVedio(uint64_t attackId, const string &vedio)
{
	string path = Config::GetValue(CONFIG_VEDIO_DIR);
	if (path.empty())
	{
		error_log("[vedio config err][attackId=%llu]", attackId);
		DB_ERROR_RETURN_MSG("set_vedio_fail");
	}
	if (path[path.length()-1] != '/') path += '/';
	int subdir = attackId / 10000;
	path += CTrans::ITOS(subdir);
	struct stat buf;
	if (stat(path.c_str(), &buf) != 0 || !S_ISDIR(buf.st_mode))
	{
		if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0)
		{
			error_log("[mkdir fail][attackId=%llu,path=%s]", attackId,path.c_str());
			DB_ERROR_RETURN_MSG("set_vedio_fail");
		}
	}
	path.append("/").append(Convert::UInt64ToString(attackId)).append(".dat");
	if (File::Write(path, vedio) != 0)
	{
		error_log("[Write fail][attackId=%llu,path=%s]", attackId,path.c_str());
		DB_ERROR_RETURN_MSG("set_vedio_fail");
	}
	CDataAttack attackDb;
	int ret = attackDb.SetAttackFlag(attackId, AF_VEDIO);
	if (ret != 0)
	{
		error_log("[SetAttackFlag fail][attackId=%ll]", attackId);
		DB_ERROR_RETURN_MSG("set_attack_fail");
	}
	return 0;
}

int CLogicAttack::GetVedio(uint64_t attackId, string &vedio)
{
	string path = Config::GetValue(CONFIG_VEDIO_DIR);
	if (path.empty())
	{
		error_log("[vedio config err][attackId=%llu]", attackId);
		DB_ERROR_RETURN_MSG("get_vedio_fail");
	}
	if (path[path.length()-1] != '/') path += '/';
	int subdir = attackId / 10000;
	path += CTrans::ITOS(subdir);
	path.append("/").append(Convert::UInt64ToString(attackId)).append(".dat");
	if (File::Read(path, vedio) != 0)
	{
		error_log("[Read fail][attackId=%ll]", attackId);
		DB_ERROR_RETURN_MSG("get_vedio_fail");
	}

	return 0;
}

int CLogicAttack::GetAllAttackHistory(unsigned uid, vector<DataAttack> &attacks)
{
	CDataUserAttack userAttackDB;
	vector<DataUserAttack> datas;
	int ret = userAttackDB.GetUserAttacksAfterOnType(uid, AT_ATTACK, 0, datas);
	if (ret != 0)
	{
		error_log("[GetUserAttackAfter fail][ret=%d,uid=%d]",ret,uid);
		DB_ERROR_RETURN_MSG("get_user_attack_fail");
	}

	CDataAttack attackDB;
	for (size_t i = 0; i < datas.size(); i++)
	{
		DataAttack attack;
		ret = attackDB.GetAttackLimit(datas[i].attack_id, attack);
		if (ret != 0)
		{
			error_log("[GetAttackLimit fail][ret=%d,uid=%u,attack_id=%llu]", ret, uid, datas[i].attack_id);
		}
		else
		{
			attacks.push_back(attack);
		}
	}

	return 0;
}
