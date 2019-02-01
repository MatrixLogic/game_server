#include "DataUserBasic.h"

int CDataUserBasic::AddUserBasic(const DataUserBasic &user)
{
	DBCREQ_DECLARE(DBC::InsertRequest, user.uid);
	DBCREQ_SET_KEY(user.uid);

	DBCREQ_SET_INT(user, platform);
	DBCREQ_SET_INT(user, is_register_platform);
	DBCREQ_SET_STR(user, open_id);
	DBCREQ_SET_STR(user, name);
	DBCREQ_SET_INT(user, gender);
	DBCREQ_SET_STR(user, nation);
	DBCREQ_SET_STR(user, province);
	DBCREQ_SET_STR(user, city);
	DBCREQ_SET_STR(user, figure_url);
	DBCREQ_SET_INT(user, vip_type);
	DBCREQ_SET_INT(user, vip_level);
	DBCREQ_SET_INT(user, fnum);
	DBCREQ_SET_STR(user, friends);
	DBCREQ_SET_STR(user, extra);
	DBCREQ_EXEC;
	return 0;
}

int CDataUserBasic::SetUserBasic(unsigned uid, int platform, const DataUserBasic &user)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, platform, platform);

	DBCREQ_SET_INT(user, is_register_platform);
	DBCREQ_SET_STR(user, open_id);
	DBCREQ_SET_STR(user, name);
	DBCREQ_SET_INT(user, gender);
	DBCREQ_SET_STR(user, nation);
	DBCREQ_SET_STR(user, province);
	DBCREQ_SET_STR(user, city);
	DBCREQ_SET_STR(user, figure_url);
	DBCREQ_SET_INT(user, vip_type);
	DBCREQ_SET_INT(user, vip_level);
	DBCREQ_SET_INT(user, fnum);
	DBCREQ_SET_STR(user, friends);
	DBCREQ_SET_STR(user, extra);
	DBCREQ_EXEC;
	return 0;
}

int CDataUserBasic::GetUserBasic(unsigned uid, int platform, DataUserBasic &user)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, platform, platform);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(platform);
	DBCREQ_NEED(is_register_platform);
	DBCREQ_NEED(open_id);
	DBCREQ_NEED(name);
	DBCREQ_NEED(gender);
	DBCREQ_NEED(nation);
	DBCREQ_NEED(province);
	DBCREQ_NEED(city);
	DBCREQ_NEED(figure_url);
	DBCREQ_NEED(vip_type);
	DBCREQ_NEED(vip_level);
	DBCREQ_NEED(fnum);
	DBCREQ_NEED(friends);
	DBCREQ_NEED(extra);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(user, uid);
	DBCREQ_GET_INT(user, platform);
	DBCREQ_GET_INT(user, is_register_platform);
	DBCREQ_GET_STR(user, open_id);
	DBCREQ_GET_STR(user, name);
	DBCREQ_GET_INT(user, gender);
	DBCREQ_GET_STR(user, nation);
	DBCREQ_GET_STR(user, province);
	DBCREQ_GET_STR(user, city);
	DBCREQ_GET_STR(user, figure_url);
	DBCREQ_GET_INT(user, vip_type);
	DBCREQ_GET_INT(user, vip_level);
	DBCREQ_GET_INT(user, fnum);
	DBCREQ_GET_STR(user, friends);
	DBCREQ_GET_STR(user, extra);

	return 0;
}

int CDataUserBasic::SetUserBasicLimit(unsigned uid, int platform, const DataUserBasic &user)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, platform, platform);

	DBCREQ_SET_INT(user, is_register_platform);
	DBCREQ_SET_STR(user, open_id);
	DBCREQ_SET_STR(user, name);
	DBCREQ_SET_INT(user, gender);
	DBCREQ_SET_STR(user, nation);
	DBCREQ_SET_STR(user, province);
	DBCREQ_SET_STR(user, city);
	DBCREQ_SET_STR(user, figure_url);
	DBCREQ_SET_INT(user, vip_type);
	DBCREQ_SET_INT(user, vip_level);
	DBCREQ_SET_INT(user, fnum);
	DBCREQ_SET_STR(user, extra);
	DBCREQ_EXEC;
	return 0;
}

int CDataUserBasic::GetUserBasicLimit(unsigned uid, int platform, DataUserBasic &user)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, platform, platform);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(platform);
	DBCREQ_NEED(is_register_platform);
	DBCREQ_NEED(open_id);
	DBCREQ_NEED(name);
	DBCREQ_NEED(gender);
	DBCREQ_NEED(nation);
	DBCREQ_NEED(province);
	DBCREQ_NEED(city);
	DBCREQ_NEED(figure_url);
	DBCREQ_NEED(vip_type);
	DBCREQ_NEED(vip_level);
	DBCREQ_NEED(fnum);
	DBCREQ_NEED(extra);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(user, uid);
	DBCREQ_GET_INT(user, platform);
	DBCREQ_GET_INT(user, is_register_platform);
	DBCREQ_GET_STR(user, open_id);
	DBCREQ_GET_STR(user, name);
	DBCREQ_GET_INT(user, gender);
	DBCREQ_GET_STR(user, nation);
	DBCREQ_GET_STR(user, province);
	DBCREQ_GET_STR(user, city);
	DBCREQ_GET_STR(user, figure_url);
	DBCREQ_GET_INT(user, vip_type);
	DBCREQ_GET_INT(user, vip_level);
	DBCREQ_GET_INT(user, fnum);
	DBCREQ_GET_STR(user, extra);

	return 0;
}

int CDataUserBasic::GetUserBasicRegisterLimit(unsigned uid, DataUserBasic &user)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, is_register_platform, 1);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(platform);
	DBCREQ_NEED(is_register_platform);
	DBCREQ_NEED(open_id);
	DBCREQ_NEED(name);
	DBCREQ_NEED(gender);
	DBCREQ_NEED(nation);
	DBCREQ_NEED(province);
	DBCREQ_NEED(city);
	DBCREQ_NEED(figure_url);
	DBCREQ_NEED(vip_type);
	DBCREQ_NEED(vip_level);
	DBCREQ_NEED(fnum);
	DBCREQ_NEED(extra);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(user, uid);
	DBCREQ_GET_INT(user, platform);
	DBCREQ_GET_INT(user, is_register_platform);
	DBCREQ_GET_STR(user, open_id);
	DBCREQ_GET_STR(user, name);
	DBCREQ_GET_INT(user, gender);
	DBCREQ_GET_STR(user, nation);
	DBCREQ_GET_STR(user, province);
	DBCREQ_GET_STR(user, city);
	DBCREQ_GET_STR(user, figure_url);
	DBCREQ_GET_INT(user, vip_type);
	DBCREQ_GET_INT(user, vip_level);
	DBCREQ_GET_INT(user, fnum);
	DBCREQ_GET_STR(user, extra);
	return 0;
}

int CDataUserBasic::GetOpenidList(unsigned uid, vector<DataOpenidPlatform> &openids)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(platform);
	DBCREQ_NEED(open_id);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(openids);
	DBCREQ_ARRAY_GET_INT(openids, platform);
	DBCREQ_ARRAY_GET_STR(openids, open_id);
	DBCREQ_ARRAY_GET_END();

	return 0;
}

int CDataUserBasic::RemoveUserBasic(unsigned uid)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_EXEC;
	return 0;
}

int CDataUserBasic::SetFriends(unsigned uid, int platform, int fnum, const string &friends)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, platform, platform);

	DBCREQ_SET_INT_S(fnum);
	DBCREQ_SET_STR_S(friends);
	DBCREQ_EXEC;
	return 0;
}

int CDataUserBasic::GetFriends(unsigned uid, int platform, string &friends)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, platform, platform);
	DBCREQ_GET_STR_V(friends);
	return 0;
}

int CDataUserBasic::SetOidFriends(unsigned uid, int platform, const string &oid_friends)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, platform, platform);

	DBCREQ_SET_STR_S(oid_friends);
	DBCREQ_EXEC;
	return 0;
}

int CDataUserBasic::GetOidFriends(unsigned uid, int platform, string &oid_friends)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, platform, platform);
	DBCREQ_GET_STR_V(oid_friends);
	return 0;
}

int CDataUserBasic::AddUserBasicExt(const DataUserBasic &user)
{
	DBCREQ_DECLARE(DBC::InsertRequest, user.uid);
	DBCREQ_SET_KEY(user.uid);

	DBCREQ_SET_INT(user, platform);
	DBCREQ_SET_INT(user, is_register_platform);
	DBCREQ_SET_STR(user, open_id);
	DBCREQ_SET_STR(user, name);
	DBCREQ_SET_INT(user, gender);
	DBCREQ_SET_STR(user, nation);
	DBCREQ_SET_STR(user, province);
	DBCREQ_SET_STR(user, city);
	DBCREQ_SET_STR(user, figure_url);
	DBCREQ_SET_INT(user, vip_type);
	DBCREQ_SET_INT(user, vip_level);
	DBCREQ_SET_INT(user, fnum);
	DBCREQ_SET_STR(user, friends);
	DBCREQ_SET_STR(user, oid_friends);
	DBCREQ_SET_STR(user, extra);
	DBCREQ_EXEC;
	return 0;
}

int CDataUserBasic::GetUserBasicExt(unsigned uid, int platform, DataUserBasic &user)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, platform, platform);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(platform);
	DBCREQ_NEED(is_register_platform);
	DBCREQ_NEED(open_id);
	DBCREQ_NEED(name);
	DBCREQ_NEED(gender);
	DBCREQ_NEED(nation);
	DBCREQ_NEED(province);
	DBCREQ_NEED(city);
	DBCREQ_NEED(figure_url);
	DBCREQ_NEED(vip_type);
	DBCREQ_NEED(vip_level);
	DBCREQ_NEED(fnum);
	DBCREQ_NEED(friends);
	DBCREQ_NEED(oid_friends);
	DBCREQ_NEED(extra);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT(user, uid);
	DBCREQ_GET_INT(user, platform);
	DBCREQ_GET_INT(user, is_register_platform);
	DBCREQ_GET_STR(user, open_id);
	DBCREQ_GET_STR(user, name);
	DBCREQ_GET_INT(user, gender);
	DBCREQ_GET_STR(user, nation);
	DBCREQ_GET_STR(user, province);
	DBCREQ_GET_STR(user, city);
	DBCREQ_GET_STR(user, figure_url);
	DBCREQ_GET_INT(user, vip_type);
	DBCREQ_GET_INT(user, vip_level);
	DBCREQ_GET_INT(user, fnum);
	DBCREQ_GET_STR(user, friends);
	DBCREQ_GET_STR(user, oid_friends);
	DBCREQ_GET_STR(user, extra);
	return 0;
}

int CDataUserBasic::SetUserBasicExt(unsigned uid, int platform, const DataUserBasic &user)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, platform, platform);

	DBCREQ_SET_INT(user, is_register_platform);
	DBCREQ_SET_STR(user, open_id);
	DBCREQ_SET_STR(user, name);
	DBCREQ_SET_INT(user, gender);
	DBCREQ_SET_STR(user, nation);
	DBCREQ_SET_STR(user, province);
	DBCREQ_SET_STR(user, city);
	DBCREQ_SET_STR(user, figure_url);
	DBCREQ_SET_INT(user, vip_type);
	DBCREQ_SET_INT(user, vip_level);
	DBCREQ_SET_INT(user, fnum);
	DBCREQ_SET_STR(user, friends);
	DBCREQ_SET_STR(user, oid_friends);
	DBCREQ_SET_STR(user, extra);
	DBCREQ_EXEC;
	return 0;
}

int CDataUserBasic::GetUserName(unsigned uid, int platform, string &name)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, platform, platform);
	DBCREQ_GET_STR_V(name);
	return 0;
}

int CDataUserBasic::GetWorldUserInfo(int unsigned uid,int platform,string &signature,string &figure_url,string &name)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, platform, platform);
	DBCREQ_NEED_BEGIN()	;
	DBCREQ_NEED(signature);
	DBCREQ_NEED(figure_url);
	DBCREQ_NEED(name);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_STR_S(signature);
	DBCREQ_GET_STR_S(figure_url);
	DBCREQ_GET_STR_S(name);
	return 0;

}
int CDataUserBasic::SetWorldUserInfo(int unsigned uid,int platform, const string &signature)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_CONDITION(EQ, platform, platform);
	DBCREQ_SET_STR_V(signature);
	DBCREQ_EXEC;
	return 0;
}

