/*
 * DataGM.h
 *
 *  Created on: 2013-3-20
 *      Author: Administrator
 */

#ifndef DATAGM_H_
#define DATAGM_H_

#include "Kernel.h"
#include "DataInc.h"

#define GM_NUM 100

enum GMEnum
{
	gm_none = 0,
	gm_admin = 1,
	gm_th = 2,
};

struct GM
{
	char openid[64];
	unsigned flag;
};

struct DataGM {
	GM gm[GM_NUM];
};

class CDataGM {
public:
	CDataGM();
	int Init(const string &path, semdat sem=sem_gm);
	int Query(Json::Value &buff, unsigned flag);
	int Add(string& buff, unsigned flag);
	int Del(string& buff, unsigned flag);
	int Check(string& buff, unsigned &flag);

protected:
	CShareMemory m_sh;
	bool m_init;
};

#endif /* DATAGM_H_ */
