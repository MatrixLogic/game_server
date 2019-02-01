#include "DataUserNomenate.h"

int CDataUserNomenate::addNomenate(const DataUserNomenate &data)
{
	DBCREQ_DECLARE(DBC::InsertRequest, data.uid);
	DBCREQ_SET_KEY(data.uid);

	DBCREQ_SET_STR(data, rec_list);
	DBCREQ_SET_STR(data, add_list);
	DBCREQ_SET_STR(data, black_list);
	DBCREQ_SET_STR(data, friend_list);
	DBCREQ_SET_INT(data, update_time);
	DBCREQ_EXEC;

	return 0;
}
int CDataUserNomenate::setNomenate(unsigned uid, const DataUserNomenate &data)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_STR(data, rec_list);
	DBCREQ_SET_STR(data, add_list);
	DBCREQ_SET_STR(data, black_list);
	DBCREQ_SET_STR(data, friend_list);
	DBCREQ_SET_INT(data, update_time);
	DBCREQ_EXEC;

	return 0;
}
int CDataUserNomenate::getNomenate(unsigned uid, DataUserNomenate &data)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(rec_list);
	DBCREQ_NEED(add_list);
	DBCREQ_NEED(black_list);
	DBCREQ_NEED(friend_list);
	DBCREQ_NEED(update_time);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(data, uid);
	DBCREQ_GET_STR(data, rec_list);
	DBCREQ_GET_STR(data, add_list);
	DBCREQ_GET_STR(data, black_list);
	DBCREQ_GET_STR(data, friend_list);
	DBCREQ_GET_INT(data, update_time);

	return 0;

}
int CDataUserNomenate::deleteNomenate(unsigned uid)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_EXEC;

	return 0;
}
int CDataUserNomenate::setRecList(unsigned uid, const string &rec_list)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_STR_S(rec_list);
	DBCREQ_EXEC;
	return 0;
}
int CDataUserNomenate::getRecList(unsigned uid, string &rec_list)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_GET_STR_V(rec_list);

	return 0;
}
int CDataUserNomenate::setAddList(unsigned uid, const string &add_list)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_STR_S(add_list);
	DBCREQ_EXEC;
	return 0;
}
int CDataUserNomenate::getAddList(unsigned uid, string &add_list)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_GET_STR_V(add_list);

	return 0;
}
int CDataUserNomenate::setBlackList(unsigned uid, const string &black_list)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_STR_S(black_list);
	DBCREQ_EXEC;
	return 0;
}
int CDataUserNomenate::getBlackList(unsigned uid, string &black_list)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_GET_STR_V(black_list);

	return 0;
}
int CDataUserNomenate::setFriendList(unsigned uid, const string &friend_list)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_SET_STR_S(friend_list);
	DBCREQ_EXEC;
	return 0;
}
int CDataUserNomenate::getFriendList(unsigned uid, string &friend_list)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_GET_STR_V(friend_list);

	return 0;
}
