/*
 * DataUserMapping.cpp
 *
 *  Created on: 2011-7-8
 *      Author: dada
 */

#include "DataUserMapping.h"
#include "crc.h"

int CDataUserMapping::AddMapping(const string &openid, int platform,
		unsigned uid) {
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
	DBCREQ_DECLARE(DBC::InsertRequest, crc);
	DBCREQ_SET_KEY(openid.c_str());
	DBCREQ_SET_INT_S(uid);
	DBCREQ_SET_INT_S(platform);
	DBCREQ_SET_INT_S(serverid);
	DBCREQ_SET_INT_S(updatetime);
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
	DBCREQ_SET_CONDITION(EQ, platform, platform);
	DBCREQ_SET_CONDITION(EQ, serverid, serverid);
	DBCREQ_GET_INT_V(uid);
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
	DBCREQ_SET_CONDITION(EQ, platform, platform);
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
