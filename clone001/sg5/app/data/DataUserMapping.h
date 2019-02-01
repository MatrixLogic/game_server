/*
 * DataUserMapping.h
 *
 *  Created on: 2011-7-8
 *      Author: dada
 */

#ifndef DATAUSERMAPPING_H_
#define DATAUSERMAPPING_H_

#include "Kernel.h"

class CDataUserMapping : public CDataBaseDBC
{
public:
	CDataUserMapping(int table = DB_USER_MAPPING) : CDataBaseDBC(table) {}

public:
	int AddMapping(const string &openid, int platform, unsigned uid);
	int GetMapping(const string &openid, int platform, unsigned &uid);
	int RemoveMapping(const string &openid, int platform);
	int GetAllMapping(const string &openid, vector<unsigned> &uid);
	int GetUID(const string &openid, unsigned serverid, unsigned &uid);
};

#endif /* DATAUSERMAPPING_H_ */
