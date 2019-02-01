/*
 * LogicNeighbour.h
 *
 *  Created on: 2011-6-9
 *      Author: dada
 */

#ifndef LOGICNEIGHBOUR_H_
#define LOGICNEIGHBOUR_H_

#include "LogicInc.h"

class CLogicNeighbour
{
public:

	int GetNeighbours(unsigned uid, UidList &neighbours);
	int SetNeighbours(unsigned uid, const UidList &neighbours);
	bool IsNeighbour(unsigned uid, unsigned nid);
	int GetUserInfoJson(const Json::Value &uidList, Json::Value &result);

protected:

	int AddNeighbour(unsigned uid, unsigned nid);
	int RemoveNeighbour(unsigned uid, unsigned nid);

};

#endif /* LOGICNEIGHBOUR_H_ */
