/*
 * DataAllianceMapping.h
 *
 *  Created on: 2011-9-29
 *      Author: dada
 */

#ifndef DATAALLIANCEMAPPING_H_
#define DATAALLIANCEMAPPING_H_

#include "Kernel.h"

class CDataAllianceMapping : public CDataBaseDBC
{
public:
	CDataAllianceMapping(int table = DB_ALLIANCE_MAPPING) : CDataBaseDBC(table) {}

public:
	int AddMapping(const string &alliance_name, unsigned alliance_id);
	int GetMapping(const string &alliance_name, unsigned &alliance_id);
	int RemoveMapping(const string &alliance_name);
};
#endif /* DATAALLIANCEMAPPING_H_ */
