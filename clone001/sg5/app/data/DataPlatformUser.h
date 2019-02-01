/*
 * DataPlatformUser.h
 *
 *  Created on: 2011-12-14
 *      Author: dada
 */

#ifndef DATAPLATFORMUSER_H_
#define DATAPLATFORMUSER_H_

#include "Kernel.h"

class CDataPlatformUser : public CDataBaseDBC
{
public:
	CDataPlatformUser(int table = DB_PLATFORM_USER) : CDataBaseDBC(table) {}

public:
	int AddData(const string &openid, int platform, const string &data);
	int SetData(const string &openid, int platform, const string &data);
	int GetData(const string &openid, int platform, string &data);
	int RemoveData(const string &openid, int platform);

};

#endif /* DATAPLATFORMUSER_H_ */
