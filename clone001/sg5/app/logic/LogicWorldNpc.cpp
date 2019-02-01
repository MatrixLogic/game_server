/*
 * LogicWorldNpc.cpp
 *
 *  Created on: 2013-1-23
 *      Author: Administrator
 */

#include "LogicWorldNpc.h"

CLogicWorldNpc::CLogicWorldNpc() {
	// TODO Auto-generated constructor stub

}

CLogicWorldNpc::~CLogicWorldNpc() {
	// TODO Auto-generated destructor stub
}

CDataWorldNpc *CLogicWorldNpc::GetDataWorldNpc()
{
	GET_MEM_DATA_SEM(CDataWorldNpc, CONFIG_WORLDNPC_DATA, sem_worldnpc)
	/*static CDataWorldNpc *pData = NULL;

	if(pData == NULL)
	{
		pData = new CDataWorldNpc;
	}
	if(!pData->Init(Config::GetValue(CONFIG_WORLDNPC_DATA)))
	{
		delete pData;
		pData = NULL;
	}
	return pData;*/
}

int CLogicWorldNpc::ReplaceNpc(const NpcItem &npc )
{
	CDataWorldNpc *pdata = GetDataWorldNpc();
	if(NULL == pdata)
	{
		return R_ERR_DB;
	}
	int ret = pdata->ReplaceNpc(npc);
	return ret;
}

int CLogicWorldNpc::GetNpc(NpcItem &npc )
{
	CDataWorldNpc *pdata = GetDataWorldNpc();
	if(NULL == pdata)
	{
		return R_ERR_DB;
	}
	int ret = pdata->GetNpc(npc);
	return ret;
}

int CLogicWorldNpc::GetAllNpc( Json::Value &npcValue)
{
	CDataWorldNpc *pdata = GetDataWorldNpc();
	if(NULL == pdata)
	{
		return R_ERR_DB;
	}
	NpcItem Npc[MAX_WORLD_NPC_COUNT];
	int iCount;
	int ret = pdata->GetAllNpc(Npc,iCount);
	if(0 != ret)
	{
		error_log("get_world_npc_failed");
		return ret;
	}
	int i;
	int freeCount = 0;
	int occupiedCount = 0;
	int duCount = 0;
	int zhouCount = 0;
	string name;
	CLogicUserBasic logicUserBasic;
	DataUserBasic userBasic;
	string  signature;
	string figure_url;
	DataBaseExtra baseExtra;
	CLogicBaseExtra logicBaseExtra;
	unsigned now = Time::GetGlobalTime();
	for(i = 0; i < iCount; i++)
	{

		if(Npc[i].GetType() == JUN_CITY)
		{
			if(IsValidUid(Npc[i].uid))
			{
				npcValue["occupied_jun"][occupiedCount]["world_pos"] = Npc[i].worldpos;
				npcValue["occupied_jun"][occupiedCount]["type"] = Npc[i].GetType();
				npcValue["occupied_jun"][occupiedCount]["reward"] = Npc[i].GetReward();
				npcValue["occupied_jun"][occupiedCount]["nextreward"] = Npc[i].GetNextReward();
				npcValue["occupied_jun"][occupiedCount]["npcid"] = Npc[i].uid;
				npcValue["occupied_jun"][occupiedCount]["cityname"] = Npc[i].cityName;
				npcValue["occupied_jun"][occupiedCount]["name"] = Npc[i].name;
				logicBaseExtra.GetBaseExtra(Npc[i].uid,Npc[i].worldpos,baseExtra);
				if(baseExtra.protected_time <=  now)
				{
					npcValue["occupied_jun"][occupiedCount]["attackpermitted"] = APT_ALLOW;
				}else{
					npcValue["occupied_jun"][occupiedCount]["attackpermitted"] = APT_BAN_DAMAGE_PROTECT;
				}
				occupiedCount++;
			}else{
				npcValue["free_jun"][freeCount]["world_pos"] = Npc[i].worldpos;
				npcValue["free_jun"][freeCount]["type"] = Npc[i].GetType();
				npcValue["free_jun"][freeCount]["reward"] = Npc[i].GetReward();
				npcValue["free_jun"][freeCount]["nextreward"] = Npc[i].GetNextReward();
				npcValue["free_jun"][freeCount]["npcid"] = Npc[i].uid;
				npcValue["free_jun"][freeCount]["cityname"] = Npc[i].cityName;
				npcValue["free_jun"][freeCount]["name"] = Npc[i].name;
				npcValue["free_jun"][freeCount]["attackpermitted"] = APT_ALLOW;
				freeCount++;
			}
		}else if(Npc[i].GetType() == DU_CITY){
			npcValue["du"][duCount]["world_pos"] = Npc[i].worldpos;
			npcValue["du"][duCount]["type"] = Npc[i].GetType();
			npcValue["du"][duCount]["reward"] = Npc[i].GetReward();
			npcValue["du"][duCount]["nextreward"] = Npc[i].GetNextReward();
			npcValue["du"][duCount]["npcid"] = Npc[i].uid;
			npcValue["du"][duCount]["cityname"] = Npc[i].cityName;
			npcValue["du"][duCount]["name"] = Npc[i].name;
			if(IsValidUid(Npc[i].uid))
			{
				logicBaseExtra.GetBaseExtra(Npc[i].uid,Npc[i].worldpos,baseExtra);
				if(baseExtra.protected_time <=  now)
				{
					npcValue["du"][duCount]["attackpermitted"] = APT_ALLOW;
				}else{
					npcValue["du"][duCount]["attackpermitted"] = APT_BAN_DAMAGE_PROTECT;
				}
			}else{
				npcValue["du"][duCount]["attackpermitted"] = APT_ALLOW;
			}
			duCount++;
		}else if(Npc[i].GetType() == ZHOU_CITY){
			npcValue["zhou"][zhouCount]["name"] = Npc[i].name;
			npcValue["zhou"][zhouCount]["world_pos"] = Npc[i].worldpos;
			npcValue["zhou"][zhouCount]["npcid"] = Npc[i].uid;;
			npcValue["zhou"][zhouCount]["type"] = Npc[i].GetType();
			npcValue["zhou"][zhouCount]["reward"] = Npc[i].GetReward();
			npcValue["zhou"][zhouCount]["nextreward"] = Npc[i].GetNextReward();
			npcValue["zhou"][zhouCount]["cityname"] = Npc[i].cityName;
			if(IsValidUid(Npc[i].uid))
			{
				logicBaseExtra.GetBaseExtra(Npc[i].uid,Npc[i].worldpos,baseExtra);
				if(baseExtra.protected_time <=  now)
				{
					npcValue["zhou"][zhouCount]["attackpermitted"] = APT_ALLOW;
				}else{
					npcValue["zhou"][zhouCount]["attackpermitted"] = APT_BAN_DAMAGE_PROTECT;
				}
			}else{
				npcValue["zhou"][zhouCount]["attackpermitted"] = APT_ALLOW;
			}
			zhouCount++;
		}

	}
	return ret;
}
int CLogicWorldNpc::RemoveNpc(const NpcItem &npc )
{
	CDataWorldNpc *pdata = GetDataWorldNpc();
	if(NULL == pdata)
	{
		return 6;
	}
	int ret = pdata->RemoveNpc(npc);
	return ret;
}
