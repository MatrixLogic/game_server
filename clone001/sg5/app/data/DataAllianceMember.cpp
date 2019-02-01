/*
 * DataAllianceMember.cpp
 *
 *  Created on: 2011-9-19
 *      Author: dada
 */

#include "DataAllianceMember.h"

int CDataAllianceMember::AddMember(const DataAllianceMember &member)
{
	DBCREQ_DECLARE(DBC::InsertRequest, member.alliance_id);
	DBCREQ_SET_KEY(member.alliance_id);
	DBCREQ_SET_INT(member, uid);
	DBCREQ_SET_INT(member, type);
	DBCREQ_SET_INT(member, point);
	DBCREQ_SET_INT(member, curr_point);
	DBCREQ_SET_INT(member, join_time);
	DBCREQ_SET_INT(member, donate_rank);
	DBCREQ_SET_INT(member, donate_drank);
	DBCREQ_SET_STR(member, extra_data);
	DBCREQ_SET_STR(member, donate_data);
	DBCREQ_EXEC;
	return 0;
}

int CDataAllianceMember::SetMember(const DataAllianceMember &member)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, member.alliance_id);
	DBCREQ_SET_KEY(member.alliance_id);
	DBCREQ_SET_CONDITION(EQ, uid, member.uid);
	DBCREQ_SET_INT(member, type);
	DBCREQ_SET_INT(member, point);
	DBCREQ_SET_INT(member, curr_point);
	DBCREQ_SET_INT(member, join_time);
	DBCREQ_SET_INT(member, donate_rank);
	DBCREQ_SET_INT(member, donate_drank);
	DBCREQ_SET_STR(member, extra_data);
	DBCREQ_SET_STR(member, donate_data);
	DBCREQ_EXEC;
	return 0;
}

int CDataAllianceMember::GetMember(unsigned alliance_id, unsigned uid, DataAllianceMember &member)
{
	DBCREQ_DECLARE(DBC::GetRequest, alliance_id);
	DBCREQ_SET_KEY(alliance_id);
	DBCREQ_SET_CONDITION(EQ, uid, uid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(alliance_id);
	DBCREQ_NEED(uid);
	DBCREQ_NEED(type);
	DBCREQ_NEED(point);
	DBCREQ_NEED(curr_point);
	DBCREQ_NEED(join_time);
	DBCREQ_NEED(extra_data);
	DBCREQ_NEED(donate_data);
	DBCREQ_NEED(donate_rank);
	DBCREQ_NEED(donate_drank);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(member, alliance_id);
	DBCREQ_GET_INT(member, uid);
	DBCREQ_GET_INT(member, type);
	DBCREQ_GET_INT(member, point);
	DBCREQ_GET_INT(member, curr_point);
	DBCREQ_GET_INT(member, join_time);
	DBCREQ_GET_STR(member, extra_data);
	DBCREQ_GET_STR(member, donate_data);
	DBCREQ_GET_INT(member, donate_rank);
	DBCREQ_GET_INT(member, donate_drank);

	return 0;
}

int CDataAllianceMember::GetMembers(unsigned alliance_id, vector<DataAllianceMember> &members)
{
	DBCREQ_DECLARE(DBC::GetRequest, alliance_id);
	DBCREQ_SET_KEY(alliance_id);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(alliance_id);
	DBCREQ_NEED(uid);
	DBCREQ_NEED(type);
	DBCREQ_NEED(point);
	DBCREQ_NEED(curr_point);
	DBCREQ_NEED(join_time);
	DBCREQ_NEED(extra_data);
	DBCREQ_NEED(donate_data);
	DBCREQ_NEED(donate_rank);
	DBCREQ_NEED(donate_drank);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(members);
	DBCREQ_ARRAY_GET_INT(members, alliance_id);
	DBCREQ_ARRAY_GET_INT(members, uid);
	DBCREQ_ARRAY_GET_INT(members, type);
	DBCREQ_ARRAY_GET_INT(members, point);
	DBCREQ_ARRAY_GET_INT(members, curr_point);
	DBCREQ_ARRAY_GET_INT(members, join_time);
	DBCREQ_ARRAY_GET_STR(members, extra_data);
	DBCREQ_ARRAY_GET_STR(members, donate_data);
	DBCREQ_ARRAY_GET_INT(members, donate_rank);
	DBCREQ_ARRAY_GET_INT(members, donate_drank);
	DBCREQ_ARRAY_GET_END();

	return 0;
}

int CDataAllianceMember::RemoveMember(unsigned alliance_id, unsigned uid)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, alliance_id);
	DBCREQ_SET_KEY(alliance_id);
	DBCREQ_SET_CONDITION(EQ, uid, uid);
	DBCREQ_EXEC;
	return 0;
}

int CDataAllianceMember::RemoveMembers(unsigned alliance_id)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, alliance_id);
	DBCREQ_SET_KEY(alliance_id);
	DBCREQ_EXEC;
	return 0;
}

int CDataAllianceMember::AddPoint(unsigned alliance_id, unsigned uid, int point)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, alliance_id);
	DBCREQ_SET_KEY(alliance_id);
	DBCREQ_SET_CONDITION(EQ, uid, uid);
	DBCREQ_ADD_INT_V(point);
	return 0;
}


int CDataAllianceMember::AddCurrPoint(unsigned alliance_id, unsigned uid, int curr_point)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, alliance_id);
	DBCREQ_SET_KEY(alliance_id);
	DBCREQ_SET_CONDITION(EQ, uid, uid);
	DBCREQ_ADD_INT_V(curr_point);
	return 0;
}

int CDataAllianceMember::SetMemberType(unsigned alliance_id, unsigned uid, int type)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, alliance_id);
	DBCREQ_SET_KEY(alliance_id);
	DBCREQ_SET_CONDITION(EQ, uid, uid);
	DBCREQ_SET_INT_V(type);
	return 0;
}

int CDataAllianceMember::GetMemberType(unsigned alliance_id, unsigned uid, int &type)
{
	DBCREQ_DECLARE(DBC::GetRequest, alliance_id);
	DBCREQ_SET_KEY(alliance_id);
	DBCREQ_SET_CONDITION(EQ, uid, uid);
	DBCREQ_GET_INT_V(type);
	return 0;
}
