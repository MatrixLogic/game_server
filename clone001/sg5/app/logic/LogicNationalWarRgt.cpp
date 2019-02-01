
/*
 * LogicNationalWarRgt.h
 *
 *  Created on: 2013-08-13
 *      Author: Administrator
 */

#include "LogicNationalWarRgt.h"


CLogicNationalWarRgt::CLogicNationalWarRgt() {
	// TODO Auto-generated constructor stub

}

CLogicNationalWarRgt::~CLogicNationalWarRgt() {
	// TODO Auto-generated destructor stub
}

CDataNationalWarRgt* CLogicNationalWarRgt::GetNationalWarRgtData()
{
	GET_MEM_DATA_SEM(CDataNationalWarRgt, NATIONAL_WAR_RGT, sem_nationalwarrgt)

}

int CLogicNationalWarRgt::GetAllCityState(unsigned uid, CDataCitiesInState Citystate[])
{
	CDataNationalWarRgt *pdata= GetNationalWarRgtData();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_nationalwar_rgt_fail");
	}
	int ret = pdata->GetCityState(Citystate);
	if(ret != 0)
	{
		return ret;
	}
	return 0;
}
int CLogicNationalWarRgt::GetAllCityStateJson(unsigned uid, Json::Value &result)
{
	CDataNationalWarRgt *pdata= GetNationalWarRgtData();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_nationalwar_rgt_fail");
	}
	CDataCitiesInState Citystate[CITY_NUM];
	int ret = GetAllCityState(uid,Citystate);
	if(ret != 0)
	{
			return ret;
	}
	int iCount = CITY_NUM;
	//三国城市状态
	for(int i = 0; i < CITY_NUM; i++)
	{
		result[i]["ChallAlliance_id"] = Citystate[i].ChallAlliance_id;
		result[i]["Chall_InviteAlliance_id_0"] = Citystate[i].Chall_InviteAlliance_id[0];
		result[i]["Chall_InviteAlliance_id_1"] = Citystate[i].Chall_InviteAlliance_id[1];
		result[i]["OccAlliance_id"] = Citystate[i].OccAlliance_id;
		result[i]["Occ_InviteAlliance_id_0"] = Citystate[i].Occ_InviteAlliance_id[0];
		result[i]["Occ_InviteAlliance_id_1"] = Citystate[i].Occ_InviteAlliance_id[1];
		result[i]["occ_ts"] = Citystate[i].Occ_ts;
		result[i]["chall_state"] = Citystate[i].chall_state;
		result[i]["chall_ts"] = Citystate[i].chall_ts;
		result[i]["chall_uid"] = Citystate[i].chall_uid;
		result[i]["state"] = Citystate[i].state;
		result[i]["city_id"] = Citystate[i].city_id;
		result[i]["Challname"] = string(Citystate[i].Challname);
		result[i]["Challname0"] = string(Citystate[i].Challname0);
		result[i]["Challname1"] = string(Citystate[i].Challname1);
		result[i]["Occname"] = string(Citystate[i].Occname);
		result[i]["Occname0"] = string(Citystate[i].Occname0);
		result[i]["Occname1"] = string(Citystate[i].Occname1);
	}

	DataUser userdata;
	CLogicUser logicUser;
	ret = logicUser.GetUserLimit(uid, userdata);
	if (ret != 0)
		return ret;
	unsigned leader_uid2 = 0;
	int kingdom = 0;
	int level = 0;
	unsigned leader_uid = 0;

	if(0 != userdata.alliance_id)
	{
		DataAlliance alliancedata;
		CLogicAlliance getalldata;
		ret = getalldata.GetAlliance(userdata.alliance_id, alliancedata);
		if(ret != 0)
		{
			return ret;
		}

		vector<DataAllianceMember> members;
		CLogicAlliance logicAlliance;
		int ret = logicAlliance.GetMembers(userdata.alliance_id, members);
		if(ret != 0)
		{
			return ret;
		}

		for(vector<DataAllianceMember>::iterator itr=members.begin(); itr!=members.end(); itr++)
		{
			int type;
			type = (*itr).type;
			if(2 == type)
			{
				leader_uid2 = (*itr).uid;
				break;
			}
		}
		kingdom = alliancedata.kingdom;
		leader_uid = alliancedata.leader_uid;
		level = alliancedata.level;
	}
	result[CITY_NUM]["alliance_id"] = userdata.alliance_id;
	result[CITY_NUM]["kingdom"] = kingdom;
	result[CITY_NUM]["leader_uid"] = leader_uid;
	result[CITY_NUM]["level"] = level;
	result[CITY_NUM]["leader_uid2"] = leader_uid2;
	return 0;
}

int CLogicNationalWarRgt::UpdateCityState(int unsigned uid, Json::Value &input,Json::Value &result)
{
	CDataNationalWarRgt *pdata= GetNationalWarRgtData();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_nationalwar_rgt_fail");
	}
	CDataCitiesInState updateCityState;
	int ret = 0;

	CLogicAlliance getalldata;
	Json::Value &value = input["updatecity"];

	int city_id = 0;
	unsigned ChallAlliance_id = 0;
	unsigned OccAlliance_id = 0;
	unsigned chall_ts = Time::GetGlobalTime();
	unsigned Occ_ts = 0;
	int state = 0;
	int chall_state = 0;

	if( !Json::GetInt(value, "city_id", city_id)
	   || !Json::GetUInt(value, "ChallAlliance_id", ChallAlliance_id)
	   || !Json::GetUInt(value, "OccAlliance_id", OccAlliance_id)
	   || !Json::GetUInt(value, "occ_ts", Occ_ts)
	   || !Json::GetInt(value, "state", state)
	   || !Json::GetInt(value, "chall_state", chall_state)
	  )
	{
		return R_ERR_PARAM;
	}

	updateCityState.city_id = city_id;
	updateCityState.ChallAlliance_id = ChallAlliance_id;
//	updateCityState.Chall_InviteAlliance_id[0] = Chall_InviteAlliance_id_0;
//	updateCityState.Chall_InviteAlliance_id[1] = Chall_InviteAlliance_id_1;
	updateCityState.chall_uid = uid;
	updateCityState.OccAlliance_id = OccAlliance_id;
//	updateCityState.Occ_InviteAlliance_id[0] = Occ_InviteAlliance_id_0;
//	updateCityState.Occ_InviteAlliance_id[1] = Occ_InviteAlliance_id_1;
	updateCityState.Occ_ts = Occ_ts;
	updateCityState.chall_ts = chall_ts;
	updateCityState.state = state;
	updateCityState.chall_state = chall_state;

	if(updateCityState.ChallAlliance_id != 0)
	{
		DataAlliance alliancedata;
		ret = getalldata.GetAlliance(updateCityState.ChallAlliance_id, alliancedata);
		if(ret != 0)
		{
			return ret;
		}
		memcpy(updateCityState.Challname,alliancedata.name.c_str(),sizeof(updateCityState.Challname) - 1);
	}

	if(updateCityState.OccAlliance_id != 0)
	{
		DataAlliance alliancedata;
		ret = getalldata.GetAlliance(updateCityState.OccAlliance_id, alliancedata);
		if(ret != 0)
		{
			return ret;
		}
		memcpy(updateCityState.Occname,alliancedata.name.c_str(),sizeof(updateCityState.Occname) - 1);
	}

	int updatedone = 0;
	ret = pdata->UpdateCityState(updateCityState,updatedone);
	if(ret != 0)
	{
		return ret;
	}
	if(updatedone ==1)
	{
		result["updatedone"] = "Has_been_declared";
	}
	return 0;
}

int CLogicNationalWarRgt::InviteAlliance(int unsigned uid, Json::Value &input,Json::Value &result)
{
	CDataNationalWarRgt *pdata= GetNationalWarRgtData();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_nationalwar_rgt_fail");
	}
	CDataCitiesInState updateCityState;
	int ret = 0;

	CLogicAlliance getalldata;
	Json::Value &value = input["invitecity"];

	int city_id = 0;
	unsigned Chall_InviteAlliance_id_0 = 0;
	unsigned Chall_InviteAlliance_id_1 = 0;
	unsigned Occ_InviteAlliance_id_0 = 0;
	unsigned Occ_InviteAlliance_id_1 = 0;


	if(!Json::GetUInt(value, "Chall_InviteAlliance_id_0", Chall_InviteAlliance_id_0)
		|| !Json::GetInt(value, "city_id", city_id)
	 	|| !Json::GetUInt(value, "Chall_InviteAlliance_id_1", Chall_InviteAlliance_id_1)
	    || !Json::GetUInt(value, "Occ_InviteAlliance_id_0", Occ_InviteAlliance_id_0)
	    || !Json::GetUInt(value, "Occ_InviteAlliance_id_1", Occ_InviteAlliance_id_1))
	{
		return R_ERR_PARAM;
	}

	updateCityState.city_id = city_id;
	updateCityState.Chall_InviteAlliance_id[0] = Chall_InviteAlliance_id_0;
	updateCityState.Chall_InviteAlliance_id[1] = Chall_InviteAlliance_id_1;
	updateCityState.Occ_InviteAlliance_id[0] = Occ_InviteAlliance_id_0;
	updateCityState.Occ_InviteAlliance_id[1] = Occ_InviteAlliance_id_1;

	if(updateCityState.Chall_InviteAlliance_id[0] != 0)
	{
		DataAlliance alliancedata;
		ret = getalldata.GetAlliance(updateCityState.Chall_InviteAlliance_id[0], alliancedata);
		if(ret != 0)
		{
			return ret;
		}
		memcpy(updateCityState.Challname0,alliancedata.name.c_str(),sizeof(updateCityState.Challname0) - 1);
	}

	if(updateCityState.Chall_InviteAlliance_id[1] != 0)
	{
		DataAlliance alliancedata;
		ret = getalldata.GetAlliance(updateCityState.Chall_InviteAlliance_id[1], alliancedata);
		if(ret != 0)
		{
			return ret;
		}
		memcpy(updateCityState.Challname1,alliancedata.name.c_str(),sizeof(updateCityState.Challname1) - 1);
	}

	if(updateCityState.Occ_InviteAlliance_id[0] != 0)
	{
		DataAlliance alliancedata;
		ret = getalldata.GetAlliance(updateCityState.Occ_InviteAlliance_id[0], alliancedata);
		if(ret != 0)
		{
			return ret;
		}
		memcpy(updateCityState.Occname0,alliancedata.name.c_str(),sizeof(updateCityState.Occname0) - 1);
	}

	if(updateCityState.Occ_InviteAlliance_id[1] != 0)
	{
		DataAlliance alliancedata;
		ret = getalldata.GetAlliance(updateCityState.Occ_InviteAlliance_id[1], alliancedata);
		if(ret != 0)
		{
			return ret;
		}
		memcpy(updateCityState.Occname1,alliancedata.name.c_str(),sizeof(updateCityState.Occname1) - 1);
	}

	int invitedone = 0;
	ret = pdata->InviteSuccess(updateCityState,invitedone);
	if(ret != 0)
	{
		return ret;
	}
	if(invitedone ==1)
	{
		result["invitealliance"] = "Has_Full";
	}
	return 0;

}

int CLogicNationalWarRgt::CleanCityStateDate(void)
{
	CDataNationalWarRgt *pdata= GetNationalWarRgtData();
	if(pdata == NULL)
	{
		DB_ERROR_RETURN_MSG("init_nationalwar_rgt_fail");
	}
	return pdata->CleanNationalWarRgt();
}
