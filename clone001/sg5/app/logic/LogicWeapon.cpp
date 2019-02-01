/*
 * LogicWeapon.cpp
 *
 *  Created on: 2013-3-13
 *      Author: Administrator
 */

#include "LogicWeapon.h"

int CLogicWeapon::SetWeapon(unsigned uid, const Json::Value &data)
{
	CDataWeapon dbWeapon;
	string weaponInfo;
	int ret = dbWeapon.GetInfo(uid, weaponInfo);
	if (R_ERR_NO_DATA == ret)
	{
		ret = dbWeapon.AddInfo(uid, Json::ToString(data));
		if (0 != ret)
		{
			error_log("[Add Weapon fail][uid=%u,ret=%d]", uid, ret);
			return ret;
		}
		return 0;
	}
	else if (0 == ret)
	{
		ret = dbWeapon.SetInfo(uid, Json::ToString(data));
		if (0 != ret)
		{
			error_log("[set Weapon fail][uid=%u,ret=%d]", uid, ret);
			return ret;
		}
		return 0;
	}
	else
	{
		return R_ERR_DATA;
	}
}

int CLogicWeapon::GetWeapon(unsigned uid, Json::Value &data)
{
	int ret = 0;
	string weaponInfo;
	CDataWeapon dbWeapon;
	ret = dbWeapon.GetInfo(uid, weaponInfo);
	if (0 != ret)
	{
		if (R_ERR_NO_DATA == ret)
		{
			data["godfb"] = "{}";
			return 0;
		}
		error_log("[get weapon info fail][uid=%u,ret=%d]",uid,ret);
		return ret;
	}

	if (weaponInfo.empty())
	{
		data["godfb"] = "{}";
		return 0;
	}
	Json::Value temData;
	if (!Json::FromString(temData, weaponInfo))
	{
		error_log("Parse Json fail");
		return R_ERR_DATA;
	}
	data["godfb"] = temData;
	return 0;
}

