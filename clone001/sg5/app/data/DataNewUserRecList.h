/*
 * DataNewUserRecList.h
 *
 *  Created on: 2013-8-9
 *      Author: Administrator
 */

#ifndef DATANEWUSERRECLIST_H_
#define DATANEWUSERRECLIST_H_

#include "Kernel.h"

#define MAX_REC_LIST_COUNT 100

struct DataNewUserRecList
{
	unsigned list[MAX_REC_LIST_COUNT];
};

class CDataNewUserRecList {
public:
	CDataNewUserRecList();
	int Init(const string &path, semdat sem=sem_new_user_rec);
	int GetRecList(unsigned *list, const int count);
	int SetRecList(unsigned uid);

protected:
	CShareMemory m_sh;
	bool m_init;
};

#endif /* DATANEWUSERRECLIST_H_ */
