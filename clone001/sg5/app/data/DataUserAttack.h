#ifndef DATA_USER_ATTACK_H_
#define DATA_USER_ATTACK_H_

#include "Kernel.h"

struct DataUserAttack
{
	unsigned uid;
	uint64_t attack_id;
	unsigned opposite_uid;
	unsigned worldpos;
	int opposite_platform;
	int type;
	int status;
	unsigned start_time;
};

class CDataUserAttack : public CDataBaseDBC
{
public:
	CDataUserAttack(int table = DB_USER_ATTACK) : CDataBaseDBC(table) {}

public:
	int AddUserAttack(DataUserAttack &data);
	int SetUserAttackStatus(unsigned uid, uint64_t attack_id, int status);
	int SetUserAttackStatusAfter(unsigned uid, unsigned time, int status);
	int SetUserAttackStatusAfter(unsigned uid, unsigned worldpos,unsigned time, int status);

	int SetUserAttackStatusAfter(unsigned uid, int type, unsigned time, int status);
	int SetUserAttackStatusAfter(unsigned uid, unsigned worldpos, int type, unsigned time, int status);

	int GetUserAttacksAfter(unsigned uid, unsigned time, vector<DataUserAttack> &datas);
	int GetUserAttacksAfter(unsigned uid, unsigned worldpos, unsigned time, vector<DataUserAttack> &datas);
	int GetUserAttacksAfterOnType(unsigned uid, int type, unsigned time, vector<DataUserAttack> &datas);
	int GetUserAttacksAfterExceptWorldpos(unsigned uid, unsigned worldpos, int type, unsigned time, vector<DataUserAttack> &datas);

	int GetUserAttacks(unsigned uid, int status, int type, unsigned time, vector<DataUserAttack> &datas);
	int GetUserAttacks(unsigned uid, unsigned worldpos,int status, int type, unsigned time, vector<DataUserAttack> &datas);

	int GetUserAttacksCount(unsigned uid, unsigned time, int type, int &count);
	int GetUserAttacksCount(unsigned uid, unsigned worldpos, unsigned time, int type, int &count);

};

#endif
