/*
 * DataGoods.h
 *
 *  Created on: 2012-9-17
 *      Author: Administrator
 */

#ifndef DATAGoods_H_
#define DATAGoods_H_


#include "Kernel.h"

struct DataGoods{
	int unsigned uid;
	uint64_t id;
	int unsigned count;
	int unsigned buy_time;
	string data;

};
class CDataGoods:public CDataBaseDBC{
public:
	CDataGoods(int tableId = DB_GOODS):CDataBaseDBC(tableId){}
	virtual ~CDataGoods();
	int AddGoods(DataGoods data);
	//int RemoveGoods(int unsigned uid,int unsigned ts);
	int UpdateGoods(int unsigned uid, uint64_t id,int unsigned count);
	int GetCountById(int unsigned uid, uint64_t id, int &count);
	int GetGoods(int unsigned uid, vector<DataGoods> &vdata);
};

#endif /* DATAGoods_H_ */
