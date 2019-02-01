/*
 * DataNeighbour.h
 *
 *  Created on: 2011-7-11
 *      Author: dada
 */

#ifndef DATANEIGHBOUR_H_
#define DATANEIGHBOUR_H_

#include "Kernel.h"

class CDataNeighbour : public CDataBaseDBC
{
public:
	CDataNeighbour(int table = DB_NEIGHBOUR) : CDataBaseDBC(table) {}

public:
	int AddNeighbours(unsigned uid, const string &neighbours);
	int SetNeighbours(unsigned uid, const string &neighbours);
	int GetNeighbours(unsigned uid, string &neighbours);
	int RemoveNeighbours(unsigned uid);

};

#endif /* DATANEIGHBOUR_H_ */
