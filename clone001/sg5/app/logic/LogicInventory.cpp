#include "LogicInventory.h"

int CLogicInventory::ChangeItem(unsigned uid, const string &type, int count)
{
	int ret;
	CDataInventory dbInventory;
	if(count == 0)
	{
		return 0;
	}
	unsigned oldCount;
	ret = dbInventory.GetItem(uid, type, oldCount);
	if(ret == 0)
	{
		if(count < 0 && oldCount < (unsigned)-count)
		{
			error_log("[not_enough_inventory][uid=%u,type=%s,old=%u,change=%d]", uid, type.c_str(), oldCount, count);
			LOGIC_ERROR_RETURN_MSG("not_enough_inventory");
		}
		oldCount += count;
		if(oldCount == 0)
		{
			ret = dbInventory.RemoveItem(uid, type);
			if(ret != 0)
			{
				error_log("[RemoveItem fail][uid=%u,type=%s,oldCount=%u,count=%d]", uid, type.c_str(), oldCount, count);
				DB_ERROR_RETURN_MSG("remove_inventory_fail");
			}
		}
		else
		{
			ret = dbInventory.SetItem(uid, type, oldCount);
			if(ret != 0)
			{
				error_log("[SetItem fail][uid=%u,type=%s,count=%u]", uid, type.c_str(), oldCount);
				DB_ERROR_RETURN_MSG("set_inventory_fail");
			}
		}
		info_log("[inventory][uid=%u&item=%s&delta=%d&count=%u]",  uid, type.c_str(), count, oldCount);
	}
	else if(ret == R_ERR_NO_DATA)
	{
		if(count < 0)
		{
			error_log("[not_enough_inventory][uid=%u,type=%s,old=null,change=%d]", uid, type.c_str(), count);
			LOGIC_ERROR_RETURN_MSG("not_enough_inventory");
		}
		ret = dbInventory.AddItem(uid, type, count);
		if(ret != 0)
		{
			error_log("[AddItem fail][ret=%d,uid=%u,type=%s,count=%d]", ret, uid, type.c_str(), count);
		}
		info_log("[inventory][uid=%u&item=%s&delta=%d&count=%u]",  uid, type.c_str(), count, count);
	}
	else
	{
		error_log("[GetItem fail][ret=%d,uid=%u,type=%s,change=%d]", ret, uid, type.c_str(), count);
		DB_ERROR_RETURN_MSG("get_inventory_fail");
	}
	return 0;
}

int CLogicInventory::ChangeItemsJson(unsigned uid, const Json::Value &data)
{
	int ret;
	if(!data.isObject())
	{
		return R_ERR_PARAM;
	}
	Json::Value::Members members = data.getMemberNames();
	for(Json::Value::Members::iterator itr = members.begin(); itr != members.end(); itr++)
	{
		int count;
		if(Json::GetInt(data, itr->c_str(), count))
		{
			ret = ChangeItem(uid, *itr, count);
			if(ret != 0)
			{
				error_log("[ChangeItem fail][ret=%d,uid=%u,itemscount=%d]", uid, (int)members.size());
				return ret;
			}
		}
		else
		{
			error_log("[data_error][uid=%u,type=%s]", uid, itr->c_str());
		}
	}
	return 0;
}

int CLogicInventory::GetItems(unsigned uid, map<string, unsigned> &items)
{
	int ret;
	CDataInventory dbInventory;
	ret = dbInventory.GetItems(uid, items);
	if(ret != 0)
	{
		error_log("[GetItems fail][ret=%d,uid=%u]", ret, uid);
		return ret;
	}
	return 0;
}

int CLogicInventory::GetItemsJson(unsigned uid, Json::Value &data)
{
	int ret;
	map<string, unsigned> items;
	ret = GetItems(uid, items);
	if(ret != 0)
	{
		return ret;
	}
	data = Json::Value(Json::objectValue);
	for(map<string, unsigned>::iterator itr = items.begin(); itr != items.end(); itr++)
	{
		data[itr->first] = itr->second;
	}
	return 0;
}

int CLogicInventory::GetItem(unsigned uid, const string &type, unsigned& count)
{
	int ret;
	CDataInventory dbInventory;
	ret = dbInventory.GetItem(uid, type, count);
	if(ret == R_ERR_NO_DATA)
	{
		count = 0;
	}
	else if(ret != 0)
	{
		error_log("[GetItem fail][ret=%d,uid=%u,type=%s]", ret, uid, type.c_str());
		DATA_ERROR_RETURN_MSG("get_inventory_fail");
	}
	return 0;
}

int CLogicInventory::RemoveAllItem(unsigned uid)
{
	CDataInventory dbInventory;
	int ret = dbInventory.RemoveItems(uid);
	if (ret != 0)
	{
		error_log("[RemoveItem fail][uid=%u,ret=%d]", uid, ret);
		DB_ERROR_RETURN_MSG("remove_inventory_fail");
	}
	return 0;
}
