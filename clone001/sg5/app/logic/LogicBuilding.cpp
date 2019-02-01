#include "LogicBuilding.h"

int CLogicBuilding::InitializeBuilding(unsigned uid)
{
	const Json::Value *pInitJson = 0;
	int ret = CLogicArchive::GetInitArchive(pInitJson);
	if (ret != 0)
		return ret;
	if (!pInitJson->isMember("baseop"))
	{
		return 0;
	}
	const Json::Value &baseop = (*pInitJson)["baseop"];
	if (!baseop.isArray())
	{
		error_log("[init building error][uid=%u]",uid);
		DB_ERROR_RETURN_MSG("init_building_error");
	}

	Json::FastWriter writer;
	CDataBuilding bdDB;
	unsigned size = baseop.size();
	for (unsigned i = 0; i < size; i++)
	{
		unsigned id = 0;
		if (!Json::GetUInt(baseop[i], "id", id))
		{
			error_log("[init building error][uid=%u,index=%u]",uid,i);
			DB_ERROR_RETURN_MSG("init_building_error");
		}
		ret = bdDB.AddBuilding(uid, id, writer.write(baseop[i]));
		if (ret != 0)
		{
			error_log("[AddBuilding fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
			DB_ERROR_RETURN_MSG("add_building_fail");
		}
	}
	return 0;
}


int CLogicBuilding::InitializeBuilding(unsigned uid,unsigned worldpos)
{
	const Json::Value *pInitJson = 0;
	int ret = CLogicArchive::GetInitArchive(pInitJson);
	if (ret != 0)
		return ret;
	if (!pInitJson->isMember("baseop"))
	{
		return 0;
	}
	const Json::Value &baseop = (*pInitJson)["baseop"];
	if (!baseop.isArray())
	{
		error_log("[init building error][uid=%u]",uid);
		DB_ERROR_RETURN_MSG("init_building_error");
	}

	Json::FastWriter writer;
	CDataBuilding bdDB;
	unsigned size = baseop.size();
	for (unsigned i = 0; i < size; i++)
	{
		unsigned id = 0;
		if (!Json::GetUInt(baseop[i], "id", id))
		{
			error_log("[init building error][uid=%u,index=%u]",uid,i);
			DB_ERROR_RETURN_MSG("init_building_error");
		}
		ret = bdDB.AddBuilding(uid, id, worldpos,writer.write(baseop[i]));
		if (ret != 0)
		{
			error_log("[AddBuilding fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
			DB_ERROR_RETURN_MSG("add_building_fail");
		}
	}
	return 0;
}
int CLogicBuilding::GetBuilding(unsigned uid, Json::Value &data)
{
	CDataBuilding bdDB;
	vector<DataBuilding> datas;
	int ret = bdDB.GetBuilding(uid, datas);
	if (ret != 0 && ret != R_ERR_NO_DATA)
	{
		error_log("[GetBuilding fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("get_building_fail");
	}
	if (ret == R_ERR_NO_DATA)
	{
		data.resize(0);
		return 0;
	}
	Json::Reader reader;
	data.resize(datas.size());
	for (unsigned i = 0; i < datas.size(); i++)
	{
		if (!reader.parse(datas[i].data, data[i]))
		{
			error_log("[parse fail][uid=%u,id=%d,i=%u]",uid,datas[i].id,i);
		}
	}
	return 0;
}


int CLogicBuilding::GetBuilding(unsigned uid, unsigned worldpos,Json::Value &data,bool isMainBase)
{
	CDataBuilding bdDB;
	vector<DataBuilding> olddatas;
	vector<DataBuilding> newdatas;
//	int ret = bdDB.GetBuilding(uid,worldpos, newdatas);
	int ret = 0;
	if(isMainBase)
	{
		ret = bdDB.GetBuilding(uid,0, olddatas);

		if (ret != 0 && ret != R_ERR_NO_DATA)
		{
			error_log("[GetBuilding fail][uid=%u,ret=%d]",uid,ret);
			DB_ERROR_RETURN_MSG("get_building_fail");
		}
	}else{
		ret = bdDB.GetBuilding(uid,worldpos, olddatas);

		if (ret != 0 && ret != R_ERR_NO_DATA)
		{
			error_log("[GetBuilding fail][uid=%u,ret=%d]",uid,ret);
			DB_ERROR_RETURN_MSG("get_building_fail");
		}
	}
	if (ret == R_ERR_NO_DATA)
	{
		data.resize(0);
		return 0;
	}
	Json::Reader reader;
	int iCount = olddatas.size();
//	int newCount = newdatas.size();
//	int iCount = oldCount + newCount;
//	data.resize(iCount);
//
	for (unsigned i = 0; i < iCount; i++)
	{
//		if(i < newCount)
//		{
			if (!reader.parse(olddatas[i].data, data[i]))
			{
				error_log("[parse fail][uid=%u,id=%d,i=%u]",uid,olddatas[i].id,i);
			}
//			continue;
//		}else{
//				if (!reader.parse(olddatas[i - newCount].data, data[i]))
//				{
//					error_log("[parse fail][uid=%u,id=%d,i=%u]",uid,olddatas[i - newCount].id,i);
//				}
//		}
//
	}

	return 0;
}
int CLogicBuilding::UpdateBuilding(unsigned uid, unsigned worldpos,
		const Json::Value &data, bool isMainBase) {
	if (!data.isArray())
	{
		error_log("[building type error][uid=%u,type=%d]", uid, data.type());
		DATA_ERROR_RETURN_MSG("data_building_error");
	}
	int ret = 0;
	Json::FastWriter writer;
	Json::Reader reader;
	CDataBuilding bdDB;
	map<unsigned, string> oldBuildings;
	if(isMainBase)
	{
		ret = bdDB.GetBuilding(uid, 0,oldBuildings);
	}else
	{
		ret = bdDB.GetBuilding(uid, worldpos,oldBuildings);
	}
	if (!(ret == 0 || ret == R_ERR_NO_DATA))
	{
		error_log("[GetBuilding fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("get_building_fail");
	}
	map<unsigned, string>::const_iterator oldItr;
	for (unsigned i = 0; i < data.size(); i++)
	{
		unsigned id = 0;
		int t = 0;
		if (!Json::GetUInt(data[i], "id", id) || !Json::GetInt(data[i], "t", t))
		{
			error_log("[building data error][uid=%u,index=%u,id=%u]",uid,i,id);
			DATA_ERROR_RETURN_MSG("data_building_error");
		}

		if(isMainBase)
		{
			oldItr = oldBuildings.find(id);
			if (oldItr != oldBuildings.end())
			{
				Json::Value oldBd;
				int oldt = 0;
				if (reader.parse(oldItr->second, oldBd) && Json::GetInt(oldBd, "t", oldt))
				{
					if (t != oldt)
					{
						error_log("[building type error][uid=%u,id=%u,t=%d,oldt=%d]",uid,id,t,oldt);
						DATA_ERROR_RETURN_MSG("building_type_error");
					}
					int l = 0, oldl = 0;
					Json::GetInt(data[i], "l", l);
					Json::GetInt(oldBd, "l", oldl);
					if (l < oldl)
					{
						error_log("[building level error][uid=%u,id=%u,l=%d,oldl=%d]",uid,id,l,oldl);
						DATA_ERROR_RETURN_MSG("building_level_error");
					}
				}
			}
			else
			{
				int l = 0;
				Json::GetInt(data[i], "l", l);
				if(l > 2)
				{
					error_log("[add building level error][uid=%u,id=%u,l=%d]",uid,id,l);
					DATA_ERROR_RETURN_MSG("add_building_level_error");
				}
			}
		}

		if (data[i].isMember("status")
				&& data[i]["status"].isIntegral()
				&& data[i]["status"].asInt() == 9)
		{
			if(isMainBase)
			{
				ret = bdDB.RemoveBuilding(uid, id,0);
				if (ret != 0)
				{
					error_log("[RemoveBuilding fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
					DB_ERROR_RETURN_MSG("rm_building_fail");
				}
			}
			ret = bdDB.RemoveBuilding(uid, id,worldpos);
			if (ret != 0)
			{
				error_log("[RemoveBuilding fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
				DB_ERROR_RETURN_MSG("rm_building_fail");
			}
			//info_log("[RemoveBuilding] [uid=%u,id=%u worldpos=%u]",uid,id,worldpos);
		}
		else
		{
			if(isMainBase)
			{
				ret = bdDB.ReplaceBuilding(uid, id, 0, writer.write(data[i]));
			}else
			{
				ret = bdDB.ReplaceBuilding(uid, id, worldpos, writer.write(data[i]));
			}
			if (ret != 0)
			{
				error_log("[ReplaceBuilding fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
				DB_ERROR_RETURN_MSG("set_building_fail");
			}
			//info_log("[ReplaceBuilding] [uid=%u,id=%u worldpos=%u]",uid,id,worldpos);

		}
	}
	return 0;
}

int CLogicBuilding::ImportBuilding(unsigned uid, unsigned worldpos,
		const Json::Value &data, bool isMainBase) {
	if (!data.isArray())
	{
		error_log("[building type error][uid=%u,type=%d]", uid, data.type());
		DATA_ERROR_RETURN_MSG("data_building_error");
	}
	int ret = 0;
	Json::FastWriter writer;
	Json::Reader reader;
	CDataBuilding bdDB;
	map<unsigned, string> oldBuildings;
	if(isMainBase)
	{
		ret = bdDB.GetBuilding(uid, 0,oldBuildings);
	}else
	{
		ret = bdDB.GetBuilding(uid, worldpos,oldBuildings);
	}
	if (!(ret == 0 || ret == R_ERR_NO_DATA))
	{
		error_log("[GetBuilding fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("get_building_fail");
	}
	map<unsigned, string>::const_iterator oldItr;
	for (unsigned i = 0; i < data.size(); i++)
	{
		unsigned id = 0;
		int t = 0;
		if (!Json::GetUInt(data[i], "id", id) || !Json::GetInt(data[i], "t", t))
		{
			error_log("[building data error][uid=%u,index=%u,id=%u]",uid,i,id);
			DATA_ERROR_RETURN_MSG("data_building_error");
		}

		if (data[i].isMember("status")
				&& data[i]["status"].isIntegral()
				&& data[i]["status"].asInt() == 9)
		{
			if(isMainBase)
			{
				ret = bdDB.RemoveBuilding(uid, id,0);
				if (ret != 0)
				{
					error_log("[RemoveBuilding fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
					DB_ERROR_RETURN_MSG("rm_building_fail");
				}
			}
			ret = bdDB.RemoveBuilding(uid, id,worldpos);
			if (ret != 0)
			{
				error_log("[RemoveBuilding fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
				DB_ERROR_RETURN_MSG("rm_building_fail");
			}
			//info_log("[RemoveBuilding] [uid=%u,id=%u worldpos=%u]",uid,id,worldpos);
		}
		else
		{
			if(isMainBase)
			{
				ret = bdDB.ReplaceBuilding(uid, id, 0, writer.write(data[i]));
			}else
			{
				ret = bdDB.ReplaceBuilding(uid, id, worldpos, writer.write(data[i]));
			}
			if (ret != 0)
			{
				error_log("[ReplaceBuilding fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
				DB_ERROR_RETURN_MSG("set_building_fail");
			}
			//info_log("[ReplaceBuilding] [uid=%u,id=%u worldpos=%u]",uid,id,worldpos);

		}
	}
	return 0;
}

int CLogicBuilding::GetBuilding(unsigned uid, unsigned id, Json::Value &data)
{
	string sdata;
	CDataBuilding bdDB;
	int ret = bdDB.GetBuilding(uid, id, sdata);
	if (ret != 0)
	{
		error_log("[GetBuilding fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
		DB_ERROR_RETURN_MSG("get_building_fail");
	}
	Json::Reader reader;
	if (!reader.parse(sdata, data))
	{
		error_log("[building error][uid=%u,id=%u]",uid,id);
		DATA_ERROR_RETURN_MSG("data_building_error");
	}
	return 0;
}
int CLogicBuilding::GetBuilding(unsigned uid, unsigned id, unsigned worldpos,Json::Value &data,bool isMainBase)
{
	string sdata;
	CDataBuilding bdDB;
	int ret = 0;
	if(isMainBase)
	{
		ret = bdDB.GetBuilding(uid, id, 0,sdata);
	}else
	{
		ret = bdDB.GetBuilding(uid, id, worldpos,sdata);
	}
	if (ret != 0)
	{
		error_log("[GetBuilding fail][uid=%u,id=%u,ret=%d]",uid,id,ret);
		DB_ERROR_RETURN_MSG("get_building_fail");
	}
	Json::Reader reader;
	if (!reader.parse(sdata, data))
	{
		error_log("[building error][uid=%u,id=%u]",uid,id);
		DATA_ERROR_RETURN_MSG("data_building_error");
	}
	return 0;
}

int CLogicBuilding::RemoveBuilding(unsigned uid, unsigned worldpos)
{

	int ret = 0;
	Json::FastWriter writer;
	Json::Reader reader;
	CDataBuilding bdDB;
	ret = bdDB.RemoveBuildingOnWordPos(uid,worldpos);
	if (!(ret == 0 || ret == R_ERR_NO_DATA))
	{
		error_log("[RemoveBuilding fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("get_building_fail");
	}
	info_log("[RemoveBuilding] [uid=%u,id=%u]");

	return 0;
}

