#include "DataUserData.h"

int CDataUserData::AddUserData(DataUserData &data)
{
	DBCREQ_DECLARE(DBC::InsertRequest, data.uid);
	DBCREQ_SET_KEY(data.uid);

	DBCREQ_SET_STR(data, tribute_list);
	DBCREQ_SET_STR(data, npcattack);
	DBCREQ_SET_STR(data, gift);
	DBCREQ_SET_STR(data, ladder);
	DBCREQ_SET_STR(data, dailyquest);
	DBCREQ_SET_STR(data, worldpos_collect);

	DBCREQ_EXEC;
	return 0;
}

int CDataUserData::SetUserData(unsigned uid, const DataUserData &data)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_STR(data, tribute_list);
	DBCREQ_SET_STR(data, npcattack);
	DBCREQ_SET_STR(data, gift);
	DBCREQ_SET_STR(data, ladder);
	DBCREQ_SET_STR(data, dailyquest);
	DBCREQ_SET_STR(data, worldpos_collect);


	DBCREQ_EXEC;
	return 0;
}

int CDataUserData::GetUserData(unsigned uid, DataUserData &data)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(tribute_list);
	DBCREQ_NEED(npcattack);
	DBCREQ_NEED(gift);
	DBCREQ_NEED(ladder);
	DBCREQ_NEED(dailyquest);
	DBCREQ_NEED(worldpos_collect);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(data, uid);
	DBCREQ_GET_STR(data, tribute_list);
	DBCREQ_GET_STR(data, npcattack);
	DBCREQ_GET_STR(data, gift);
	DBCREQ_GET_STR(data, ladder);
	DBCREQ_GET_STR(data, dailyquest);
	DBCREQ_GET_STR(data, worldpos_collect);

	return 0;
}

int CDataUserData::RemoveUserData(unsigned uid)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_EXEC;
	return 0;
}

int CDataUserData::SetTributeList(unsigned uid, const string &tribute_list)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_STR_V(tribute_list);
	return 0;
}

int CDataUserData::SetNpcattack(unsigned uid, const string &npcattack)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_STR_V(npcattack);
	return 0;
}

int CDataUserData::SetGift(unsigned uid, const string &gift)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_STR_V(gift);
	return 0;
}

int CDataUserData::SetLadder(unsigned uid, const string &ladder)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_STR_V(ladder);
	return 0;
}

int CDataUserData::SetAllServerLadder(unsigned uid, const string &allserver_ladder)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_STR_V(allserver_ladder);
	return 0;
}

int CDataUserData::GetAllServerLadder(unsigned uid, string &allserver_ladder)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_GET_STR_V(allserver_ladder);
	return 0;
}

int CDataUserData::SetDailyQuest(unsigned uid, const string &dailyquest)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_STR_V(dailyquest);
	return 0;
}

int CDataUserData::GetWorldposCollect(unsigned uid,string &worldpos_collect)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_GET_STR_V(worldpos_collect);
	return 0;
}
int CDataUserData::SetWorldposCollect(unsigned uid,const string &worldpos_collect)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_STR_V(worldpos_collect);
	return 0;
}
