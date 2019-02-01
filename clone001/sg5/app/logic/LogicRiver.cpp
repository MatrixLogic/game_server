/*
 * LogicRiver.cpp
 *
 *  Created on: 2013-3-20
 *      Author: Administrator
 */

#include "LogicRiver.h"

CLogicRiver::CLogicRiver(){
}

CDataRiver* CLogicRiver::GetDataRiver(void)
{
	GET_MEM_DATA_SEM(CDataRiver, CONFIG_RIVER_PATH, sem_river)
	/*static CDataRiver *pData = NULL;
	if (NULL == pData)
	{
		pData = new CDataRiver;
		int ret = pData->Init(Config::GetValue(CONFIG_RIVER_PATH));
		if (0 != ret)
		{
			delete pData;
			pData = NULL;
		}
	}
	return pData;*/
}

int CLogicRiver::SetShip(unsigned uid, const Json::Value &param)
{
	int ret = 0;
	int posy = 0;
	if(!Json::GetInt(param, "posy", posy))
	{
		PARAM_ERROR_RETURN_MSG("posy");
	}
	int attactnum = 0;
	if(!Json::GetInt(param, "attacktimes", attactnum))
	{
		PARAM_ERROR_RETURN_MSG("attacktimes");
	}
	int type = 0;
	if(!Json::GetInt(param, "type", type))
	{
		PARAM_ERROR_RETURN_MSG("type");
	}
	unsigned startTs = 0;
	if(!Json::GetUInt(param, "startts", startTs))
	{
		PARAM_ERROR_RETURN_MSG("startts");
	}
	unsigned endTs = 0;
	if(!Json::GetUInt(param, "endts", endTs))
	{
		PARAM_ERROR_RETURN_MSG("endts");
	}
	unsigned userid = 0;
	if(!Json::GetUInt(param, "uid", userid))
	{
		PARAM_ERROR_RETURN_MSG("uid");
	}

	DataShip data;
	data.uid = userid;
	data.posy = posy;
	data.attack_num = attactnum;
	data.start_time = startTs;
	data.type = type;
	data.end_time = endTs;

	CLogicUserBasic logicUserBasic;
	DataUserBasic userBasic;
	CLogicUser logicUser;
	DataUser user;
	CLogicAlliance logicAlliance;
	DataAlliance alliance;
	ret = logicUserBasic.GetUserBasicLimit(userid, OpenPlatform::GetType(), userBasic);
	if (0 != ret)
	{
		return ret;
	}
	ret = logicUser.GetUserLimit(userid, user);
	if (0 != ret)
	{
		return ret;
	}
	if (IsAllianceId(user.alliance_id))
	{
		logicAlliance.GetAllianceLimit(user.alliance_id, alliance);
	}

	data.user_level = user.level;
	strcpy(data.user_name,userBasic.name.c_str());
	strcpy(data.alliance_name,alliance.name.c_str());

	CDataRiver *pData = GetDataRiver();
	if (NULL == pData)
	{
		error_log("GetDataRiver faile");
		return R_ERR_DATA;
	}

	ret = pData->AddShip(data);
	if (0 != ret)
	{
		error_log("add ship to mem fail,ret=%d", ret);
		return ret;
	}

	return 0;
}

int CLogicRiver::GetAllShipJson(Json::Value &result)
{
	int ret = 0;
	CDataRiver *pData = GetDataRiver();
	if (NULL == pData)
	{
		error_log("GetDataRiver faile");
		return R_ERR_DATA;
	}
	vector<DataShip> data;
	ret = pData->GetShips(data);
	if (0 != ret)
	{
		return ret;
	}

	Json::Value shipJson;
	vector<DataShip>::iterator itr = data.begin();
	for (; itr!=data.end(); itr++)
	{
		GetShipJson(*itr, shipJson);

		unsigned shipEndTimn = 0;
		if (Json::GetUInt(shipJson, "endts", shipEndTimn) && shipEndTimn <= Time::GetGlobalTime())
		{
			ret = pData->RemoveShip((*itr).uid);
			if (0 == ret)
			{
				UpdatesEndOfShipping(*itr);
				continue;
			}
		}
		result[result.size()] = shipJson;
	}

	return 0;
}

int CLogicRiver::RemoveShip(unsigned uid)
{
	int ret = 0;
	CDataRiver *pData = GetDataRiver();
	if (NULL == pData)
	{
		error_log("GetDataRiver fail");
		return R_ERR_DATA;
	}
	ret = pData->RemoveShip(uid);
	if (0 != ret)
	{
		error_log("RemoveShip fai ret=%d", ret);
		return ret;
	}
	return 0;
}

int CLogicRiver::GetShipJson(unsigned uid, Json::Value &result)
{
	int ret = 0;
	DataShip ship;
	CDataRiver *pData = GetDataRiver();
	if (NULL == pData)
	{
		error_log("GetDataRiver faile");
		return R_ERR_DATA;
	}

	ret = pData->GetOneShip(uid,ship);
	if (0 != ret)
	{
		error_log("[Get Ship fail][ret=%d,uid=%u]",ret,uid);
		return ret;
	}

	result["type"] = ship.type;
	result["uid"] = ship.uid;
	result["name"] = string(ship.user_name);
	result["attacktimes"] = ship.attack_num;
	result["endts"] = ship.end_time;
	result["startts"] = ship.start_time;
	result["union"] = string(ship.alliance_name);
	result["level"] = ship.user_level;
	result["posy"] = ship.posy;

	return 0;
}
int CLogicRiver::GetShipJson(DataShip ship, Json::Value &result)
{
	result["type"] = ship.type;
	result["uid"] = ship.uid;
	result["name"] = string(ship.user_name);
	result["attacktimes"] = ship.attack_num;
	result["endts"] = ship.end_time;
	result["startts"] = ship.start_time;
	result["union"] = string(ship.alliance_name);
	result["level"] = ship.user_level;
	result["posy"] = ship.posy;

	return 0;
}

int CLogicRiver::SetAttackLog(unsigned uid, const Json::Value &logdata)
{
	int ret = 0;
	CDataWeapon dbWeapon;
	DataShip ship;
	ship.uid = uid;
	string attackLog;
	ret = dbWeapon.GetAttackLog(uid, attackLog);
	if(ret == R_ERR_NO_DATA)
			ret = dbWeapon.AddAttackLog(uid, attackLog);
	if (0 != ret)
	{
		error_log("[db get ship fail][uid=%u,ret=%d]",ship.uid, ret);
		DB_ERROR_RETURN_MSG("db_get_ship_fail");
	}

	Json::Reader reader;
	Json::Value attackLogJson;
	if (!attackLog.empty() && !reader.parse(attackLog, attackLogJson))
	{
		error_log("[parse ship attack log fail][uid=%u,ret=%d]",ship.uid, ret);
		DATA_ERROR_RETURN_MSG("parse_ship_attack_log_fial");
	}

	attackLogJson.append(logdata);
	unsigned size = attackLogJson.size();
	if (size > 4)
	{
		for (unsigned i = 0; i < 4; i++)
		{
			attackLogJson[i] = attackLogJson[size - 4 + i];
		}
		attackLogJson.resize(4);
	}

	Json::FastWriter writer;
	ret = dbWeapon.SetAttackLog(uid, writer.write(attackLogJson));
	if (0 != ret)
	{
		error_log("[db set ship attack log fail][uid=%u,ret=%d]",ship.uid, ret);
		DB_ERROR_RETURN_MSG("db_set_ship_attack_log_fail");
	}
	return 0;
}

int CLogicRiver::GetAttackLog(unsigned uid, Json::Value &logdata)
{
	int ret = 0;
	CDataWeapon dbWeapon;
	DataShip ship;
	ship.uid = uid;
	string attackLog;
	Json::Value attackLogJson;
	ret = dbWeapon.GetAttackLog(uid, attackLog);
	if (0 != ret && ret != R_ERR_NO_DATA)
	{
		error_log("[db get ship fail][uid=%u,ret=%d]",ship.uid, ret);
		DB_ERROR_RETURN_MSG("db_get_ship_fail");
	}
	Json::Reader reader;
	if (attackLog.empty())
	{
		Json::Value tempLog = Json::Value(Json::arrayValue);
		attackLogJson = tempLog;
		return 0;
	}

	if (!reader.parse(attackLog, logdata))
	{
		error_log("[ship attack log parse fail][uid=%u]",uid);
		DATA_ERROR_RETURN_MSG("parse_json_fail");
	}

	return 0;
}

int CLogicRiver::UpdatesEndOfShipping(DataShip ship)
{
	int ret = 0;

	Json::Value updates = Json::Value(Json::arrayValue);
	updates.resize(1);
	updates[(unsigned) 0]["s"] = "boatfinish";
	updates[(unsigned) 0]["uid"] = ship.uid;
	updates[(unsigned) 0]["ts"] = Time::GetGlobalTime();
	updates[(unsigned) 0]["attacktimes"] = ship.attack_num;
	updates[(unsigned) 0]["type"] = ship.type;
	updates[(unsigned) 0]["level"] = ship.user_level;
	CLogicUpdates logicUpdates;
	ret = logicUpdates.AddUpdates(ship.uid, updates);
	if (0 != ret)
	{
		error_log("AddUpdates fail uid=%u", ship.uid);
		return ret;
	}
	info_log("[boat finish][uid=%u,type=%d,end_time=%u]", ship.uid, ship.type, ship.end_time);
	return 0;
}

