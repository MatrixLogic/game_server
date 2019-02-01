#ifndef __LOGIC_EQUIPMENT_H__
#define __LOGIC_EQUIPMENT_H__

#include "LogicInc.h"

class CLogicEquipment
{
public:
	int InitializeEquipment(unsigned uid);
	int GetEquipment(unsigned uid, Json::Value &data, bool sync = false);
	int UpdateEquipment(unsigned uid, const Json::Value &data);
	int GetEquipmentUsed(unsigned uid, Json::Value &data);
};

#endif
