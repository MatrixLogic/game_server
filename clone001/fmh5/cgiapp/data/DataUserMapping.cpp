/*
 * DataUserMapping.cpp
 *
 *  Created on: 2011-7-8
 *      Author: dada
 */

#include "DataUserMapping.h"
#include "crc.h"

int CDataUserMapping::AddMapping(const string &openid, int platform, unsigned uid , string& via)
{
	unsigned crc = CCRC::GetCrc32(openid);
	if (crc > UID_MAX) {
		crc %= UID_MAX;
		if (crc < UID_MIN) {
			crc += UID_MIN;
		}
	}
	int serverid = 0;
	Config::GetDB(serverid);
	//debug_log("serverid=%d", serverid);
	int updatetime = Time::GetGlobalTime();
	int lastlogin = Time::GetGlobalTime();
	DBCREQ_DECLARE(DBC::InsertRequest, crc);
	DBCREQ_SET_KEY(openid.c_str());
	DBCREQ_SET_INT_S(uid);
	DBCREQ_SET_INT_S(platform);
	DBCREQ_SET_INT_S(serverid);
	DBCREQ_SET_INT_S(updatetime);
	DBCREQ_SET_INT_S(lastlogin);
	DBCREQ_SET_STR_S(via);
	DBCREQ_EXEC;
	return 0;
}

int CDataUserMapping::GetMapping(const string &openid, int platform,
		unsigned &uid) {
	unsigned crc = CCRC::GetCrc32(openid);
	if (crc > UID_MAX) {
		crc %= UID_MAX;
		if (crc < UID_MIN) {
			crc += UID_MIN;
		}
	}
	int serverid = 0;
	Config::GetDB(serverid);
	//debug_log("serverid=%d", serverid);
	DBCREQ_DECLARE(DBC::GetRequest, crc);
	DBCREQ_SET_KEY(openid.c_str());
	//DBCREQ_SET_CONDITION(EQ, platform, platform);
	DBCREQ_SET_CONDITION(EQ, serverid, serverid);
	DBCREQ_GET_INT_V(uid);
	return 0;
}

int CDataUserMapping::GetMapping(const string &openid, unsigned serverid, unsigned &uid, unsigned &updatetime, unsigned &lastlogin)
{
	unsigned crc = CCRC::GetCrc32(openid);
	if (crc > UID_MAX) {
		crc %= UID_MAX;
		if (crc < UID_MIN) {
			crc += UID_MIN;
		}
	}

	DBCREQ_DECLARE(DBC::GetRequest, crc);
	DBCREQ_SET_KEY(openid.c_str());
	DBCREQ_SET_CONDITION(EQ, serverid, serverid);

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);
	DBCREQ_NEED(updatetime);
	DBCREQ_NEED(lastlogin);

	DBCREQ_EXEC;
	DBCREQ_IFNULLROW;
	DBCREQ_IFFETCHROW;

	DBCREQ_GET_BEGIN();
	DBCREQ_GET_INT_S(uid);
	DBCREQ_GET_INT_S(updatetime);
	DBCREQ_GET_INT_S(lastlogin);

	return 0;
}

int CDataUserMapping::RemoveMapping(const string &openid, int platform) {
	unsigned crc = CCRC::GetCrc32(openid);
	if (crc > UID_MAX) {
		crc %= UID_MAX;
		if (crc < UID_MIN) {
			crc += UID_MIN;
		}
	}
	int serverid = 0;
	Config::GetDB(serverid);
	DBCREQ_DECLARE(DBC::DeleteRequest, crc);
	DBCREQ_SET_KEY(openid.c_str());
	//DBCREQ_SET_CONDITION(EQ, platform, platform);
	DBCREQ_SET_CONDITION(EQ, serverid, serverid);
	DBCREQ_EXEC;
	return 0;
}

int CDataUserMapping::GetAllMapping(const string &openid, vector<unsigned> &uid) {
	unsigned crc = CCRC::GetCrc32(openid);
	if (crc > UID_MAX) {
		crc %= UID_MAX;
		if (crc < UID_MIN) {
			crc += UID_MIN;
		}
	}

	DBCREQ_DECLARE(DBC::GetRequest, crc);
	DBCREQ_SET_KEY(openid.c_str());

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(uid);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(uid);
	DBCREQ_ARRAY_GET_INT_S(uid, uid);
	DBCREQ_ARRAY_GET_END();

	return 0;
}

int CDataUserMapping::GetAllRegisterTime(const string &openid, vector<unsigned> &registertime) {
	unsigned crc = CCRC::GetCrc32(openid);
	if (crc > UID_MAX) {
		crc %= UID_MAX;
		if (crc < UID_MIN) {
			crc += UID_MIN;
		}
	}

	DBCREQ_DECLARE(DBC::GetRequest, crc);
	DBCREQ_SET_KEY(openid.c_str());

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(updatetime);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(registertime);
	DBCREQ_ARRAY_GET_INT_S(registertime, updatetime);
	DBCREQ_ARRAY_GET_END();

	return 0;
}

int CDataUserMapping::GetUID(const string &openid, unsigned serverid, unsigned &uid){
	unsigned crc = CCRC::GetCrc32(openid);
	if (crc > UID_MAX) {
		crc %= UID_MAX;
		if (crc < UID_MIN) {
			crc += UID_MIN;
		}
	}

	DBCREQ_DECLARE(DBC::GetRequest, crc);
	DBCREQ_SET_KEY(openid.c_str());
	DBCREQ_SET_CONDITION(EQ, serverid, serverid);
	DBCREQ_GET_INT_V(uid);

	return 0;
}

int CDataUserMapping::UpdateLastLogin(const string &openid, int platform)
{
	unsigned crc = CCRC::GetCrc32(openid);
	if (crc > UID_MAX) {
		crc %= UID_MAX;
		if (crc < UID_MIN) {
			crc += UID_MIN;
		}
	}
	int lastlogin = Time::GetGlobalTime();
	int serverid = 0;
	Config::GetDB(serverid);
	//debug_log("serverid=%d", serverid);
	DBCREQ_DECLARE(DBC::UpdateRequest, crc);
	DBCREQ_SET_KEY(openid.c_str());
	//DBCREQ_SET_CONDITION(EQ, platform, platform);
	DBCREQ_SET_CONDITION(EQ, serverid, serverid);

	DBCREQ_SET_INT_S(lastlogin);
	DBCREQ_EXEC;
	return 0;
}

int CDataUserMapping::SetLastLogin(const string &openid, unsigned serverid, unsigned lastlogin)
{
	unsigned crc = CCRC::GetCrc32(openid);
	if (crc > UID_MAX) {
		crc %= UID_MAX;
		if (crc < UID_MIN) {
			crc += UID_MIN;
		}
	}
	DBCREQ_DECLARE(DBC::UpdateRequest, crc);
	DBCREQ_SET_KEY(openid.c_str());
	DBCREQ_SET_CONDITION(EQ, serverid, serverid);

	DBCREQ_SET_INT_S(lastlogin);
	DBCREQ_EXEC;
	return 0;
}

int CDataUserMapping::GetServer(const string &openid, vector<server> &servers)
{
	unsigned crc = CCRC::GetCrc32(openid);
	if (crc > UID_MAX) {
		crc %= UID_MAX;
		if (crc < UID_MIN) {
			crc += UID_MIN;
		}
	}

	DBCREQ_DECLARE(DBC::GetRequest, crc);
	DBCREQ_SET_KEY(openid.c_str());

	DBCREQ_NEED_BEGIN();
	DBCREQ_NEED(serverid);
	DBCREQ_NEED(lastlogin);

	DBCREQ_EXEC;

	DBCREQ_ARRAY_GET_BEGIN(servers);
	DBCREQ_ARRAY_GET_INT(servers, serverid);
	DBCREQ_ARRAY_GET_INT(servers, lastlogin);
	DBCREQ_ARRAY_GET_END();

	return 0;
}

int CDataUserMapping::SetTime(const string &openid, unsigned ts)
{
	unsigned crc = CCRC::GetCrc32(openid);
	if (crc > UID_MAX) {
		crc %= UID_MAX;
		if (crc < UID_MIN) {
			crc += UID_MIN;
		}
	}
	int lastlogin = ts;
	int updatetime = ts;
	int serverid = 0;
	Config::GetDB(serverid);
	//debug_log("serverid=%d", serverid);
	DBCREQ_DECLARE(DBC::UpdateRequest, crc);
	DBCREQ_SET_KEY(openid.c_str());
	//DBCREQ_SET_CONDITION(EQ, platform, platform);
	DBCREQ_SET_CONDITION(EQ, serverid, serverid);

	DBCREQ_SET_INT_S(lastlogin);
	DBCREQ_SET_INT_S(updatetime);
	DBCREQ_EXEC;
	return 0;
}
