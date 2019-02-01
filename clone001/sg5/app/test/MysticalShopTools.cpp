/*
 * MysticalShopTools.cpp
 *
 *  Created on: 2012-12-27
 *      Author: Administrator
 */
#include "LogicInc.h"
#include <iostream>

void usage()
{
	cout << "usage: MysticalShopTool eid nums min max ratio" << endl
			<< " MysticalShopTool del" << endl
			<< " MysticalShopTool init" << endl
			<< " MysticalShopTool watch" << endl
			<< " MysticalShopTool change" << endl
			<< " MysticalShopTool pickUp" << endl
			<< " MysticalShopTool delAll" << endl;
}
void dump(DataEquipShop* pdata)
{
	int count = pdata->count;
	for (int i = 0; i < count; i++)
	{
		cout << pdata->equipAll[i].eid <<","
			<< pdata->equipAll[i].nums <<","
			<< pdata->equipAll[i].ratio <<","
			<< pdata->equipAll[i].price <<","
			<< pdata->equipAll[i].min <<","
			<< pdata->equipAll[i].max <<","
			<< pdata->equipAll[i].ratioRank << endl;

	}
	cout << "------------------------------" << endl;
	for (int i = 0; i < SELL_EQUIPS_NUM; i++)
	{
		cout << pdata->equipForSell[i].eid <<","
		<< pdata->equipForSell[i].nums <<","
		<< pdata->equipForSell[i].ratioRank <<","
		<< pdata->equipForSell[i].price <<","
		<< pdata->equipForSell[i].ratio <<","
		<< endl;
	}
	cout << "------------------------------" << endl;
	cout << "time: " << pdata->range_time << endl;

}

int main(int argc, char *argv[])
{

	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "init kernel fail" << endl;
		return 1;
	}
	Config::SetDomain(0);

	if (argc != 6 && string("watch") != argv[1] && string("init") != argv[1] && string("del") != argv[1] && string("pickUp") != argv[1] && string("delSelledId") != argv[1] && string("delAll") != argv[1])
	{
		usage();
		return 0;
	}
	int ret = 0;
	CDataMysticalShop MysticalShop;
	ret = MysticalShop.Init(Config::GetValue(CONFIG_MYSTICALSHOP_PATH));
	if (ret != 0)
	{
		cout << "init mysticalshop fail,ret:" << ret << endl;
		return 0;
	}
	DataEquipShop *pdata = NULL;
	pdata = MysticalShop.GetMysticalShopPdata();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_mysticalshop_fail");
	}

	if(string("watch") == argv[1])
	{
		//dump(pdata);
		int count = pdata->count;
		vector<DataEquipItem> vecDataEquips;
		vecDataEquips.clear();
		ret = MysticalShop.QueryEquips(vecDataEquips);
		if (ret != 0)
		{
			cout << "get equips fail!" << endl;
		}
		for (int i = 0; i < count; ++i)
		{
			cout << vecDataEquips[i].eid <<","
				<< vecDataEquips[i].nums <<","
				<< vecDataEquips[i].ratio <<","
				<< vecDataEquips[i].price <<","
				<< vecDataEquips[i].min <<","
				<< vecDataEquips[i].max <<","
				<< vecDataEquips[i].ratioRank << endl;
		}
		cout << "------------------------------" << endl;
		for (int i = 0; i < SELL_EQUIPS_NUM; i++)
		{
			cout << pdata->equipForSell[i].eid <<","
			<< pdata->equipForSell[i].nums <<","
			<< pdata->equipForSell[i].ratioRank <<","
			<< pdata->equipForSell[i].price <<","
			<< pdata->equipForSell[i].ratio <<","
			<< endl;
		}
		cout << "------------------------------" << endl;
		cout << "time: " << pdata->range_time << endl;

		return 0;
	}

	if(string("init") == argv[1])
	{
		MysticalShop.InitMysticalShop();
		return 0;
	}

	if(string("del") == argv[1])
	{
		for(int i = 0; i < pdata->count; i++)
		{
			cout << pdata->equipAll[i].eid << endl;
		}
		cout << "-----------------------------" << endl;
		cout << "please input eid:"<< endl;
		int eid;
		cin >> eid;
		//int ret = del(pdata, eid);

		MysticalShop.DeleteEquip(eid);

		if (ret == 0)
		{
			cout << "删除成功" << endl;
		}
		cout << "-----------------------------" << endl;
		for(int i = 0; i < pdata->count; i++)
		{
			cout << pdata->equipAll[i].eid << endl;
		}

		return 0;
	}

	if(string("change") == argv[1])
	{
		for(int i = 0; i < pdata->count; i++)
		{
			cout << pdata->equipAll[i].eid << endl;
		}
		cout << "-----------------------------" << endl;
		cout << "please input eid:"<< endl;
		int eid;
		cin >> eid;
		//int ret = (pdata, eid);
		if (ret == 0)
		{
			cout << "删除成功" << endl;
		}
		cout << "-----------------------------" << endl;
		for(int i = 0; i < pdata->count; i++)
		{
			cout << pdata->equipAll[i].eid << endl;
		}

		return 0;
	}
	if(string("delAll") == argv[1])
	{
		memset(pdata->equipAll,0,sizeof(pdata->equipAll));
		pdata->count = 0;
		return 0;
	}

	if(string("pickUp") == argv[1])
	{
		for (int i = 0; i< 100; i++)
		{
			MysticalShop.UpdateEquipRank();
			for (int i = 0; i < SELL_EQUIPS_NUM; i++)
			{
				info_log("TestMysticalEquip: eid=%u,ratio= %u", pdata->equipForSell[i].eid, pdata->equipForSell[i].ratio);
//				cout << pdata->equipForSell[i].eid <<","
//				<< pdata->equipForSell[i].nums <<","
//				<< pdata->equipForSell[i].ratioRank <<","
//				<< pdata->equipForSell[i].price <<","
//				<< endl;
			}


		}


//		for (int i = 0; i < SELL_EQUIPS_NUM; i++)
//		{
//			cout << pdata->equipForSell[i].eid <<","
//			<< pdata->equipForSell[i].nums <<","
//			<< pdata->equipForSell[i].ratioRank <<","
//			<< pdata->equipForSell[i].price <<","
//			<< endl;
//		}
		return 0;
	}

	if(string("delSelledId") == argv[1])
	{
		for(int i = 0; i < pdata->sellsCount; i++)
		{
			cout << pdata->sells[i].buy_Equip.eid << endl;
		}
		cout << "-----------------------------" << endl;
		cout << "please input eid:"<< endl;
		int eid;
		cin >> eid;
		int ret = MysticalShop.DeleteSelledEquip(eid);
		if (ret == 0)
		{
			cout << "删除成功" << endl;
		}
		cout << "-----------------------------" << endl;
		for(int i = 0; i < pdata->sellsCount; i++)
		{
			cout << pdata->sells[i].buy_Equip.eid << endl;
		}

		return 0;
	}



	int eid = atoi(argv[1]);
	int num = atoi(argv[2]);
	int min = atoi(argv[3]);
	int max = atoi(argv[4]);
	int ratio = atoi(argv[5]);
	int equipNums = pdata->count;
	ret = MysticalShop.AddEquip(eid, num, min, max, ratio);
	if (ret == 0)
	{
		cout << "添加成功！" << endl;
	}



//	if (equipNums == 0)
//	{
//		pdata->equipAll[0].eid = eid;
//		pdata->equipAll[0].nums = num;
//		pdata->equipAll[0].min = min;
//		pdata->equipAll[0].max = max;
//		pdata->equipAll[0].ratio = ratio;
//		pdata->count++;
//	}
//	else
//	{
//		pdata->equipAll[equipNums].eid = eid;
//		pdata->equipAll[equipNums].nums = num;
//		pdata->equipAll[equipNums].min = min;
//		pdata->equipAll[equipNums].max = max;
//		pdata->equipAll[equipNums].ratio = ratio;
//		pdata->count++;
//	}




	return 0;
}

