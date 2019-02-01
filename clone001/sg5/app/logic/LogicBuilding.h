#ifndef __LOGIC_BUILDING_H__
#define __LOGIC_BUILDING_H__

#include "LogicInc.h"

class CLogicBuilding
{
public:
	int InitializeBuilding(unsigned uid);
	int InitializeBuilding(unsigned uid,unsigned worldpos);

	int GetBuilding(unsigned uid, Json::Value &data);
	int GetBuilding(unsigned uid, unsigned worldpos,Json::Value &data,bool isMainBase);

	int UpdateBuilding(unsigned uid, unsigned worldpos,const Json::Value &data,bool isMainBase);
	int GetBuilding(unsigned uid, unsigned id, Json::Value &data);
	int GetBuilding(unsigned uid, unsigned id, unsigned worldpos,Json::Value &data,bool isMainBase);
	int RemoveBuilding(unsigned uid, unsigned worldpos);
	int ImportBuilding(unsigned uid, unsigned worldpos,const Json::Value &data, bool isMainBase);


};

#endif
