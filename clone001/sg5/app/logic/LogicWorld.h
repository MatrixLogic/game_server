/*
 * LogicWorld.h
 *
 *  Created on: 2012-8-21
 *      Author: Administrator
 */

#ifndef LOGICWORLD_H_
#define LOGICWORLD_H_
#include "LogicInc.h"
class CLogicWorld {
public:
	CLogicWorld();
	virtual ~CLogicWorld();
	CDataWorld *GetDataWorld();
	int Move(const Json::Value &data,Json::Value &result);
	int AddUser( const WorldPoint  &worldpoint, int unsigned &world_pos);
	//int AddUser( const WorldPoint  &worldpoint, int unsigned &world_pos);
	int AddUser(const WorldPoint  &worldpoint,int unsigned &world_pos,short begin_x,short begin_y,short end_x,short end_y);
	int AddUser( const WorldPoint &worldpoint, int x,int y);
	int GetUserInfo(const int unsigned world_pos,WorldPoint &worldPoint);
	int ReplaceUser(const WorldPoint &worldpoint, int unsigned &world_pos);
	int ViewMap()
	{
		CDataWorld *pdata = GetDataWorld();
		return pdata->ViewMap();
	}
	int MoveCity(unsigned uid,unsigned oldWorldpos,unsigned toWorldpos);
};

#endif /* LOGICWORLD_H_ */
