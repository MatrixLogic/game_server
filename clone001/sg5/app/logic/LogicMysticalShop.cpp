/*
 * LogicMysticalShop.cpp
 *
 *  Created on: 2012-12-26
 *      Author: colin
 */
#include "LogicMysticalShop.h"

CLogicMysticalShop::CLogicMysticalShop()
{

}

CLogicMysticalShop::~CLogicMysticalShop()
{

}

CDataMysticalShop* CLogicMysticalShop::GetCDataMysticalShop()
{
	GET_MEM_DATA_SEM(CDataMysticalShop,CONFIG_MYSTICALSHOP_PATH,sem_mysticalshop)
	/*static CDataMysticalShop* pdata = NULL;
	if (!pdata)
	{
		pdata = new CDataMysticalShop();
		if (0 != pdata->Init(Config::GetValue(CONFIG_MYSTICALSHOP_PATH)))
		{
			delete pdata;
			pdata = NULL;
		}
	}
	return pdata;*/
}

int CLogicMysticalShop::InitMysticalShop()
{
	CDataMysticalShop *pdata = GetCDataMysticalShop();
	if(pdata == NULL)
	{
		return -1;
	}
	int ret = pdata->InitMysticalShop();
	return ret;
}

int CLogicMysticalShop::GetTopEquipNum(DataEquipItem dataEquipItem[],  int &equipNum)
{
	CDataMysticalShop *pdata = GetCDataMysticalShop();
	if(pdata == NULL)
	{
		return -1;
	}
	int ret = pdata->GetTopEquipNum(dataEquipItem, equipNum);
	if(0 != ret)
	{
		return ret;
	}
	return ret;

}

int CLogicMysticalShop::PayMysticalEquip(unsigned uid, string name, unsigned eid)
{
	CDataMysticalShop *pdata = GetCDataMysticalShop();
	if(pdata == NULL)
	{
		return -1;
	}
	unsigned cost = 0;
	int ret = pdata->PayMysticalEquip(uid, name, eid, cost);
	info_log("uid = %u,buying MysticalEquip: eid = %d,cost = %d",uid,eid,cost);
	if(0 == ret)
	{
		CLogicPay logicPay;
		ret = logicPay.ChangePay(uid ,-cost, 0, "MysticalShop", 1);
		if (ret != 0)
		{
			info_log("uid = %u,buying MysticalEquip cost fail!: eid = %d,cost = %d",uid,eid,cost);
			int tmpRet = pdata->AddOneNumForSell(eid);
			return ret;
		}
		else
		{
			info_log("uid = %u,bought MysticalEquip: eid = %d,cost = %d",uid,eid,cost);
			int tmpRet = pdata->AddSelledEquip(uid, name, eid, cost);
			return ret;
		}
	}
	return ret;
}

int CLogicMysticalShop::GetInfobuys(DataEquipSells dataEquipSells[], int &buyNums)
{
	CDataMysticalShop *pdata = GetCDataMysticalShop();
	if(pdata == NULL)
	{
		return -1;
	}
	int ret = pdata->GetInfobuys(dataEquipSells, buyNums);
	if(0 != ret)
	{
		return ret;
	}
	return ret;

}

