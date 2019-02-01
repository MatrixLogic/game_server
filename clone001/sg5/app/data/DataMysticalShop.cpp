/*
 * DataMysticalShop.cpp
 *
 *  Created on: 2012-12-26
 *      Author: colin
 */

#include "DataMysticalShop.h"


CDataMysticalShop::CDataMysticalShop()
{
	m_init = false;
}

CDataMysticalShop::~CDataMysticalShop()
{

}

int CDataMysticalShop::Init(const std::string &path, semdat sem)
{
	if(m_init)
	{
		return 0;
	}

	if (path.empty())
	{
		error_log("[init_MysticalShop_fail][path=%s]", path.c_str());
		return R_ERROR;
	}

	int semgroup = 0;
	int semserver = 0;
	Config::GetDomain(semserver);
	if(!semserver)
		Config::GetDB(semserver);
	if(!m_sh.CreateOrOpen(path.c_str(), sizeof(DataEquipShop), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init__MysticalShop_data_fail][path=%s]", path.c_str());
		return -1;
	}
	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{
		DataEquipShop *pdata = (DataEquipShop *)m_sh.GetAddress();
		if(pdata == NULL)
		{
			DB_ERROR_RETURN_MSG("init_MysticalShop_fail");
		}
		memset(pdata, 0x00, sizeof(*pdata));
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}

int sortEquip(const DataEquipItem& left,const DataEquipItem& right)
{
	return left.ratioRank > right.ratioRank;
}

int CalcEquipRatio(const vector<DataEquipItem>& vEquipRank, map<int,DataEquipItem>& equipSells)
{
	map<int,DataEquipItem> equipMaps;
	int idx = 0;
	equipSells.clear();
	for(vector<DataEquipItem>::const_iterator it = vEquipRank.begin(); it != vEquipRank.end(); ++it)
	{
		if (it->ratio == 100)
		{
			equipSells[it->eid] = *it;
			if(equipSells.size() >= SELL_EQUIPS_NUM)
			{
				break;
			}
			continue;
		}
		for(int i=0;i<it->ratio;++i)
		{
			equipMaps[idx++] = *it;
		}
	}
	for(int i = 100;i>0;--i)
	{
		if(equipSells.size() >= SELL_EQUIPS_NUM)
		{
			break;
		}
		int pos = rand()%equipMaps.size();
		equipSells[equipMaps[pos].eid] = equipMaps[pos];
	}
	if(equipSells.size() < SELL_EQUIPS_NUM)
	{
		for(vector<DataEquipItem>::const_iterator it = vEquipRank.begin();it != vEquipRank.end();++it)
		{
			equipSells[it->eid] = *it;
			if(equipSells.size() >= SELL_EQUIPS_NUM)
			{
				break;
			}
		}
	}
	return 0;
}

/*
int CalcEquipRatio(vector<DataEquipItem>& vEquipRank, int nums)
{
	int sumRatio = 0;
	for (vector<DataEquipItem>::iterator myItr = vEquipRank.begin(); myItr != vEquipRank.end(); myItr++)
	{
		sumRatio += myItr->ratio;
	}
	if (sumRatio != 0)
	{
		for (vector<DataEquipItem>::iterator myItr = vEquipRank.begin(); myItr != vEquipRank.end(); myItr++)
		{

			int randI = rand() / sumRatio;
			myItr->ratioRank = myItr->ratio * randI;
		}
	}
	else
	{
		return -1;
	}
	return 0;
}
*/

int SetEquipItemPrice(unsigned min, unsigned max)
{
	if(min > max)
	{
		return max;
	}
	int i = rand()%(max - min + 1);
	return min + i;
}

int CalcStartTime()
{
	time_t now;
	time(&now);
	int tempNow = now;
	struct tm *pTm = localtime(&now);
	int tempSec = pTm->tm_sec;
	int tempMin = pTm->tm_min;
	int tempHour = pTm->tm_hour;
	int startTime = tempNow - tempSec - tempMin * 60 - tempHour *3600;
	return startTime;
}

int CDataMysticalShop::InitMysticalShop()
{
	DataEquipShop *pdata = (DataEquipShop *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_mysticalshop_fail");
	}

	string rangeTime = Config::GetValue(CONFIG_MYSTICALSHOP_RANGE_TS);
	Json::Value value;
	Json::Reader().parse(rangeTime, value);

	unsigned tempNow = time(NULL);
	unsigned startTime = CalcStartTime();
	tempNow -= startTime;


	for(unsigned int i=0;i<value.size();++i)
	{
		if(tempNow >= value[i][0u].asUInt() && tempNow <= value[i][1u].asUInt()){

			if(pdata->range_time - startTime >= value[i][0u].asUInt() && pdata->range_time - startTime <= value[i][1u].asUInt()){
				return 0;
			}

			UpdateEquipRank();
			if(pdata->count > 0){
				pdata->range_time = tempNow + startTime;
			}
			break;
		}
	}

	return 0;
}

bool JudgeTime(unsigned mytime)
{
	bool tempFlag = false;
//	if (mytime > 1360252800 && mytime < 1361203200)
//	{
//		string rangeTime = Config::GetValue(CONFIG_MYSTICALSHOP_RANGE_TS);
//		Json::Value value;
//		Json::Reader().parse(rangeTime, value);
//		unsigned startTime = CalcStartTime();
//		unsigned tempNow = mytime - startTime;
//		for(unsigned int i=0;i<value.size();++i)
//		{
//			if(tempNow >= value[i][0u].asUInt() && tempNow <= value[i][1u].asUInt())
//			{
//				tempFlag = true;
//				break;
//			}
//		}
//	}
	string rangeTime = Config::GetValue(CONFIG_MYSTICALSHOP_RANGE_TS);
	Json::Value value;
	Json::Reader().parse(rangeTime, value);
	unsigned startTime = CalcStartTime();
	unsigned tempNow = mytime - startTime;
	for(unsigned int i=0;i<value.size();++i)
	{
		if(tempNow >= value[i][0u].asUInt() && tempNow <= value[i][1u].asUInt())
		{
			tempFlag = true;
			break;
		}
	}
	return tempFlag;
}

int CDataMysticalShop::UpdateEquipRank()
{
	DataEquipShop *pdata = (DataEquipShop *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_mysticalshop_fail");
	}
	DataEquipItem dataEquipItem[MAX_EQUIPS_NUM];
	memset(&dataEquipItem,0x00,sizeof(DataEquipItem));
	int equipNumAll = 0;
	int i = 0;
	equipNumAll = pdata->count;
	vector<DataEquipItem> vEquipRank;
	map<int,DataEquipItem> mEquipSelect;

	if(pdata->count == 0){
		return 0;
	}

	for(i = 0; i < pdata->count; i++)
	{
		vEquipRank.push_back(pdata->equipAll[i]);
	}

	int ret = CalcEquipRatio(vEquipRank, mEquipSelect);

	i = 0;
	for (map<int,DataEquipItem>::iterator myItr = mEquipSelect.begin(); myItr != mEquipSelect.end(); myItr++)
	{
		pdata->equipForSell[i++] = myItr->second;
	}

	/*
	// 计算ratioRank
	int ret = CalcEquipRatio(vEquipRank, equipNumAll);

	// 排序
	sort(vEquipRank.begin(),vEquipRank.end(),sortEquip);

	for(i = 0 ;i < pdata->count; i++)
	{
		pdata->equipAll[i] = vEquipRank[i];
	}

	//选择top3 商品
	for(i = 0; i < SELL_EQUIPS_NUM; i++)
	{
		pdata->equipForSell[i] = pdata->equipAll[i];
	}
	*/

	// 确定价格
	for(i = 0; i < SELL_EQUIPS_NUM; i++)
	{
		pdata->equipForSell[i].price = SetEquipItemPrice(pdata->equipForSell[i].min, pdata->equipForSell[i].max);
	}

	pdata->sellsCount = 0;
	memset(pdata->sells,0,sizeof(pdata->sells));

	return 0;
}

int CDataMysticalShop::GetTopEquipNum(DataEquipItem dataEquipItem[],  int &equipNum)
{
	DataEquipShop *pdata = (DataEquipShop *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_mysticalshop_fail");
	}
	CAutoLock lock(&m_sh, true);
	memcpy(dataEquipItem, pdata->equipForSell, sizeof(DataEquipItem) * SELL_EQUIPS_NUM);
	equipNum = SELL_EQUIPS_NUM;
	return 0;
}

int CDataMysticalShop::PayMysticalEquip(unsigned uid, string name, unsigned eid,unsigned& cost)
{
	DataEquipShop *pdata = (DataEquipShop *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_mysticalshop_fail");
	}

	unsigned tempNow = time(NULL);
	bool flag = JudgeTime(tempNow);
	if (flag == false)
	{
		DB_ERROR_RETURN_MSG("buy_time_range error!");
	}

	CAutoLock lock(&m_sh, true);
	int i;
	for(i = 0; i < SELL_EQUIPS_NUM; i++)
	{
		if (pdata->equipForSell[i].eid == eid)
		{
			if(pdata->equipForSell[i].nums == 0){
				DB_ERROR_RETURN_MSG("mysticalshop_no_goods");
			}

			cost = pdata->equipForSell[i].price;
			pdata->equipForSell[i].nums--;
			break;
		}
	}
	if(i >= SELL_EQUIPS_NUM){
		info_log("uid = %u, MysticalEquip no_such_equip: eid = %d; right eid: %d, %d, %d",uid, eid, pdata->equipForSell[0].eid, pdata->equipForSell[1].eid, pdata->equipForSell[2].eid);
		DB_ERROR_RETURN_MSG("no_such_equip");
	}
	return 0;
}

DataEquipShop* CDataMysticalShop::GetMysticalShopPdata()
{
	DataEquipShop *pdata = (DataEquipShop *)m_sh.GetAddress();
	return pdata;
}

int CDataMysticalShop::GetInfobuys(DataEquipSells dataEquipSells[], int &buyNums)
{
	DataEquipShop *pdata = (DataEquipShop *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_mysticalshop_fail");
	}
	int i;
	buyNums = pdata->sellsCount;
	//pdata->sells
	for (i=0; i< buyNums; i++)
	{
		dataEquipSells[i] = pdata->sells[i];
	}
	return 0;

}

int CDataMysticalShop::AddEquip(const unsigned eid, const unsigned nums, const unsigned min, const unsigned max, const unsigned ratio)
{
	DataEquipShop *pdata = (DataEquipShop *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_mysticalshop_fail");
	}
	vector<DataEquipItem> vecDataEquips;
	vecDataEquips.clear();
	int ret = QueryEquips(vecDataEquips);
	vector<DataEquipItem>::iterator itr = vecDataEquips.begin();
	for( ; itr != vecDataEquips.end(); itr++)
	{
		if(itr->eid == eid)
		{
			DB_ERROR_RETURN_MSG("it is not allowed to insert the same element");
			break;
		}
	}
	CAutoLock lock(&m_sh, true);
	int equipNums = pdata->count;
	if (equipNums >= 1000)
	{
		DB_ERROR_RETURN_MSG ("exceed the max nums");
	}
	pdata->equipAll[equipNums].eid = eid;
	pdata->equipAll[equipNums].nums = nums;
	pdata->equipAll[equipNums].min = min;
	pdata->equipAll[equipNums].max = max;
	pdata->equipAll[equipNums].ratio = ratio;
	pdata->count++;
	return 0;
}

int CDataMysticalShop::DeleteEquip(const unsigned eid)
{
	DataEquipShop *pdata = (DataEquipShop *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_mysticalshop_fail");
	}
	CAutoLock lock(&m_sh, true);
	int count = pdata->count;
	vector<DataEquipItem> vEquip;
	for (int i = 0; i < count; i++)
	{
		vEquip.push_back(pdata->equipAll[i]);
	}
	bool flag = false;
	vector<DataEquipItem>::iterator itr = vEquip.begin();
	for( ; itr != vEquip.end(); itr++)
	{
		if(itr->eid == eid)
		{
			vEquip.erase(itr);
			if (itr == vEquip.end())
			{
				flag = true;
			}
			break;
		}
	}
	if (!flag && itr == vEquip.end())
	{
		DB_ERROR_RETURN_MSG("No goods in DataEquipItem!");
	}
	pdata->count--;
	memset(pdata->equipAll,0,sizeof(pdata->equipAll));
	for (int i = 0; i < count-1 ; i++)
	{
		pdata->equipAll[i] = vEquip[i];
	}
	return 0;
}

int CDataMysticalShop::QueryEquips(vector<DataEquipItem>& vecDataEquips)
{
	DataEquipShop *pdata = (DataEquipShop *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_mysticalshop_fail");
	}
	vecDataEquips.clear();
	CAutoLock lock(&m_sh, true);
	int count = pdata->count;
	for (int i = count -1; i >= 0; --i)
	{
		vecDataEquips.push_back(pdata->equipAll[i]);
	}
	return 0;
}

int CDataMysticalShop::DeleteSelledEquip(const unsigned eid)
{
	DataEquipShop *pdata = (DataEquipShop *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_mysticalshop_fail");
	}
	CAutoLock lock(&m_sh, true);
	int count = pdata->sellsCount;
	vector<DataEquipSells> vEquip;
	for (int i = 0; i < count; i++)
	{
		vEquip.push_back(pdata->sells[i]);
	}
	bool flag = false;
	vector<DataEquipSells>::iterator itr = vEquip.begin();
	for( ; itr != vEquip.end(); itr++)
	{
		if(itr->buy_Equip.eid == eid) //pdata->sells[numSell].buy_Equip.eid = eid;
		{
			vEquip.erase(itr);
			if (itr == vEquip.end())
			{
				flag = true;
			}
			break;
		}
	}
	if (!flag && itr == vEquip.end())
	{
		DB_ERROR_RETURN_MSG("No goods in DataEquipItem!");
	}
	pdata->sellsCount--;
	memset(pdata->sells,0,sizeof(pdata->sells));
	for (int i = 0; i < count-1 ; i++)
	{
		pdata->sells[i] = vEquip[i];
	}
	return 0;
}

int CDataMysticalShop::AddSelledEquip(unsigned uid, string name, unsigned eid, unsigned cost)
{
	DataEquipShop *pdata = (DataEquipShop *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_mysticalshop_fail");
	}
	CAutoLock lock(&m_sh, true);
	int numSell = pdata->sellsCount;
	if (numSell >= MAX_EQUIPS_NUM)
	{
		// 后续完善
		numSell = 0;
		pdata->sellsCount = 0;
	}
	pdata->sells[numSell].uid = uid;
	snprintf(pdata->sells[numSell].name,sizeof(pdata->sells[numSell].name)-1,"%s",name.c_str());
	pdata->sells[numSell].buy_time = time(NULL);
	pdata->sells[numSell].buy_Equip.eid = eid;
	pdata->sells[numSell].buy_Equip.price = cost;
	pdata->sellsCount++;
	return 0;
}

int CDataMysticalShop::AddOneNumForSell(unsigned eid)
{
	DataEquipShop *pdata = (DataEquipShop *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_mysticalshop_fail");
	}
	CAutoLock lock(&m_sh, true);
	int i;
	for(i = 0; i < SELL_EQUIPS_NUM; i++)
	{
		if (pdata->equipForSell[i].eid == eid)
		{
			pdata->equipForSell[i].nums++;
			break;
		}
	}

	if(i >= SELL_EQUIPS_NUM){
		DB_ERROR_RETURN_MSG("no_such_equip");
	}
	return 0;
}
