#include "LogicUserData.h"

int CLogicUserData::InitializeUserData(unsigned uid, DataUserData *pData)
{
	DataUserData userData;
	userData.uid = uid;
	userData.tribute_list = "[]";
	userData.npcattack = "";
	userData.gift = "";
	userData.ladder = "";
	userData.dailyquest = "";
	CDataUserData userDataDB;
	int ret = userDataDB.AddUserData(userData);
	if (ret != 0)
	{
		error_log("[AddUserData fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("add_user_data_fail");
	}
	if (pData)
	{
		pData->uid = userData.uid;
		pData->tribute_list = userData.tribute_list;
		pData->npcattack = userData.npcattack;
		pData->gift = userData.gift;
		pData->ladder = userData.ladder;
		pData->dailyquest = userData.dailyquest;
	}
	return 0;
}

int CLogicUserData::GetUserData(unsigned uid, DataUserData &data)
{
	CDataUserData userDataDB;
	int ret = userDataDB.GetUserData(uid, data);
	if (ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[GetUserData fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("get_user_data_fail");
	}
	if (ret == R_ERR_NO_DATA)
	{
		ret = InitializeUserData(uid, &data);
		if (ret != 0)
			return ret;
	}
	return 0;
}

int CLogicUserData::SetUserData(unsigned uid, const DataUserData &data)
{
	CDataUserData userDataDB;
	int ret = userDataDB.SetUserData(uid, data);
	if (ret != 0)
	{
		error_log("[SetUserData fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("set_user_data_fail");
	}
	return 0;
}

int CLogicUserData::SetTributeList(unsigned uid, const Json::Value &data)
{
	if (data.isNull() || !data.isArray())
	{
		error_log("[data type error][uid=%u,type=%d]",uid,data.type());
		DATA_ERROR_RETURN_MSG("tribute_list_type_error");
	}
	Json::FastWriter writer;
	CDataUserData userDataDB;
	int ret = userDataDB.SetTributeList(uid, writer.write(data));
	if (ret != 0)
	{
		error_log("[SetTributeList fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("set_user_data_fail");
	}
	return 0;
}

int CLogicUserData::SetNpcattack(unsigned uid, const Json::Value &data)
{
	if (data.isNull())
	{
		error_log("[data type error][uid=%u,type=%d]",uid,data.type());
		DATA_ERROR_RETURN_MSG("npcattack_type_error");
	}
	Json::FastWriter writer;
	CDataUserData userDataDB;
	int ret = userDataDB.SetNpcattack(uid, writer.write(data));
	if (ret != 0)
	{
		error_log("[SetNpcattack fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("set_user_data_fail");
	}
	return 0;
}

int CLogicUserData::SetGift(unsigned uid, const Json::Value &data)
{
	if (data.isNull())
	{
		error_log("[data type error][uid=%u,type=%d]",uid,data.type());
		DATA_ERROR_RETURN_MSG("gift_type_error");
	}
	Json::FastWriter writer;
	CDataUserData userDataDB;
	int ret = userDataDB.SetGift(uid, writer.write(data));
	if (ret != 0)
	{
		error_log("[SetGift fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("set_user_data_fail");
	}
	return 0;
}

int CLogicUserData::SetLadder(unsigned uid, const Json::Value &data)
{
	if (data.isNull())
	{
		error_log("[data type error][uid=%u,type=%d]",uid,data.type());
		DATA_ERROR_RETURN_MSG("ladder_type_error");
	}
	Json::FastWriter writer;
	CDataUserData userDataDB;
	int ret = userDataDB.SetLadder(uid, writer.write(data));
	if (ret != 0)
	{
		error_log("[SetLadder fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("set_user_data_fail");
	}
	return 0;
}

int CLogicUserData::SetDailyQuest(unsigned uid, const Json::Value &data)
{
	if (data.isNull())
	{
		error_log("[data type error][uid=%u,type=%d]",uid,data.type());
		DATA_ERROR_RETURN_MSG("dailyquest_type_error");
	}
	Json::FastWriter writer;
	CDataUserData userDataDB;
	int ret = userDataDB.SetDailyQuest(uid, writer.write(data));
	if (ret != 0)
	{
		error_log("[SetDailyQuest fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("set_user_data_fail");
	}
	return 0;
}
int CLogicUserData::AddWorldposCollect(unsigned uid,const unsigned worldpos)
{
	CDataUserData userData;
	string worldpos_collect;
	Json::Reader reader;
	Json::Value collectValue;
	Json::FastWriter writer;
	int i =0;
	int ret = 0;
	userData.GetWorldposCollect(uid,worldpos_collect);
	if(!worldpos_collect.empty())
	{
		if(!reader.parse(worldpos_collect,collectValue))
		{
			error_log("parse worldpos collect failed uid=%u",uid);
			collectValue.clear();
			//return R_ERR_PARAM ;
		}
		if(collectValue.size() > 20)
		{
			//error_log("world collect full uid=%u",uid);
			return R_ERR_LOGIC;
		}
	}

	for(i = 0; i < collectValue.size(); i ++)
	{
		if(collectValue[i].asUInt() == worldpos)
		{
//			ret = userData.SetWorldposCollect(uid,worldpos_collect);
//			if(ret != 0)
//			{
//				error_log("set worldpos collect failed uid=%u",uid);
//				return R_ERR_DB;
//			}
			return 0;
		}
	}
	collectValue[collectValue.size()] = worldpos;
	worldpos_collect = writer.write(collectValue);
	ret = userData.SetWorldposCollect(uid,worldpos_collect);
	if(ret != 0)
	{
		error_log("set worldpos collect failed uid=%u",uid);
		return R_ERR_DB            ;
	}
	return 0;
}

int CLogicUserData::DelWorldposCollect(unsigned uid, const unsigned world_pos)
{
	int ret =0, i =0;
	int iCount = 0;
	int j = 0;
	CDataUserData userData;
	string worldpos_collect;
	Json::Reader reader;
	Json::Value collectValue;
	Json::Value newCollectValue;
	Json::FastWriter writer;
	userData.GetWorldposCollect(uid,worldpos_collect);
	//newCollectValue.resize()
	reader.parse(worldpos_collect,collectValue);
	iCount = collectValue.size();

	worldpos_collect.clear();
	for(i = 0,j = 0; i < iCount; i++)
	{
		//error_log("collect = %s",newCollectValue.asString().c_str());
		if(collectValue[i].asUInt() != world_pos)
		{
			newCollectValue[j] = collectValue[i];
			j++;
		}
		//error_log("collect = %s",worldpos_collect[i].asString().c_str());


	}
	//error_log("collect = %s",newCollectValue.asString().c_str());
	worldpos_collect = writer.write(newCollectValue);
	userData.SetWorldposCollect(uid,worldpos_collect);
	return 0;
}
int CLogicUserData::GetWorldposCollect(unsigned uid, Json::Value &result)
{
	CDataUserData userData;
	int i =0;
	int iCount = 0;
	string worldpos_collect;
	Json::Value collect;
	Json::Reader reader;
	unsigned worldpos;
	CLogicWorld logicWorld;
	WorldPoint worldPoint;
	CLogicUserBasic logicUserBasic;
	DataUserBasic dataUserBasic;
	int ret = userData.GetWorldposCollect(uid,worldpos_collect);
	if(ret != 0)
	{
		error_log("get worldpos collect failed, uid=%u",uid);
		return ret;
	}
	if(!worldpos_collect.empty() && !reader.parse(worldpos_collect,collect))
	{
		error_log("parse worldpos collect failed, uid=%u, worldpos_collect=%s",uid,worldpos_collect.c_str());
		return ret;
	}
	iCount = collect.size();
	for(i = 0; i < iCount; i++)
	{
		worldpos = collect[i].asUInt();
		logicWorld.GetUserInfo(worldpos,worldPoint);
		if(IsValidUid(worldPoint.uid))
		{
			logicUserBasic.GetUserBasicLimit(worldPoint.uid,OpenPlatform::GetType(),dataUserBasic);
		}
		result[i]["pos"] = worldpos;
		result[i]["t"] = worldPoint.map_flag;
		result[i]["uid"] = worldPoint.uid;
		result[i]["n"] = dataUserBasic.name;
	}

	return 0;

}
