/*
 * LogicBuyGoods.cpp
 *
 *  Created on: 2012-9-17
 *      Author: Administrator
 */

#include "LogicBuyGoods.h"

CLogicBuyGoods::CLogicBuyGoods() {
	// TODO Auto-generated constructor stub

}

CLogicBuyGoods::~CLogicBuyGoods() {
	// TODO Auto-generated destructor stub
}

int CLogicBuyGoods::BuyGoods(DataGoods data)
{
	CDataGoods datagoods;
	int ret  = datagoods.AddGoods(data);
	if(0 != ret)
	{
		return ret;
	}
	return 0;
}
int CLogicBuyGoods::UpdateGoods(int unsigned uid, uint64_t id, int unsigned count)
{
	CDataGoods datagoods;
	int ret  = datagoods.UpdateGoods(uid,id,count);
	if(0 != ret)
	{
		return ret;
	}
	return 0;
}
int CLogicBuyGoods::GetGoods(int unsigned uid,Json::Value &value)
{
	CDataGoods datagoods;
	vector<DataGoods> vdata;
	vdata.clear();
	int ret = datagoods.GetGoods(uid,vdata);
	Json::Reader reader;
	if(ret == R_ERR_NO_DATA)
	{
		return 0;
	}
	else
	{
		int iCount = vdata.size();
		for(int i = 0; i < iCount; i++)
		{
			reader.parse(vdata[i].data,value[i]);
			value[i]["count"] = vdata[i].count;
			value[i]["seq"] = Convert::UInt64ToString(vdata[i].id);
			value[i]["ts"] = Time::GetGlobalTime();
		}
	}
	return 0;

}
