#include "DataUserAttack.h"

int CDataUserAttack::AddUserAttack(DataUserAttack &data)
{
	DBCREQ_DECLARE(DBC::InsertRequest, data.uid);
	DBCREQ_SET_KEY(data.uid);

	DBCREQ_SET_INT(data, attack_id);
	DBCREQ_SET_INT(data, opposite_uid);
	DBCREQ_SET_INT(data, worldpos);
	DBCREQ_SET_INT(data, opposite_platform);
	DBCREQ_SET_INT(data, type);
	DBCREQ_SET_INT(data, status);
	DBCREQ_SET_INT(data, start_time);
	DBCREQ_EXEC;
	return 0;
}

int CDataUserAttack::SetUserAttackStatus(unsigned uid, uint64_t attack_id, int status)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, attack_id, attack_id);
	DBCREQ_SET_INT_V(status);
	return 0;
}

int CDataUserAttack::SetUserAttackStatusAfter(unsigned uid, unsigned time, int status)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(GE, start_time, time);
	DBCREQ_SET_INT_V(status);
	return 0;
}
int CDataUserAttack::SetUserAttackStatusAfter(unsigned uid, unsigned worldpos,unsigned time, int status)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(GE, start_time, time);
	DBCREQ_SET_CONDITION(EQ, worldpos, worldpos);
	DBCREQ_SET_INT_V(status);
	return 0;
}

int CDataUserAttack::SetUserAttackStatusAfter(unsigned uid, int type, unsigned time, int status)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, type, type);
	DBCREQ_SET_CONDITION(GE, start_time, time);
	DBCREQ_SET_INT_V(status);
	return 0;
}
int CDataUserAttack::SetUserAttackStatusAfter(unsigned uid, unsigned worldpos,int type, unsigned time, int status)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, type, type);
	DBCREQ_SET_CONDITION(GE, start_time, time);
	DBCREQ_SET_CONDITION(EQ, worldpos, worldpos);
	DBCREQ_SET_INT_V(status);
	return 0;
}
int CDataUserAttack::GetUserAttacksAfter(unsigned uid, unsigned time, vector<DataUserAttack> &datas)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);

	DBCREQ_SET_KEY(uid);
	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(attack_id);
	DBCREQ_NEED(opposite_uid);
	DBCREQ_NEED(worldpos);

	DBCREQ_NEED(opposite_platform);
	DBCREQ_NEED(type);
	DBCREQ_NEED(status);
	DBCREQ_NEED(start_time);
	DBCREQ_SET_CONDITION(GE, start_time, time);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(datas);
	DBCREQ_ARRAY_GET_INT(datas, uid);
	DBCREQ_ARRAY_GET_INT(datas, attack_id);
	DBCREQ_ARRAY_GET_INT(datas, opposite_uid);
	DBCREQ_ARRAY_GET_INT(datas, worldpos);
	DBCREQ_ARRAY_GET_INT(datas, opposite_platform);
	DBCREQ_ARRAY_GET_INT(datas, type);
	DBCREQ_ARRAY_GET_INT(datas, status);
	DBCREQ_ARRAY_GET_INT(datas, start_time);
	DBCREQ_ARRAY_GET_END();
		return 0;
}
int CDataUserAttack::GetUserAttacksAfter(unsigned uid,unsigned worldpos, unsigned time, vector<DataUserAttack> &datas)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);

	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, worldpos, worldpos);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(attack_id);
	DBCREQ_NEED(opposite_uid);
	DBCREQ_NEED(worldpos);

	DBCREQ_NEED(opposite_platform);
	DBCREQ_NEED(type);
	DBCREQ_NEED(status);
	DBCREQ_NEED(start_time);
	DBCREQ_SET_CONDITION(GE, start_time, time);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(datas);
	DBCREQ_ARRAY_GET_INT(datas, uid);
	DBCREQ_ARRAY_GET_INT(datas, attack_id);
	DBCREQ_ARRAY_GET_INT(datas, opposite_uid);
	DBCREQ_ARRAY_GET_INT(datas, worldpos);
	DBCREQ_ARRAY_GET_INT(datas, opposite_platform);
	DBCREQ_ARRAY_GET_INT(datas, type);
	DBCREQ_ARRAY_GET_INT(datas, status);
	DBCREQ_ARRAY_GET_INT(datas, start_time);
	DBCREQ_ARRAY_GET_END();

	return 0;
}

int CDataUserAttack::GetUserAttacksAfterOnType(unsigned uid, int type, unsigned time, vector<DataUserAttack> &datas)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);

	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, type, type);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(attack_id);
	DBCREQ_NEED(opposite_uid);
	DBCREQ_NEED(worldpos);

	DBCREQ_NEED(opposite_platform);
	DBCREQ_NEED(type);
	DBCREQ_NEED(status);
	DBCREQ_NEED(start_time);
	DBCREQ_SET_CONDITION(GE, start_time, time);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(datas);
	DBCREQ_ARRAY_GET_INT(datas, uid);
	DBCREQ_ARRAY_GET_INT(datas, attack_id);
	DBCREQ_ARRAY_GET_INT(datas, opposite_uid);
	DBCREQ_ARRAY_GET_INT(datas, worldpos);
	DBCREQ_ARRAY_GET_INT(datas, opposite_platform);
	DBCREQ_ARRAY_GET_INT(datas, type);
	DBCREQ_ARRAY_GET_INT(datas, status);
	DBCREQ_ARRAY_GET_INT(datas, start_time);
	DBCREQ_ARRAY_GET_END();

	return 0;
}
int CDataUserAttack::GetUserAttacksAfterExceptWorldpos(unsigned uid,
		unsigned worldpos, int type, unsigned time,
		vector<DataUserAttack> &datas) {
	DBCREQ_DECLARE(DBC::GetRequest, uid);

	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, type, type);
	DBCREQ_SET_CONDITION(NE, worldpos, worldpos);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(attack_id);
	DBCREQ_NEED(opposite_uid);
	DBCREQ_NEED(worldpos);

	DBCREQ_NEED(opposite_platform);
	DBCREQ_NEED(type);
	DBCREQ_NEED(status);
	DBCREQ_NEED(start_time);
	DBCREQ_SET_CONDITION(GE, start_time, time);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(datas);
	DBCREQ_ARRAY_GET_INT(datas, uid);
	DBCREQ_ARRAY_GET_INT(datas, attack_id);
	DBCREQ_ARRAY_GET_INT(datas, opposite_uid);
	DBCREQ_ARRAY_GET_INT(datas, worldpos);
	DBCREQ_ARRAY_GET_INT(datas, opposite_platform);
	DBCREQ_ARRAY_GET_INT(datas, type);
	DBCREQ_ARRAY_GET_INT(datas, status);
	DBCREQ_ARRAY_GET_INT(datas, start_time);
	DBCREQ_ARRAY_GET_END();

	return 0;
}

int CDataUserAttack::GetUserAttacks(unsigned uid, int status, int type, unsigned time, vector<DataUserAttack> &datas)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);

	DBCREQ_SET_KEY(uid);
	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(attack_id);
	DBCREQ_NEED(opposite_uid);
	DBCREQ_NEED(worldpos);

	DBCREQ_NEED(opposite_platform);
	DBCREQ_NEED(type);
	DBCREQ_NEED(status);
	DBCREQ_NEED(start_time);
	DBCREQ_SET_CONDITION(EQ, status, status);
	DBCREQ_SET_CONDITION(EQ, type, type);
	DBCREQ_SET_CONDITION(GE, start_time, time);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(datas);
	DBCREQ_ARRAY_GET_INT(datas, uid);
	DBCREQ_ARRAY_GET_INT(datas, attack_id);
	DBCREQ_ARRAY_GET_INT(datas, opposite_uid);
	DBCREQ_ARRAY_GET_INT(datas, worldpos);
	DBCREQ_ARRAY_GET_INT(datas, opposite_platform);
	DBCREQ_ARRAY_GET_INT(datas, type);
	DBCREQ_ARRAY_GET_INT(datas, status);
	DBCREQ_ARRAY_GET_INT(datas, start_time);
	DBCREQ_ARRAY_GET_END();

	return 0;
}

int CDataUserAttack::GetUserAttacks(unsigned uid, unsigned worldpos, int status, int type, unsigned time, vector<DataUserAttack> &datas)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);

	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, worldpos, worldpos);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(attack_id);
	DBCREQ_NEED(opposite_uid);
	DBCREQ_NEED(worldpos);

	DBCREQ_NEED(opposite_platform);
	DBCREQ_NEED(type);
	DBCREQ_NEED(status);
	DBCREQ_NEED(start_time);
	DBCREQ_SET_CONDITION(EQ, status, status);
	DBCREQ_SET_CONDITION(EQ, type, type);
	DBCREQ_SET_CONDITION(GE, start_time, time);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(datas);
	DBCREQ_ARRAY_GET_INT(datas, uid);
	DBCREQ_ARRAY_GET_INT(datas, attack_id);
	DBCREQ_ARRAY_GET_INT(datas, opposite_uid);
	DBCREQ_ARRAY_GET_INT(datas, worldpos);
	DBCREQ_ARRAY_GET_INT(datas, opposite_platform);
	DBCREQ_ARRAY_GET_INT(datas, type);
	DBCREQ_ARRAY_GET_INT(datas, status);
	DBCREQ_ARRAY_GET_INT(datas, start_time);
	DBCREQ_ARRAY_GET_END();

	return 0;
}

int CDataUserAttack::GetUserAttacksCount(unsigned uid, unsigned time, int type, int &count)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(GE, start_time, time);
	DBCREQ_SET_CONDITION(EQ, type, type);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(attack_id);

	DBCREQ_EXEC;

	count = m_dbcret.TotalRows();

	return 0;
}

int CDataUserAttack::GetUserAttacksCount(unsigned uid,unsigned worldpos, unsigned time, int type, int &count)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(GE, start_time, time);
	DBCREQ_SET_CONDITION(EQ, type, type);
	DBCREQ_SET_CONDITION(EQ, worldpos, worldpos);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(attack_id);

	DBCREQ_EXEC;

	count = m_dbcret.TotalRows();

	return 0;
}
