#ifndef DATAATTACK_H_
#define DATAATTACK_H_

#include "Kernel.h"

struct DataAttack
{
	uint64_t attack_id;
	unsigned attack_uid;
	unsigned defence_uid;
	unsigned worldpos;
//	unsigned defence_baseid;
	int attack_platform;
	string attack_user_name;
	string attack_photo;
	unsigned attack_alliance_id;
	string attack_alliance_name;
	int attack_alliance_flag;
	int defence_platform;
	string defence_user_name;
	string defence_photo;
//	string defence_base_name;
	unsigned defence_alliance_id;
	string defence_alliance_name;
	int defence_alliance_flag;
	unsigned start_time;
	unsigned end_time;
	int r1_loot;
	int r2_loot;
	int r3_loot;
	int r4_loot;
	int r5_loot;
	int flag;
	string log;
};

class CDataAttack : public CDataBaseDBC
{
public:
	CDataAttack(int table = DB_ATTACK) : CDataBaseDBC(table) {}

public:
	int AddAttack(DataAttack &attack);
	int SetAttack(const DataAttack &attack);
	int GetAttack(uint64_t attack_id, DataAttack &attack);
	int GetAttackLimit(uint64_t attack_id, DataAttack &attack);	//不返回LOG
	int SetAttackFlag(uint64_t attack_id, int flag);
};

#endif /* DATAATTACK_H_ */
