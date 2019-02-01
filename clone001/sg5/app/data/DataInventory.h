#ifndef DATAINVENTORY_H_
#define DATAINVENTORY_H_

#include "Kernel.h"

class CDataInventory : public CDataBaseDBC
{
public:
	CDataInventory(int table = DB_INVENTORY) : CDataBaseDBC(table) {}

public:
	int AddItem(unsigned uid, const string &type, unsigned count);
	int SetItem(unsigned uid, const string &type, unsigned count);
	int GetItem(unsigned uid, const string &type, unsigned &count);
	int GetItems(unsigned uid, map<string, unsigned> &items);
	int RemoveItem(unsigned uid, const string &type);
	int RemoveItems(unsigned uid);
};

#endif /* DATAINVENTORY_H_ */
