/*
 * DataMysticalShop.h
 *
 *  Created on: 2012-12-26
 *      Author: colin
 */

#ifndef DATAMYSTICALSHOP_H_
#define DATAMYSTICALSHOP_H_
#include "Kernel.h"


#define MAX_EQUIPS_NUM 1000
#define SELL_EQUIPS_NUM 4

struct DataEquipItem
{
	unsigned eid;
	unsigned ratio;
	unsigned nums;
	unsigned price;
	unsigned ratioRank;
	unsigned min;
	unsigned max;
	DataEquipItem &operator= (DataEquipItem const& other)
	{
		eid = other.eid;
		ratio = other.ratio;
		nums = other.nums;
		price = other.price;
		ratioRank = other.ratioRank;
		min = other.min;
		max = other.max;
		return *this;
	}
};
struct DataEquipSells
{
	//int stat;
	int uid;
	char name[64];
	unsigned buy_time;
	DataEquipItem buy_Equip;
	DataEquipSells &operator= (DataEquipSells const& other)
	{
		uid = other.uid;
		memcpy(name, other.name, sizeof(name)-1);
		buy_time = other.buy_time;
		buy_Equip = other.buy_Equip;
		return *this;
	}
};
struct DataEquipShop
{
	unsigned count;
	unsigned sellsCount;
	unsigned range_time;
	DataEquipItem equipAll[MAX_EQUIPS_NUM];
	DataEquipItem equipForSell[SELL_EQUIPS_NUM];
	DataEquipSells sells[MAX_EQUIPS_NUM];
};

class CDataMysticalShop
{
public:
	CDataMysticalShop();
	virtual ~CDataMysticalShop();
	int Init(const std::string &path, semdat sem=sem_mysticalshop);
	int InitMysticalShop();
	int UpdateEquipRank();
	int GetTopEquipNum(DataEquipItem dataEquipItem[],  int &equipNum);
	int PayMysticalEquip(unsigned uid, string name, unsigned eid, unsigned& cost);
	DataEquipShop* GetMysticalShopPdata();
	int GetInfobuys(DataEquipSells dataEquipSells[], int &buyNums);
	int AddEquip(const unsigned eid, const unsigned nums, const unsigned min, const unsigned max, const unsigned ratio);
	int DeleteEquip(const unsigned eid);
	int QueryEquips(vector<DataEquipItem>& vecDataEquips);
	int DeleteSelledEquip(const unsigned eid);
	int AddSelledEquip(unsigned uid, string name, unsigned eid, unsigned cost);
	int AddOneNumForSell(const unsigned eid);
protected:
	bool m_init;
	CShareMemory m_sh;


};

#endif /* DATAMYSTICALSHOP_H_ */
