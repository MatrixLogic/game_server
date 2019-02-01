/*
 * LogicBuyGoods.h
 *
 *  Created on: 2012-9-17
 *      Author: Administrator
 */

#ifndef LOGICBUYGOODS_H_
#define LOGICBUYGOODS_H_
#include "DataGoods.h"
class CLogicBuyGoods {
public:
	CLogicBuyGoods();
	virtual ~CLogicBuyGoods();
	int BuyGoods(DataGoods data);
	int UpdateGoods(int unsigned uid, uint64_t id, int unsigned count);
	int GetGoods(int unsigned uid,Json::Value &value);
	//int DeleteHistory();
};

#endif /* LOGICBUYGOODS_H_ */
