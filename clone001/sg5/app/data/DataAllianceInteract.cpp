/*
 * DataAllianceInteract.cpp
 *
 *  Created on: 2013-4-18
 *      Author: Administrator
 */

#include "DataAllianceInteract.h"

int CDataAllianceInteract::AddAllianceInteract(const DataUserInteract &interact)
{
	DBCREQ_DECLARE(DBC::InsertRequest, interact.uid);
	DBCREQ_SET_KEY(interact.uid);

	DBCREQ_SET_INT(interact, opposite_uid);
	DBCREQ_SET_INT(interact, help_from);
	DBCREQ_SET_INT(interact, help_to);
	DBCREQ_SET_INT(interact, attack_from);
	DBCREQ_SET_INT(interact, attack_to);
	DBCREQ_SET_INT(interact, retaliate_count);
	DBCREQ_SET_INT(interact, last_attack_time);
	DBCREQ_SET_INT(interact, truce_time);
	DBCREQ_EXEC;
	return 0;
}

int CDataAllianceInteract::SetAllianceInteract(const DataUserInteract &interact)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, interact.uid);
	DBCREQ_SET_KEY(interact.uid);
	DBCREQ_SET_CONDITION(EQ, opposite_uid, interact.opposite_uid);

	DBCREQ_SET_INT(interact, help_from);
	DBCREQ_SET_INT(interact, help_to);
	DBCREQ_SET_INT(interact, attack_from);
	DBCREQ_SET_INT(interact, attack_to);
	DBCREQ_SET_INT(interact, retaliate_count);
	DBCREQ_SET_INT(interact, last_attack_time);
	DBCREQ_SET_INT(interact, truce_time);
	DBCREQ_EXEC;
	return 0;
}

int CDataAllianceInteract::GetAllianceInteract(unsigned uid, unsigned opposite_uid, DataUserInteract &interact)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, opposite_uid, opposite_uid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(opposite_uid);
	DBCREQ_NEED(help_from);
	DBCREQ_NEED(help_to);
	DBCREQ_NEED(attack_from);
	DBCREQ_NEED(attack_to);
	DBCREQ_NEED(retaliate_count);
	DBCREQ_NEED(last_attack_time);
	DBCREQ_NEED(truce_time);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(interact, uid);
	DBCREQ_GET_INT(interact, opposite_uid);
	DBCREQ_GET_INT(interact, help_from);
	DBCREQ_GET_INT(interact, help_to);
	DBCREQ_GET_INT(interact, attack_from);
	DBCREQ_GET_INT(interact, attack_to);
	DBCREQ_GET_INT(interact, retaliate_count);
	DBCREQ_GET_INT(interact, last_attack_time);
	DBCREQ_GET_INT(interact, truce_time);

	return 0;
}

int CDataAllianceInteract::GetAllianceInteracts(unsigned uid, vector<DataUserInteract> &interacts)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(opposite_uid);
	DBCREQ_NEED(help_from);
	DBCREQ_NEED(help_to);
	DBCREQ_NEED(attack_from);
	DBCREQ_NEED(attack_to);
	DBCREQ_NEED(retaliate_count);
	DBCREQ_NEED(last_attack_time);
	DBCREQ_NEED(truce_time);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(interacts);
	DBCREQ_ARRAY_GET_INT(interacts, uid);
	DBCREQ_ARRAY_GET_INT(interacts, opposite_uid);
	DBCREQ_ARRAY_GET_INT(interacts, help_from);
	DBCREQ_ARRAY_GET_INT(interacts, help_to);
	DBCREQ_ARRAY_GET_INT(interacts, attack_from);
	DBCREQ_ARRAY_GET_INT(interacts, attack_to);
	DBCREQ_ARRAY_GET_INT(interacts, retaliate_count);
	DBCREQ_ARRAY_GET_INT(interacts, last_attack_time);
	DBCREQ_ARRAY_GET_INT(interacts, truce_time);
	DBCREQ_ARRAY_GET_END();

	return 0;
}

int CDataAllianceInteract::GetAllianceInteracts(unsigned uid, map<unsigned, DataUserInteract> &interacts)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(opposite_uid);
	DBCREQ_NEED(help_from);
	DBCREQ_NEED(help_to);
	DBCREQ_NEED(attack_from);
	DBCREQ_NEED(attack_to);
	DBCREQ_NEED(retaliate_count);
	DBCREQ_NEED(last_attack_time);
	DBCREQ_NEED(truce_time);

	DBCREQ_EXEC;

	interacts.clear();
	int count = m_dbcret.TotalRows();
	for(int i = 0; i < count; i++)
	{
		DBCREQ_IFFETCHROW;
		DataUserInteract &interact = interacts[m_dbcret.IntValue(2)];
		DBCREQ_GET_BEGIN();
		DBCREQ_GET_INT(interact, uid);
		DBCREQ_GET_INT(interact, opposite_uid);
		DBCREQ_GET_INT(interact, help_from);
		DBCREQ_GET_INT(interact, help_to);
		DBCREQ_GET_INT(interact, attack_from);
		DBCREQ_GET_INT(interact, attack_to);
		DBCREQ_GET_INT(interact, retaliate_count);
		DBCREQ_GET_INT(interact, last_attack_time);
		DBCREQ_GET_INT(interact, truce_time);
	}

	return 0;
}

int CDataAllianceInteract::GetAllianceInteractsAttackAfter(unsigned uid, unsigned last_attack_time, vector<DataUserInteract> &interacts)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(GE, last_attack_time, last_attack_time);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(opposite_uid);
	DBCREQ_NEED(help_from);
	DBCREQ_NEED(help_to);
	DBCREQ_NEED(attack_from);
	DBCREQ_NEED(attack_to);
	DBCREQ_NEED(retaliate_count);
	DBCREQ_NEED(last_attack_time);
	DBCREQ_NEED(truce_time);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(interacts);
	DBCREQ_ARRAY_GET_INT(interacts, uid);
	DBCREQ_ARRAY_GET_INT(interacts, opposite_uid);
	DBCREQ_ARRAY_GET_INT(interacts, help_from);
	DBCREQ_ARRAY_GET_INT(interacts, help_to);
	DBCREQ_ARRAY_GET_INT(interacts, attack_from);
	DBCREQ_ARRAY_GET_INT(interacts, attack_to);
	DBCREQ_ARRAY_GET_INT(interacts, retaliate_count);
	DBCREQ_ARRAY_GET_INT(interacts, last_attack_time);
	DBCREQ_ARRAY_GET_INT(interacts, truce_time);
	DBCREQ_ARRAY_GET_END();

	return 0;
}

int CDataAllianceInteract::RemoveAllianceInteracts(unsigned uid)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_EXEC;
	return 0;
}

int CDataAllianceInteract::RemoveAllianceInteracts(unsigned uid, unsigned opposite_uid)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, opposite_uid, opposite_uid);
	DBCREQ_EXEC;
	return 0;
}

