/*
 * DataUserBasic.h
 *
 *  Created on: 2011-7-8
 *      Author: dada
 */

#ifndef DATAUSERBASIC_H_
#define DATAUSERBASIC_H_

#include "Kernel.h"

struct DataUserBasic
{
	unsigned uid;
	int      platform;
	int      is_register_platform;
	string   open_id;
	string   name;
	int      gender;
	string   nation;
	string   province;
	string   city;
	string   figure_url;
	int      vip_type;
	int      vip_level;
	int      fnum;
	string   friends;
	string   oid_friends;
	string   extra;
	string   signature;

	DataUserBasic& operator = (DataUserBasic& userBasic)
	{
		if (this == &userBasic)
			return *this;
		this->uid = userBasic.uid;
		this->platform = userBasic.platform;
		this->is_register_platform = userBasic.is_register_platform;
		this->open_id = userBasic.open_id;
		this->name = userBasic.name;
		this->gender = userBasic.gender;
		this->nation = userBasic.nation;
		this->province = userBasic.province;
		this->city = userBasic.city;
		this->figure_url = userBasic.figure_url;
		this->vip_type = userBasic.vip_type;
		this->vip_level = userBasic.vip_level;
		this->fnum = userBasic.fnum;
		this->friends = userBasic.friends;
		this->oid_friends = userBasic.oid_friends;
		this->extra = userBasic.extra;
		this->signature = userBasic.signature;
		return *this;
	}
};

struct DataOpenidPlatform
{
	int    platform;
	string open_id;
};

class CDataUserBasic : public CDataBaseDBC
{
public:
	CDataUserBasic(int table = DB_USER_BASIC) : CDataBaseDBC(table) {}

	//no include oid_friends
	int AddUserBasic(const DataUserBasic &user);
	int SetUserBasic(unsigned uid, int platform, const DataUserBasic &user);
	int GetUserBasic(unsigned uid, int platform, DataUserBasic &user);

	int SetUserBasicLimit(unsigned uid, int platform, const DataUserBasic &user);
	int GetUserBasicLimit(unsigned uid, int platform, DataUserBasic &user);	//不返回friends

	int GetUserBasicRegisterLimit(unsigned uid, DataUserBasic &user);
	int GetOpenidList(unsigned uid, vector<DataOpenidPlatform> &openids);
	int RemoveUserBasic(unsigned uid);

	int SetFriends(unsigned uid, int platform, int fnum, const string &friends);
	int GetFriends(unsigned uid, int platform, string &friends);

	int SetOidFriends(unsigned uid, int platform, const string &oid_friends);
	int GetOidFriends(unsigned uid, int platform, string &oid_friends);

	//include oid_friends
	int AddUserBasicExt(const DataUserBasic &user);
	int GetUserBasicExt(unsigned uid, int platform, DataUserBasic &user);
	int SetUserBasicExt(unsigned uid, int platform, const DataUserBasic &user);

	int GetUserName(unsigned uid, int platform, string &name);
	int GetWorldUserInfo(int unsigned uid,int platform,string &signature,string &figure_url,string &name);
	int SetWorldUserInfo(int unsigned uid,int platform, const string &signature);
};

#endif /* DATAUSERBASIC_H_ */
