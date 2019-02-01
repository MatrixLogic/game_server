/*
 * LogicBaseExtra.cpp
 *
 *  Created on: 2012-10-25
 *      Author: Administrator
 */

#include "LogicBaseExtra.h"

CLogicBaseExtra::CLogicBaseExtra() {
	// TODO Auto-generated constructor stub

}

CLogicBaseExtra::~CLogicBaseExtra() {
	// TODO Auto-generated destructor stub
}

int CLogicBaseExtra::AddBaseExtra(unsigned uid, const DataBaseExtra &data)
{
	CDataBaseExtra baseExtra;
	int ret = baseExtra.AddBaseExtra(uid,data);
	if(ret != 0)
	{
		error_log("add base extra data failed,uid = %u,worldpos = %u",uid,data.worldpos);
		DB_ERROR_RETURN_MSG("add_base_extra_fail");

	}

	return 0;
}
int CLogicBaseExtra::GetBaseExtra(unsigned uid, unsigned worldpos,  DataBaseExtra &data)
{
	CDataBaseExtra baseExtra;
	int ret = baseExtra.GetBaseExtra(uid,worldpos,data);
	return ret;
}
int CLogicBaseExtra::GetBaseExtraOnType(unsigned uid,int type, vector<DataBaseExtra> &data)
{
	CDataBaseExtra baseExtra;
	int ret = baseExtra.GetBaseExtraOnType(uid,type,data);
	if(!(ret == 0 || ret == R_ERR_NO_DATA))
	{
		error_log("get base extra data failed,uid = %u,type = %d",uid,type);
		DB_ERROR_RETURN_MSG("get_base_extra_fail");

	}
	return 0;
}
int CLogicBaseExtra::GetBaseExtra(unsigned uid, vector<DataBaseExtra> &data)
{
	CDataBaseExtra baseExtra;
	int ret = baseExtra.GetBaseExtra(uid,data);
	if(!(ret == 0 || ret == R_ERR_NO_DATA))
	{
		error_log("get base extra data failed,uid = %u",uid);
		DB_ERROR_RETURN_MSG("get_base_extra_fail");

	}
	return 0;
}
int CLogicBaseExtra::SetBaseExtra(unsigned uid, unsigned worldpos,const DataBaseExtra &data)
{
	CDataBaseExtra baseExtra;
	int ret = baseExtra.SetBaseExtra(uid,worldpos,data);
	if(ret != 0 )
	{
		error_log("set base extra data failed,uid = %u,worldpos = %u",uid,worldpos);
		DB_ERROR_RETURN_MSG("set_base_extra_fail");

	}

	return 0;
}
//int CLogicBaseExtra::SetStationeHeros(unsigned uid, unsigned worldpos,const string &heros)
//{
//	CDataBaseExtra baseExtra;
//	int ret = baseExtra.SetStationeHeros(uid,worldpos,heros);
//	if(ret != 0 )
//	{
//		error_log("set stationed heros failed,uid = %u,worldpos = %u",uid,worldpos);
//		DB_ERROR_RETURN_MSG("set_stationed_heros_fail");
//
//	}
//
//	return 0;
//}
//int CLogicBaseExtra::GetStationeHeros(unsigned uid, unsigned worldpos,string &heros)
//{
//	CDataBaseExtra baseExtra;
//	int ret = baseExtra.GetStationeHeros(uid,worldpos,heros);
//	if(ret != 0 )
//	{
//		error_log("Get stationed heros failed,uid = %u,worldpos = %u",uid,worldpos);
//		DB_ERROR_RETURN_MSG("get_stationed_heros_fail");
//
//	}
//
//	return 0;
//}
//int CLogicBaseExtra::GetStationedSoldiers(unsigned uid, unsigned worldpos,string &soldiers)
//{
//	CDataBaseExtra baseExtra;
//	int ret = baseExtra.GetStationedSoldiers(uid,worldpos,soldiers);
//	if(ret != 0 )
//	{
//		error_log("Get stationed soldiers failed,uid = %u,worldpos = %u",uid,worldpos);
//		DB_ERROR_RETURN_MSG("get_stationed_soldiers_fail");
//
//	}
//
//	return 0;
//}
//int CLogicBaseExtra::SetStationedSoldiers(unsigned uid, unsigned worldpos,const string &soldiers)
//{
//	CDataBaseExtra baseExtra;
//	int ret = baseExtra.SetStationedSoldiers(uid,worldpos,soldiers);
//	if(ret != 0 )
//	{
//		error_log("set stationed soldiers failed,uid = %u,worldpos = %u",uid,worldpos);
//		DB_ERROR_RETURN_MSG("set_stationed_soldiers_fail");
//
//	}
//
//	return 0;
//}
int CLogicBaseExtra::GetWorldPosList(unsigned uid,Json::Value &worldposList)
{
	CDataBaseExtra dbBaseExtra;
	vector<DataBaseExtra> vWpList;
	int ret = dbBaseExtra.GetWorldposList(uid,vWpList);
	if(!(ret != 0 || ret != R_ERR_NO_DATA))
	{
		return ret;
	}
	worldposList.resize(vWpList.size());
	for(int i = 0; i < vWpList.size();i++)
	{
		worldposList[i][0u] = vWpList[i].worldpos;
		worldposList[i][1u] = vWpList[i].type;
	}
	return 0;
}


int CLogicBaseExtra::GetSubBaseInfoJson(unsigned uid,Json::Value &worldres)
{
	CDataBaseExtra dbBaseExtra;
	vector<DataBaseExtra> vSubBases;
	Json::Reader reader;
	int iIndex = 0;
	int ret = dbBaseExtra.GetSubBaseInfo(uid,vSubBases);
	if(!(ret != 0 || ret != R_ERR_NO_DATA))
	{
		error_log("get subbase resource failed, uid=%u",uid);
		return ret;
	}
	worldres.resize(vSubBases.size());
	for(int i = 0; i < worldres.size();i++)
	{
//		if(vSubBases[i].resource.length() == 0)
//			continue;
		if(reader.parse(vSubBases[i].resource,worldres[(unsigned)iIndex]["m"]) && worldres[(unsigned)iIndex]["m"].isArray())
		{
			worldres[(unsigned)iIndex]["world_pos"] = vSubBases[i].worldpos;
			iIndex++;
		}
	}
	return 0;
}


int CLogicBaseExtra::SetSubBaseResource(unsigned uid, unsigned worldpos,string const &resource)
{
	CDataBaseExtra dbBaseExtra;
	int ret = dbBaseExtra.SetSubBaseResource(uid,worldpos,resource);
	if(0 != ret)
	{
		error_log("Set subbase resource failed uid= %u,worldpos = %u",uid,worldpos);
		return ret;
	}
	return 0;
}
int CLogicBaseExtra::GetProtectedTime(unsigned uid,unsigned worldpos,unsigned &protectedTime)
{
	CDataBaseExtra dbBaseExtra;
	int ret = dbBaseExtra.GetProtectedTime(uid,worldpos,protectedTime);
	if(0 != ret)
	{
		error_log("Get subbase protected time failed uid= %u,worldpos = %u",uid,worldpos);
		return ret;
	}
	return 0;
}
int CLogicBaseExtra::UpdateProtectedTime(unsigned uid,unsigned worldpos,unsigned const protectedTime)
{
	CDataBaseExtra dbBaseExtra;
	int ret = dbBaseExtra.UpdateProtectedTime(uid,worldpos,protectedTime);
	if(0 != ret)
	{
		error_log("Set subbase protected time failed uid= %u,worldpos = %u",uid,worldpos);
		return ret;
	}
	return 0;
}

//int CLogicBaseExtra::UpdateLastBreathTime(unsigned uid,unsigned worldpos,unsigned const last_save_time)
//{
//	CDataBaseExtra dbBaseExtra;
//	int ret = dbBaseExtra.UpdateLastBreathTime(uid,worldpos,last_save_time);
//	if(0 != ret)
//	{
//		error_log("Set subbase protected time failed uid= %u,worldpos = %u",uid,worldpos);
//		return ret;
//	}
//	return 0;
//}

int CLogicBaseExtra::UpdateBreathInfo(unsigned uid,unsigned worldpos, const DataBreathInfo &breathInfo)
{
	CDataBaseExtra dbBaseExtra;
	int ret = dbBaseExtra.UpdateBreathInfo(uid,worldpos,breathInfo);
	if(0 != ret)
	{
		error_log("Set subbase protected time failed uid= %u,worldpos = %u",uid,worldpos);
		return ret;
	}
	return 0;
}
int CLogicBaseExtra::GetLastBreathTime(unsigned uid,unsigned worldpos,unsigned &last_breath_time)
{
	CDataBaseExtra dbBaseExtra;
	int ret = dbBaseExtra.GetLastBreathTime(uid,worldpos,last_breath_time);
	if(0 != ret)
	{
		error_log("Get subbase protected time failed uid= %u,worldpos = %u",uid,worldpos);
		return ret;
	}
	return 0;
}
int CLogicBaseExtra::RemoveSubBase(unsigned uid, unsigned worldpos)
{
	CDataBaseExtra dbBaseExtra;
	int ret = dbBaseExtra.RemoveSubBase(uid,worldpos);
	if(0 != ret)
	{
		error_log("Remove subbase failed uid= %u,worldpos = %u",uid,worldpos);
		return ret;
	}
	return 0;
}

