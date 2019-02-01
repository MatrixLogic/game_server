#include "DataAttack.h"

int CDataAttack::AddAttack(DataAttack &attack)
{
	DBCREQ_DECLARE(DBC::InsertRequest, attack.attack_id);
	DBCREQ_SET_KEY(attack.attack_id);

	DBCREQ_SET_INT(attack, attack_uid);
	DBCREQ_SET_INT(attack, defence_uid);
	DBCREQ_SET_INT(attack, worldpos);
	DBCREQ_SET_INT(attack, attack_platform);
	DBCREQ_SET_STR(attack, attack_user_name);
	DBCREQ_SET_STR(attack, attack_photo);
	DBCREQ_SET_INT(attack, attack_alliance_id);
	DBCREQ_SET_STR(attack, attack_alliance_name);
	DBCREQ_SET_INT(attack, attack_alliance_flag);
	DBCREQ_SET_INT(attack, defence_platform);
	DBCREQ_SET_STR(attack, defence_user_name);
	DBCREQ_SET_STR(attack, defence_photo);
	DBCREQ_SET_INT(attack, defence_alliance_id);
	DBCREQ_SET_STR(attack, defence_alliance_name);
	DBCREQ_SET_INT(attack, defence_alliance_flag);
	DBCREQ_SET_INT(attack, start_time);
	DBCREQ_SET_INT(attack, end_time);
	DBCREQ_SET_INT(attack, r1_loot);
	DBCREQ_SET_INT(attack, r2_loot);
	DBCREQ_SET_INT(attack, r3_loot);
	DBCREQ_SET_INT(attack, r4_loot);
	DBCREQ_SET_INT(attack, r5_loot);
	DBCREQ_SET_INT(attack, flag);
	DBCREQ_SET_STR(attack, log);
	DBCREQ_EXEC;
	return 0;
}

int CDataAttack::SetAttack(const DataAttack &attack)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, attack.attack_id);
	DBCREQ_SET_KEY(attack.attack_id);

	DBCREQ_SET_INT(attack, attack_uid);
	DBCREQ_SET_INT(attack, defence_uid);
	DBCREQ_SET_INT(attack, worldpos);

	DBCREQ_SET_INT(attack, attack_platform);
	DBCREQ_SET_STR(attack, attack_user_name);
	DBCREQ_SET_STR(attack, attack_photo);
	DBCREQ_SET_INT(attack, attack_alliance_id);
	DBCREQ_SET_STR(attack, attack_alliance_name);
	DBCREQ_SET_INT(attack, attack_alliance_flag);
	DBCREQ_SET_INT(attack, defence_platform);
	DBCREQ_SET_STR(attack, defence_user_name);
	DBCREQ_SET_STR(attack, defence_photo);
	DBCREQ_SET_INT(attack, defence_alliance_id);
	DBCREQ_SET_STR(attack, defence_alliance_name);
	DBCREQ_SET_INT(attack, defence_alliance_flag);
	DBCREQ_SET_INT(attack, start_time);
	DBCREQ_SET_INT(attack, end_time);
	DBCREQ_SET_INT(attack, r1_loot);
	DBCREQ_SET_INT(attack, r2_loot);
	DBCREQ_SET_INT(attack, r3_loot);
	DBCREQ_SET_INT(attack, r4_loot);
	DBCREQ_SET_INT(attack, r5_loot);
	DBCREQ_SET_INT(attack, flag);
	DBCREQ_SET_STR(attack, log);
	DBCREQ_EXEC;
	return 0;
}

int CDataAttack::GetAttack(uint64_t attack_id, DataAttack &attack)
{
	DBCREQ_DECLARE(DBC::GetRequest, attack_id);
	DBCREQ_SET_KEY(attack_id);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(attack_id);
	DBCREQ_NEED(attack_uid);
	DBCREQ_NEED(worldpos);

	DBCREQ_NEED(defence_uid);
	DBCREQ_NEED(attack_platform);
	DBCREQ_NEED(attack_user_name);
	DBCREQ_NEED(attack_photo);
	DBCREQ_NEED(attack_alliance_id);
	DBCREQ_NEED(attack_alliance_name);
	DBCREQ_NEED(attack_alliance_flag);
	DBCREQ_NEED(defence_platform);
	DBCREQ_NEED(defence_user_name);
	DBCREQ_NEED(defence_photo);
	DBCREQ_NEED(defence_alliance_id);
	DBCREQ_NEED(defence_alliance_name);
	DBCREQ_NEED(defence_alliance_flag);
	DBCREQ_NEED(start_time);
	DBCREQ_NEED(end_time);
	DBCREQ_NEED(r1_loot);
	DBCREQ_NEED(r2_loot);
	DBCREQ_NEED(r3_loot);
	DBCREQ_NEED(r4_loot);
	DBCREQ_NEED(r5_loot);
	DBCREQ_NEED(flag);
	DBCREQ_NEED(log);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(attack, attack_id);
	DBCREQ_GET_INT(attack, attack_uid);
	DBCREQ_GET_INT(attack, defence_uid);
	DBCREQ_GET_INT(attack, worldpos);
	DBCREQ_GET_INT(attack, attack_platform);
	DBCREQ_GET_STR(attack, attack_user_name);
	DBCREQ_GET_STR(attack, attack_photo);
	DBCREQ_GET_INT(attack, attack_alliance_id);
	DBCREQ_GET_STR(attack, attack_alliance_name);
	DBCREQ_GET_INT(attack, attack_alliance_flag);
	DBCREQ_GET_INT(attack, defence_platform);
	DBCREQ_GET_STR(attack, defence_user_name);
	DBCREQ_GET_STR(attack, defence_photo);
	DBCREQ_GET_INT(attack, defence_alliance_id);
	DBCREQ_GET_STR(attack, defence_alliance_name);
	DBCREQ_GET_INT(attack, defence_alliance_flag);
	DBCREQ_GET_INT(attack, start_time);
	DBCREQ_GET_INT(attack, end_time);
	DBCREQ_GET_INT(attack, r1_loot);
	DBCREQ_GET_INT(attack, r2_loot);
	DBCREQ_GET_INT(attack, r3_loot);
	DBCREQ_GET_INT(attack, r4_loot);
	DBCREQ_GET_INT(attack, r5_loot);
	DBCREQ_GET_INT(attack, flag);
	DBCREQ_GET_STR(attack, log);

	return 0;
}

int CDataAttack::GetAttackLimit(uint64_t attack_id, DataAttack &attack)
{
	DBCREQ_DECLARE(DBC::GetRequest, attack_id);
	DBCREQ_SET_KEY(attack_id);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(attack_id);
	DBCREQ_NEED(attack_uid);
	DBCREQ_NEED(defence_uid);
	DBCREQ_NEED(worldpos);
	DBCREQ_NEED(attack_platform);
	DBCREQ_NEED(attack_user_name);
	DBCREQ_NEED(attack_photo);
	DBCREQ_NEED(attack_alliance_id);
	DBCREQ_NEED(attack_alliance_name);
	DBCREQ_NEED(attack_alliance_flag);
	DBCREQ_NEED(defence_platform);
	DBCREQ_NEED(defence_user_name);
	DBCREQ_NEED(defence_photo);
	DBCREQ_NEED(defence_alliance_id);
	DBCREQ_NEED(defence_alliance_name);
	DBCREQ_NEED(defence_alliance_flag);
	DBCREQ_NEED(start_time);
	DBCREQ_NEED(end_time);
	DBCREQ_NEED(r1_loot);
	DBCREQ_NEED(r2_loot);
	DBCREQ_NEED(r3_loot);
	DBCREQ_NEED(r4_loot);
	DBCREQ_NEED(r5_loot);
	DBCREQ_NEED(flag);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(attack, attack_id);
	DBCREQ_GET_INT(attack, attack_uid);
	DBCREQ_GET_INT(attack, defence_uid);
	DBCREQ_GET_INT(attack, worldpos);
	DBCREQ_GET_INT(attack, attack_platform);
	DBCREQ_GET_STR(attack, attack_user_name);
	DBCREQ_GET_STR(attack, attack_photo);
	DBCREQ_GET_INT(attack, attack_alliance_id);
	DBCREQ_GET_STR(attack, attack_alliance_name);
	DBCREQ_GET_INT(attack, attack_alliance_flag);
	DBCREQ_GET_INT(attack, defence_platform);
	DBCREQ_GET_STR(attack, defence_user_name);
	DBCREQ_GET_STR(attack, defence_photo);
	DBCREQ_GET_INT(attack, defence_alliance_id);
	DBCREQ_GET_STR(attack, defence_alliance_name);
	DBCREQ_GET_INT(attack, defence_alliance_flag);
	DBCREQ_GET_INT(attack, start_time);
	DBCREQ_GET_INT(attack, end_time);
	DBCREQ_GET_INT(attack, r1_loot);
	DBCREQ_GET_INT(attack, r2_loot);
	DBCREQ_GET_INT(attack, r3_loot);
	DBCREQ_GET_INT(attack, r4_loot);
	DBCREQ_GET_INT(attack, r5_loot);
	DBCREQ_GET_INT(attack, flag);

	return 0;
}

int CDataAttack::SetAttackFlag(uint64_t attack_id, int flag)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, attack_id);
	DBCREQ_SET_KEY(attack_id);

	DBCREQ_SET_INT_V(flag);
	return 0;
}
