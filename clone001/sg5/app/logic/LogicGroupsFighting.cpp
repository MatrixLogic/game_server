
/*
 * LogicGroupsFighting.h
 *
 *  Created on: 2013-08-20
 *      Author: Administrator
 */

#include "LogicGroupsFighting.h"


CLogicGroupsFighting::CLogicGroupsFighting() {
	// TODO Auto-generated constructor stub

}

CLogicGroupsFighting::~CLogicGroupsFighting() {
	// TODO Auto-generated destructor stub
}

CDataGroupsFigthing* CLogicGroupsFighting::GetGroupsFightingData(int cityid)
{
//	GET_MEM_DATA_SEM(CDataGroupsFigthing, GROUPS_FIGHTING, sem_groupsfighting)


	static map<int, CDataGroupsFigthing*> dataMap;
	int serverId = 0;
	Config::GetDomain(serverId);
	if (0 == serverId)
		return NULL;
	map<int, CDataGroupsFigthing*>::iterator itr = dataMap.find(serverId);
	if (itr != dataMap.end() && NULL != itr->second)
		return itr->second;

	static CDataGroupsFigthing* pdata = NULL;
	if (!pdata)
	{

		pdata = new CDataGroupsFigthing();
		if (pdata->Init(Config::GetValue(GROUPS_FIGHTING),cityid ,sem_groupsfighting) != 0)
		{
			delete pdata;
			pdata = NULL;
			return NULL;
		}
	}
	dataMap[serverId] = pdata;
	return pdata;

}


//获取所有数据
int CLogicGroupsFighting::GetGroupsFightingJsonData(int cityid,Json::Value &result,Json::Value &input)
{
	CDataGroupsFigthing *pdata= GetGroupsFightingData(cityid);
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_getgroupsjsondata_fail");
	}

	vector <EveryoneFormation> All_data;

	vector <EveryoneFormation> Self_data;

	struct EveryoneFormation data;
	memset(&data,0,sizeof(EveryoneFormation));

	Self_data.push_back(data);
	unsigned Data_id = 0;
	unsigned Senduid = 0;
	int FortifiedNum = 0;

	Self_data[0].alliance_id = input["soldier"][0u]["allianceId"].asUInt();
	Self_data[0].camp = input["soldier"][0u]["camp"].asInt();
	Self_data[0].fullmorale = input["soldier"][0u]["fullmorale"].asUInt();
	Self_data[0].morale = input["soldier"][0u]["morale"].asUInt();
	Self_data[0].position = input["soldier"][0u]["position"].asInt();
	Self_data[0].role = input["soldier"][0u]["role"].asInt();
	Self_data[0].state = input["soldier"][0u]["state"].asInt();
	Self_data[0].target = input["soldier"][0u]["target"].asInt();
	Self_data[0].ts = input["soldier"][0u]["ts"].asUInt();
	Self_data[0].uid = input["soldier"][0u]["uid"].asUInt();
	Self_data[0].personalWarData.attack = input["soldier"][0u]["attack"].asInt();
	Self_data[0].personalWarData.attackRate = input["soldier"][0u]["attackRate"].asInt();
	Self_data[0].personalWarData.critRate = input["soldier"][0u]["critRate"].asInt();
	Self_data[0].personalWarData.defence = input["soldier"][0u]["defence"].asUInt();
	Self_data[0].personalWarData.dodgeRate = input["soldier"][0u]["dodgeRate"].asInt();
	Self_data[0].personalWarData.level = input["soldier"][0u]["level"].asInt();
	Self_data[0].personalWarData.maxHp = input["soldier"][0u]["maxHp"].asUInt();
	Self_data[0].personalWarData.type = input["soldier"][0u]["type"].asInt();
	Self_data[0].personalWarData.typeLevels[0] = input["soldier"][0u]["typeLevel_0"].asInt();
	Self_data[0].personalWarData.typeLevels[1] = input["soldier"][0u]["typeLevel_1"].asInt();
	Self_data[0].personalWarData.typeLevels[2] = input["soldier"][0u]["typeLevel_2"].asInt();

	if(!Json::GetInt(input, "holdsNum", FortifiedNum))
	{
		return R_ERR_PARAM;
	}

	int ret = pdata->GetAllCombatData(All_data,cityid,Data_id,Senduid,Self_data);
	if(ret != 0)
	{
		return ret;
	}

	int numbers = All_data.size();
	result["seq_id"] = Data_id;
	result["senduid"] = Senduid;
	result["personal_num"] = numbers;

	error_log("uid=%u |%d ",All_data[0].uid,numbers);

	for(int i = 0; i < FortifiedNum;i++)
	{
		int num = 0;
		int camp = 0;
		for(int j = 0; j < numbers;j++)
		{
			if(i == All_data[j].position &&All_data[j].target == -1)
			{
				num++;
				camp = All_data[j].camp;
			}
		}
		result["fortified"][i]["num"] = num;
		result["fortified"][i]["camp"] = camp;
		result["fortified"][i]["position"] = i;
	}

	for(int i = 0; i < All_data.size(); i++)
	{
		result["personal_data"][i]["uid"] = All_data[i].uid;
		result["personal_data"][i]["camp"] = All_data[i].camp;
		result["personal_data"][i]["state"] = All_data[i].state;
		result["personal_data"][i]["role"] = All_data[i].role;
		result["personal_data"][i]["morale"] = All_data[i].morale;
		result["personal_data"][i]["position"] = All_data[i].position;
		result["personal_data"][i]["fullmorale"] = All_data[i].fullmorale;
		result["personal_data"][i]["allianceId"] = All_data[i].alliance_id;
		result["personal_data"][i]["target"] = All_data[i].target;
		result["personal_data"][i]["ts"] = All_data[i].ts;
		result["personal_data"][i]["attack"] = All_data[i].personalWarData.attack;
		result["personal_data"][i]["attackRate"] = All_data[i].personalWarData.attackRate;
		result["personal_data"][i]["critRate"] = All_data[i].personalWarData.critRate;
		result["personal_data"][i]["defence"] = All_data[i].personalWarData.defence;
		result["personal_data"][i]["dodgeRate"] = All_data[i].personalWarData.dodgeRate;
		result["personal_data"][i]["level"] = All_data[i].personalWarData.level;
		result["personal_data"][i]["maxHp"] = All_data[i].personalWarData.maxHp;
		result["personal_data"][i]["type"] = All_data[i].personalWarData.type;
		result["personal_data"][i]["typeLevels_0"] = All_data[i].personalWarData.typeLevels[0];
		result["personal_data"][i]["typeLevels_1"] = All_data[i].personalWarData.typeLevels[1];
		result["personal_data"][i]["typeLevels_2"] = All_data[i].personalWarData.typeLevels[2];
	}
	return 0;
}

int CLogicGroupsFighting::LogicUpdatasFightingData(int cityid,Json::Value &input,unsigned uid)
{
	CDataGroupsFigthing *pdata= GetGroupsFightingData(cityid);
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_getgroupsjsondata_fail");
	}

	vector <EveryoneFormation> All_data;
	unsigned Data_id;
	unsigned Senduid;
	int map_id;
	int personal_num;

	if(!Json::GetUInt(input, "seq_id", Data_id) ||
	   !Json::GetInt(input, "personal_num", personal_num) || !Json::GetInt(input, "map_id", map_id))
	{
		return R_ERR_PARAM;
	}
	Senduid = uid;

	for(int i =0; i < personal_num; i++)
	{
		All_data[i].alliance_id = input["personal_data"][i]["alliance_id"].asUInt();
		All_data[i].camp = input["personal_data"][i]["camp"].asInt();
		All_data[i].fullmorale = input["personal_data"][i]["fullmorale"].asUInt();
		All_data[i].morale = input["personal_data"][i]["morale"].asUInt();
		All_data[i].position = input["personal_data"][i]["position"].asInt();
		All_data[i].role = input["personal_data"][i]["role"].asInt();
		All_data[i].state = input["personal_data"][i]["state"].asInt();
		All_data[i].target = input["personal_data"][i]["target"].asInt();
		All_data[i].ts = input["personal_data"][i]["ts"].asUInt();
		All_data[i].uid = input["personal_data"][i]["uid"].asUInt();
		All_data[i].personalWarData.attack = input["personal_data"][i]["attack"].asInt();
		All_data[i].personalWarData.attackRate = input["personal_data"][i]["attackRate"].asInt();
		All_data[i].personalWarData.critRate = input["personal_data"][i]["critRate"].asInt();
		All_data[i].personalWarData.defence = input["personal_data"][i]["defence"].asUInt();
		All_data[i].personalWarData.dodgeRate = input["personal_data"][i]["dodgeRate"].asInt();
		All_data[i].personalWarData.level = input["personal_data"][i]["level"].asInt();
		All_data[i].personalWarData.maxHp = input["personal_data"][i]["maxHp"].asUInt();
		All_data[i].personalWarData.type = input["personal_data"][i]["type"].asInt();
		All_data[i].personalWarData.typeLevels[0] = input["personal_data"][i]["typeLevels_0"].asInt();
		All_data[i].personalWarData.typeLevels[1] = input["personal_data"][i]["typeLevels_1"].asInt();
		All_data[i].personalWarData.typeLevels[2] = input["personal_data"][i]["typeLevels_2"].asInt();
	}

	int ret = pdata->UpdatasFightingData(All_data,cityid,Data_id,Senduid,map_id);
	if(ret != 0)
		return ret;
	return 0;

}

int CLogicGroupsFighting::LogicAddFullmorale(unsigned uid, int cityid)
{
	CDataGroupsFigthing *pdata= GetGroupsFightingData(cityid);
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_nationalwar_rgt_fail");
	}

	int ret = pdata->AddFullmorale(cityid,uid);
	if(ret != 0)
	{
		return ret;
	}

	return 0;

}


int CLogicGroupsFighting::CleanGroupsFightingDate(int cityid)
{
	CDataGroupsFigthing *pdata= GetGroupsFightingData(cityid);
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_nationalwar_rgt_fail");
	}
	return pdata->CleanGroupsfigthingData(cityid);
}
