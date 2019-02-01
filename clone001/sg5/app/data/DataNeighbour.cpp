/*
 * NeighboursNeighbour.cpp
 *
 *  Created on: 2011-7-11
 *      Author: dada
 */

#include "DataNeighbour.h"

int CDataNeighbour::AddNeighbours(unsigned uid, const string &neighbours)
{
	DBCREQ_DECLARE(DBC::InsertRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_STR_V(neighbours);
	return 0;
}
int CDataNeighbour::SetNeighbours(unsigned uid, const string &neighbours)
{
	DBCREQ_DECLARE(DBC::UpdateRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_SET_STR_V(neighbours);
	return 0;
}
int CDataNeighbour::GetNeighbours(unsigned uid, string &neighbours)
{
	DBCREQ_DECLARE(DBC::GetRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_GET_STR_V(neighbours);
	return 0;
}

int CDataNeighbour::RemoveNeighbours(unsigned uid)
{
	DBCREQ_DECLARE(DBC::DeleteRequest, uid);
	DBCREQ_SET_KEY(uid);
	DBCREQ_EXEC;
	return 0;
}

