/*
 * DataMap.h
 *
 *  Created on: 2011-6-21
 *      Author: dada
 */

#ifndef DATAMAP_H_
#define DATAMAP_H_

#include "Kernel.h"

class CDataMap
{
public:
	CDataMap();
	int Init(const string &path, semdat sem=sem_map);
	int SetUser(unsigned uid, int group, int type);
	int SetCount(unsigned uid, int count);
	int GetUser(int group, unsigned &uid, int type);

protected:
	CShareMemory m_sh;
	bool m_init;
};

#endif /* DATAMAP_H_ */
