/*
 * LogicWorld.cpp
 *
 *  Created on: 2012-8-21
 *      Author: Administrator
 */

#include "LogicWorld.h"
#define BLOCK_SIZE 100
CLogicWorld::CLogicWorld() {
	// TODO Auto-generated constructor stub

}
CLogicWorld::~CLogicWorld() {
	// TODO Auto-generated destructor stub
}

CDataWorld *CLogicWorld::GetDataWorld()
{
	GET_MEM_DATA_SEM(CDataWorld, CONFIG_WORLD_DATA, sem_world)
	/*static CDataWorld *pData = NULL;

	if(pData == NULL)
	{
		pData = new CDataWorld;
	}
	if(!pData->Init(Config::GetValue(CONFIG_WORLD_DATA)))
	{
		delete pData;
		pData = NULL;
	}
	return pData;*/

}

int CLogicWorld::Move(const Json::Value &data,Json::Value &result)
{
	vector<WorldPoint> vWorldPoints;
	string jsonkey;
	int  i = 0;
	int iBlockCount = 0;
	int iCityCount = 0;
	int iFieldCount = 0;
	short  begin_x = 0;
	short  begin_y = 0;
	short  end_x = 0;
	short  end_y = 0;
	int iIndex =0;
	int iblockid;
	int ret = 0;
	vector<int> vblockid;
	CLogicUser logicUser;
	DataUser user;
	CLogicAlliance logicAlliance;
	DataAlliance alliance;
	CLogicUserBasic logicUserBasic;
	DataUserBasic userBasic;
	if(!data.isArray())
	{
		error_log("params error");
		PARAM_ERROR_RETURN_MSG("params_error");
	}
	iBlockCount = data.size();
	for( i = iBlockCount - 1;i >=0; i--)
	{
		if(Json::GetInt(data,i,iblockid))
		{
			vblockid.push_back(iblockid);
		}
	}
	CDataWorld *pdata = GetDataWorld();
	if(NULL == pdata)
	{
		return -1;
	}
	for(i = 0; i < iBlockCount; i++)
	{
		if(vblockid[i] >= 2500 || vblockid[i] < 0)
		{
			error_log("params error");
			PARAM_ERROR_RETURN_MSG("params_error");
		}
		// 获取请求块左上角坐标 begin_x begin_y
		begin_x = ((vblockid[i]) / 50) * 10;
		begin_y = ((vblockid[i]) % 50) * 10;
		// 获取请求块右下角角坐标 end_x end_y
		end_x = begin_x + 9;
		end_y = begin_y + 9;
		// 获取每一小块的地图信息
		result[i]["block_id"] = vblockid[i];
		pdata->Move(begin_x, begin_y, end_x, end_y,vWorldPoints);
		iCityCount = 0;
		iFieldCount = 0;
		for(iIndex = 0; iIndex < BLOCK_SIZE ;iIndex++)
		{
			if(( vWorldPoints[iIndex].map_flag == JUN_CITY
					|| vWorldPoints[iIndex].map_flag == ZHOU_CITY
					|| vWorldPoints[iIndex].map_flag == DU_CITY
					|| vWorldPoints[iIndex].map_flag == USER_MAIN_CITY
					|| vWorldPoints[iIndex].map_flag == USER_BRANCH_CITY))
			{
//				if(vWorldPoints[iIndex].uid != 0)
//				{
					//result[i]["world_city"][iCityCount]["level"] = vWorldPoints[iIndex].level;
				if(IsValidUid(vWorldPoints[iIndex].uid))
				{
					ret = logicUser.GetUserLimit(vWorldPoints[iIndex].uid,user);
					if(0 != ret){
						error_log("get_user_limit failed uid=%u",vWorldPoints[iIndex].uid);
					}
					ret = logicUserBasic.GetUserBasicLimit(vWorldPoints[iIndex].uid,OpenPlatform::GetType(),userBasic);
					if(0 != ret){
						error_log("get_user_basic_limit failed uid=%u",vWorldPoints[iIndex].uid);
					}
					if(IsAllianceId(user.alliance_id))
					ret = logicAlliance.GetAlliance(user.alliance_id,alliance);
					if(0 != ret){
						error_log("get_alliance failed uid=%u,alliance_id=%u",vWorldPoints[iIndex].uid,user.alliance_id);
					}
					result[i]["world_city"][iCityCount]["level"] = user.level;
					result[i]["world_city"][iCityCount]["name"] = userBasic.name;
					result[i]["world_city"][iCityCount]["aname"] = alliance.name;
				}
					result[i]["world_city"][iCityCount]["cityname"] = vWorldPoints[iIndex].cityname;
					result[i]["world_city"][iCityCount]["user_state"] = vWorldPoints[iIndex].user_state;
					result[i]["world_city"][iCityCount]["world_pos"] = vWorldPoints[iIndex].x * 1000 + vWorldPoints[iIndex].y;
					result[i]["world_city"][iCityCount]["uid"] = vWorldPoints[iIndex].uid;
					//result[i]["world_city"][iCityCount]["cid"] = vWorldPoints[iIndex].cid;
					//result[i]["world_city"][iCityCount]["union_id"] = vWorldPoints[iIndex].alliance_id;
					//result[i]["world_city"][iCityCount]["union_name"] = string(vWorldPoints[iIndex].alliance_name);
					//result[i]["world_city"][iCityCount]["username"] = string(vWorldPoints[iIndex].username);
					result[i]["world_city"][iCityCount]["type"] = vWorldPoints[iIndex].map_flag;
					//result[i]["world_city"][iCityCount]["state"] = vWorldPoints[iIndex].state;
					//[i]["world_city"][iCityCount]["protect_time"] = vWorldPoints[iIndex].protect_time;
					//result[i]["world_city"][iCityCount]["user_fight_power"] = Convert::UInt64ToString(vWorldPoints[iIndex].user_fight_power);
					result[i]["world_city"][iCityCount]["top_worldpos"] = vWorldPoints[iIndex].top_worldpos;

					iCityCount++;
//				}
			}
			else{
//				if(vWorldPoints[iIndex].uid != 0)
//				{
					//result[i]["world_field"][iFieldCount]["level"] = vWorldPoints[iIndex].level;
					result[i]["world_field"][iFieldCount]["world_pos"] = vWorldPoints[iIndex].x * 1000 + vWorldPoints[iIndex].y ;
					result[i]["world_field"][iFieldCount]["owner_uid"] = vWorldPoints[iIndex].uid;
					//result[i]["world_field"][iFieldCount]["owner_cid"] = vWorldPoints[iIndex].cid;
					//result[i]["world_field"][iFieldCount]["owner_user_name"] = string(vWorldPoints[iIndex].username);
					result[i]["world_field"][iFieldCount]["type"] = vWorldPoints[iIndex].map_flag;
					//result[i]["world_field"][iFieldCount]["state"] = vWorldPoints[iIndex].state;
					//result[i]["world_field"][iFieldCount]["protect_time"] = vWorldPoints[iIndex].protect_time;
					result[i]["world_field"][iFieldCount]["top_worldpos"] = vWorldPoints[iIndex].top_worldpos;
					iFieldCount++;
//				}
			}
		}
		vWorldPoints.clear();
	}
	return 0;
}

int CLogicWorld::AddUser(const WorldPoint &worldpoint, int unsigned &world_pos)
{

	CDataWorld *pdata = GetDataWorld();
	if(NULL == pdata)
	{
		return -1;
	}
	int ret = pdata->AddUser(worldpoint,world_pos);
	if(ret != 0)
	{
		return ret;
	}
	return 0;
}


int CLogicWorld::ReplaceUser(const WorldPoint &worldpoint, int unsigned &world_pos)
{

	CDataWorld *pdata = GetDataWorld();
	if(NULL == pdata)
	{
		return -1;
	}
	if(!IsValidWorldPos(world_pos))
	{
		error_log("world_pos out of range world_pos = %d",world_pos);
		REFUSE_RETURN_MSG("invalid_world_pos");

	}
	int ret = pdata->ReplaceUser(worldpoint,world_pos);
	if(ret != 0)
	{
		return ret;
	}
	return 0;
}

int CLogicWorld::AddUser(const WorldPoint &worldpoint, int x, int y)
{

	CDataWorld *pdata = GetDataWorld();
	// string userFlag;

	if(NULL == pdata)
	{
		return -1;
	}
	int ret = pdata->ReplaceUser(worldpoint,x, y);
	if(ret != 0)
	{
		return ret;
	}
	CLogicBaseExtra baseExtra;
	DataBaseExtra dataBaseExtra;
	dataBaseExtra.worldpos = (x + 1) * 1000 + y + 1;
	dataBaseExtra.occupy_time = Time::GetGlobalTime();
	ret = baseExtra.AddBaseExtra(worldpoint.uid,dataBaseExtra);
	info_log("add user success uid = %d,x = %d;",worldpoint.uid, x,y);

	return ret;
}

int CLogicWorld::GetUserInfo(const int unsigned world_pos,WorldPoint &worldPoint)
{
	CDataWorld *pdata =  GetDataWorld();
	if(NULL == pdata)
	{
		return -1;
	}
//	if(!IsValidWorldPos(world_pos))
//	{
//		error_log("world_pos out of range world_pos = %d",world_pos);
//		REFUSE_RETURN_MSG("invalid_world_pos");
//
//	}
	int ret = pdata->GetWorldUser(world_pos,worldPoint);
	if(ret != 0)
	{
		return ret;
	}
	return 0;
}


int CLogicWorld::MoveCity(unsigned uid,unsigned oldWorldpos,unsigned toWorldpos)
{
	int ret = 0;
	CLogicUser logicUser;
	CDataWorld *pdata =  GetDataWorld();
	if(NULL == pdata)
	{
		return -1;
	}

	if(oldWorldpos == 0)
	{

		//unsigned oldpos;
		logicUser.GetMainpos(uid,oldWorldpos);
	}
	ret = pdata->MoveCity(uid,oldWorldpos,toWorldpos);
	if(0  != ret)
	{
		return ret;
	}
	logicUser.SetMainpos(uid,toWorldpos);
	return ret;
}



