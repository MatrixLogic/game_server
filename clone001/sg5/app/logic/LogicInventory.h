#ifndef LOGICINVENTORY_H_
#define LOGICINVENTORY_H_

#include "LogicInc.h"

class CLogicInventory
{
public:
	int ChangeItem(unsigned uid, const string &type, int count);
	int ChangeItemsJson(unsigned uid, const Json::Value &data);
	int GetItems(unsigned uid, map<string, unsigned> &items);
	int GetItemsJson(unsigned uid, Json::Value &data);
	int GetItem(unsigned uid, const string &type, unsigned& count);
	int RemoveAllItem(unsigned uid);
};


#endif /* LOGICINVENTORY_H_ */
