/*
 * DataNominateAlliance.h
 *
 * Created on: 2013-01-22
 * Author: Administrator
 */

#ifndef DATA_NOMINATE_ALLIANCE_H_
#define DATA_NOMINATE_ALLIANCE_H_

#include "Kernel.h"

#define MAX_NOMINATE_ALLIANCE_NUM 300

struct DataNominateAlliance
{
	unsigned alliance_id;
	int sign;
};

class CDataNominateAlliance{
public:
	CDataNominateAlliance();
	int Init(const std::string &path, semdat sem=sem_nominatealliance);
	int GetNominateAlliance(vector<DataNominateAlliance> &allianceList);
	int SetNominateAlliance(const DataNominateAlliance &data);
	int RemoveData(unsigned alliance_id);
private:
	bool m_init;
	CShareMemory m_sh;
};

#endif /* DATA_NOMINATE_ALLIANCE_H_ */
