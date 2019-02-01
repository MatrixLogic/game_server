/*
 * DataInviteMapping.h
 *
 *  Created on: 2012-9-4
 *      Author: Administrator
 */
#include "Kernel.h"
#ifndef DATAINVITEMAPPING_H_
#define DATAINVITEMAPPING_H_

class CDataInviteMapping :public CDataBaseDBC{
public:
	CDataInviteMapping(int table = DB_INVITE_MAPPING):CDataBaseDBC(table){};
	int GetUid(unsigned uid, vector<unsigned> &vUid);
	int AddMapping(unsigned uid, unsigned invite_uid);
};

#endif /* DATAINVITEMAPPING_H_ */
