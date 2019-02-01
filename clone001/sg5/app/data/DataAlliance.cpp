/*
 * DataAlliance.cpp
 *
 *  Created on: 2011-9-19
 *      Author: dada
 */

#include "DataAlliance.h"

int CDataAlliance::AddAlliance(const DataAlliance &alliance)
{
	DBCREQ_DECLARE(DBC::InsertRequest, alliance.alliance_id);
	DBCREQ_SET_KEY(alliance.alliance_id);
	DBCREQ_SET_INT(alliance, kingdom);
	DBCREQ_SET_STR(alliance, name);
	DBCREQ_SET_INT(alliance, flag);
	DBCREQ_SET_INT(alliance, leader_uid);
	DBCREQ_SET_INT(alliance, create_time);
	DBCREQ_SET_INT(alliance, remove_time);
	DBCREQ_SET_INT(alliance, status);
	DBCREQ_SET_INT(alliance, level);
	DBCREQ_SET_INT(alliance, point);
	DBCREQ_SET_INT(alliance, curr_point);
	DBCREQ_SET_INT(alliance, rank);
	DBCREQ_SET_INT(alliance, member_count);
	DBCREQ_SET_INT(alliance, enemy_alliance_id);
	DBCREQ_SET_STR(alliance, enemy_alliance_name);
	DBCREQ_SET_STR(alliance, description);
	DBCREQ_SET_STR(alliance, notice);
	DBCREQ_SET_STR(alliance, activity);
	DBCREQ_SET_STR(alliance, extra_data);
	DBCREQ_EXEC;
	return 0;
}

int CDataAlliance::SetAlliance(const DataAlliance &alliance)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, alliance.alliance_id);
	DBCREQ_SET_KEY(alliance.alliance_id);
	DBCREQ_SET_INT(alliance, kingdom);
	DBCREQ_SET_STR(alliance, name);
	DBCREQ_SET_INT(alliance, flag);
	DBCREQ_SET_INT(alliance, leader_uid);
	DBCREQ_SET_INT(alliance, create_time);
	DBCREQ_SET_INT(alliance, remove_time);
	DBCREQ_SET_INT(alliance, status);
	DBCREQ_SET_INT(alliance, level);
	DBCREQ_SET_INT(alliance, point);
	DBCREQ_SET_INT(alliance, curr_point);
	DBCREQ_SET_INT(alliance, rank);
	DBCREQ_SET_INT(alliance, member_count);
	DBCREQ_SET_INT(alliance, enemy_alliance_id);
	DBCREQ_SET_STR(alliance, enemy_alliance_name);
	DBCREQ_SET_STR(alliance, description);
	DBCREQ_SET_STR(alliance, notice);
	DBCREQ_SET_STR(alliance, activity);
	DBCREQ_SET_STR(alliance, extra_data);
	DBCREQ_EXEC;
	return 0;
}

int CDataAlliance::GetAlliance(unsigned alliance_id, DataAlliance &alliance)
{
	DBCREQ_DECLARE(DBC::GetRequest, alliance_id);
	DBCREQ_SET_KEY(alliance_id);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(alliance_id);
	DBCREQ_NEED(kingdom);;
	DBCREQ_NEED(name);
	DBCREQ_NEED(flag);
	DBCREQ_NEED(leader_uid);
	DBCREQ_NEED(create_time);
	DBCREQ_NEED(remove_time);
	DBCREQ_NEED(status);
	DBCREQ_NEED(level);
	DBCREQ_NEED(point);
	DBCREQ_NEED(curr_point);
	DBCREQ_NEED(rank);
	DBCREQ_NEED(member_count);
	DBCREQ_NEED(enemy_alliance_id);
	DBCREQ_NEED(enemy_alliance_name);
	DBCREQ_NEED(description);
	DBCREQ_NEED(notice);
	DBCREQ_NEED(activity);
	DBCREQ_NEED(extra_data);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(alliance, alliance_id);
	DBCREQ_GET_INT(alliance, kingdom);
	DBCREQ_GET_STR(alliance, name);
	DBCREQ_GET_INT(alliance, flag);
	DBCREQ_GET_INT(alliance, leader_uid);
	DBCREQ_GET_INT(alliance, create_time);
	DBCREQ_GET_INT(alliance, remove_time);
	DBCREQ_GET_INT(alliance, status);
	DBCREQ_GET_INT(alliance, level);
	DBCREQ_GET_INT(alliance, point);
	DBCREQ_GET_INT(alliance, curr_point);
	DBCREQ_GET_INT(alliance, rank);
	DBCREQ_GET_INT(alliance, member_count);
	DBCREQ_GET_INT(alliance, enemy_alliance_id);
	DBCREQ_GET_STR(alliance, enemy_alliance_name);
	DBCREQ_GET_STR(alliance, description);
	DBCREQ_GET_STR(alliance, notice);
	DBCREQ_GET_STR(alliance, activity);
	DBCREQ_GET_STR(alliance, extra_data);

	return 0;
}

int CDataAlliance::RemoveAlliance(unsigned alliance_id)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, alliance_id);
	DBCREQ_SET_KEY(alliance_id);
	DBCREQ_EXEC;
	return 0;
}

int CDataAlliance::SetAllianceLimit(const DataAlliance &alliance)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, alliance.alliance_id);
	DBCREQ_SET_KEY(alliance.alliance_id);
	DBCREQ_SET_INT(alliance, kingdom);
	DBCREQ_SET_STR(alliance, name);
	DBCREQ_SET_INT(alliance, flag);
	DBCREQ_SET_INT(alliance, leader_uid);
	DBCREQ_SET_INT(alliance, create_time);
	DBCREQ_SET_INT(alliance, remove_time);
	DBCREQ_SET_INT(alliance, status);
	DBCREQ_SET_INT(alliance, level);
	DBCREQ_SET_INT(alliance, point);
	DBCREQ_SET_INT(alliance, curr_point);
	DBCREQ_SET_INT(alliance, rank);
	DBCREQ_SET_INT(alliance, member_count);
	DBCREQ_SET_INT(alliance, enemy_alliance_id);
	DBCREQ_SET_STR(alliance, enemy_alliance_name);
	DBCREQ_EXEC;
	return 0;
}

int CDataAlliance::GetAllianceLimit(unsigned alliance_id, DataAlliance &alliance)
{
	DBCREQ_DECLARE(DBC::GetRequest, alliance_id);
	DBCREQ_SET_KEY(alliance_id);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(alliance_id);
	DBCREQ_NEED(kingdom);
	DBCREQ_NEED(name);
	DBCREQ_NEED(flag);
	DBCREQ_NEED(leader_uid);
	DBCREQ_NEED(create_time);
	DBCREQ_NEED(remove_time);
	DBCREQ_NEED(status);
	DBCREQ_NEED(level);
	DBCREQ_NEED(point);
	DBCREQ_NEED(curr_point);
	DBCREQ_NEED(rank);
	DBCREQ_NEED(member_count);
	DBCREQ_NEED(enemy_alliance_id);
	DBCREQ_NEED(enemy_alliance_name);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(alliance, alliance_id);
	DBCREQ_GET_INT(alliance, kingdom);
	DBCREQ_GET_STR(alliance, name);
	DBCREQ_GET_INT(alliance, flag);
	DBCREQ_GET_INT(alliance, leader_uid);
	DBCREQ_GET_INT(alliance, create_time);
	DBCREQ_GET_INT(alliance, remove_time);
	DBCREQ_GET_INT(alliance, status);
	DBCREQ_GET_INT(alliance, level);
	DBCREQ_GET_INT(alliance, point);
	DBCREQ_GET_INT(alliance, curr_point);
	DBCREQ_GET_INT(alliance, rank);
	DBCREQ_GET_INT(alliance, member_count);
	DBCREQ_GET_INT(alliance, enemy_alliance_id);
	DBCREQ_GET_STR(alliance, enemy_alliance_name);

	return 0;
}

int CDataAlliance::AddPoint(unsigned alliance_id, int point)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, alliance_id);
	DBCREQ_SET_KEY(alliance_id);
	DBCREQ_ADD_INT_V(point);
	return 0;
}

int CDataAlliance::AddCurrPoint(unsigned alliance_id, int curr_point)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, alliance_id);
	DBCREQ_SET_KEY(alliance_id);
	DBCREQ_ADD_INT_V(curr_point);
	return 0;
}
