#ifndef LOGICATTACK_H_
#define LOGICATTACK_H_

#include "LogicInc.h"

enum AttackType
{
	AT_ATTACK = 0,
	AT_DEFENCE = 1
};

enum AttackStatus
{
	AS_NO_READ = 0,
	AS_READ = 1
};

enum AttackFlag
{
	AF_INIT,	//无录像
	AF_VEDIO	//有录像
};

class CLogicAttack
{
public:
	int AddAttack(DataAttack &attack, unsigned db = 0);
	int UpdateAttack(const DataAttack &attack);
	int UpdateAttack(const DataAttack &attack,uint64_t &attackId);
	int GetAttack(uint64_t attackId, DataAttack &attack);
	int SetAttackRead(unsigned uid, uint64_t attack_id);
	int GetAttackHistory(unsigned uid, vector<DataAttack> &attacks);
	int GetAttackHistory(unsigned uid, unsigned worldpos, vector<DataAttack> &attacks,bool isMainPos);

	int GetUnreadAttacks(unsigned uid, AttackType type, vector<DataUserAttack> &attacks);
	int GetUnreadAttacks(unsigned uid, unsigned worldpos, AttackType type, vector<DataUserAttack> &attacks,bool isMainPos);
	int GetUserAttacksAfterOnType(unsigned uid, int type, unsigned time, vector<DataUserAttack> &datas);
	int GetUserAttacksAfterExceptWorldpos(unsigned uid, unsigned worldpos, int type, unsigned time, vector<DataUserAttack> &datas);

	//获取连续进攻保护时间
	int GetConBeAttackedCount(unsigned uid, int &count);
	int GetConBeAttackedCount(unsigned uid, unsigned worldpos,int &count,bool isMainPos);


	int SetVedio(uint64_t attackId, const string &vedio);
	int GetVedio(uint64_t attackId, string &vedio);
	int GetAllAttackHistory(unsigned uid, vector<DataAttack> &attacks);
};

#endif /* LOGICATTACK_H_ */
