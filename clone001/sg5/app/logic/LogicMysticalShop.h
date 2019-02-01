/*
 * LogicMysticalShop.h
 *
 *  Created on: 2012-12-26
 *      Author: colin
 */

#ifndef LOGICMYSTICALSHOP_H_
#define LOGICMYSTICALSHOP_H_
#include "LogicInc.h"
typedef pair<unsigned, unsigned> U_U_MAP;

class CLogicMysticalShop
{
public:
	CLogicMysticalShop();
	virtual ~CLogicMysticalShop();
	int InitMysticalShop();
	int GetTopEquipNum(DataEquipItem dataEquipItem[], int &equipNum);
	int PayMysticalEquip(unsigned uid, string name, unsigned eid);
	int GetInfobuys(DataEquipSells dataEquipSells[], int &buyNums);

protected:
	CDataMysticalShop *GetCDataMysticalShop();
};


#endif /* LOGICMYSTICALSHOP_H_ */
