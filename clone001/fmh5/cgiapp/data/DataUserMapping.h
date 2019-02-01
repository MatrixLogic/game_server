/*
 * DataUserMapping.h
 *
 *  Created on: 2011-7-8
 *      Author: dada
 */

#ifndef DATAUSERMAPPING_H_
#define DATAUSERMAPPING_H_

#include "Kernel.h"

struct server
{
	unsigned serverid;
	unsigned lastlogin;
};

class CDataUserMapping : public CDataBaseDBC
{
public:
	CDataUserMapping(int table = DB_USER_MAPPING) : CDataBaseDBC(table) {}

public:
	int AddMapping(const string &openid, int platform, unsigned uid, string& via);
	int GetMapping(const string &openid, int platform, unsigned &uid);
	int GetMapping(const string &openid, unsigned serverid, unsigned &uid, unsigned &updatetime, unsigned &lastlogin);
	int RemoveMapping(const string &openid, int platform);
	int GetAllMapping(const string &openid, vector<unsigned> &uid);
	int GetAllRegisterTime(const string &openid, vector<unsigned> &registertime);
	int GetUID(const string &openid, unsigned serverid, unsigned &uid);
	int UpdateLastLogin(const string &openid, int platform);
	int SetLastLogin(const string &openid, unsigned serverid, unsigned lastlogin);
	int GetServer(const string &openid, vector<server> &servers);
	int SetTime(const string &openid, unsigned ts);
};

#endif /* DATAUSERMAPPING_H_ */
