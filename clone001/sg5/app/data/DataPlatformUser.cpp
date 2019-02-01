/*
 * DataPlatformUser.cpp
 *
 *  Created on: 2011-12-14
 *      Author: dada
 */

#include "DataPlatformUser.h"

int CDataPlatformUser::AddData(const string &openid, int platform, const string &data)
{
	DBCREQ_DECLARE(DBC::InsertRequest, CCRC::GetCrc32(openid));
	DBCREQ_SET_KEY(openid.c_str());
	DBCREQ_SET_INT_S(platform);
	DBCREQ_SET_STR_S(data);
	DBCREQ_EXEC;
	return 0;
}

int CDataPlatformUser::SetData(const string &openid, int platform, const string &data)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, CCRC::GetCrc32(openid));
	DBCREQ_SET_KEY(openid.c_str());
	DBCREQ_SET_CONDITION(EQ, platform, platform);
	DBCREQ_SET_STR_V(data);
	return 0;
}

int CDataPlatformUser::GetData(const string &openid, int platform, string &data)
{
	DBCREQ_DECLARE(DBC::GetRequest, CCRC::GetCrc32(openid));
	DBCREQ_SET_KEY(openid.c_str());
	DBCREQ_SET_CONDITION(EQ, platform, platform);
	DBCREQ_GET_STR_V(data);
	return 0;
}

int CDataPlatformUser::RemoveData(const string &openid, int platform)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, CCRC::GetCrc32(openid));
	DBCREQ_SET_KEY(openid.c_str());
	DBCREQ_SET_CONDITION(EQ, platform, platform);
	DBCREQ_EXEC;
	return 0;
}
