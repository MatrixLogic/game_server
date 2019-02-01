#include "LogicInstance.h"

struct INSTANCE_Archive
{
	unsigned updateTime;
	Json::Value data;
};

int CLogicInstance::GetInstanceArchive(unsigned id, Json::Value &data)
{
	static map<unsigned, INSTANCE_Archive> instanceMap;
	map<unsigned, INSTANCE_Archive>::iterator it = instanceMap.find(id);
	if (it == instanceMap.end() || Time::GetGlobalTime() - (it->second).updateTime > 1800)
	{
		string path = Config::GetValue(CONFIG_INSTANCE_DATA);
		if (path.empty())
		{
			error_log("[instance config error][id=%u]",id);
			DB_ERROR_RETURN_MSG("init_instance_fail");
		}
		if (path[path.length()-1] == '/')
		{
			path.append(CTrans::UTOS(id)).append(".dat");
		}
		else
		{
			path.append("/").append(CTrans::UTOS(id)).append(".dat");
		}
		string sdata;
		int result = File::Read(path, sdata);
		if(result != 0)
		{
			error_log("[init instance fail][ret=%d,path=%s]", result, path.c_str());
			DB_ERROR_RETURN_MSG("init_instance_fail");
		}
		Json::Reader reader;
		if (it == instanceMap.end())
		{
			INSTANCE_Archive archive;
			if (!reader.parse(sdata, archive.data))
			{
				error_log("[parse json fail][path=%s]", path.c_str());
				DB_ERROR_RETURN_MSG("init_instance_fail");
			}
			archive.updateTime = Time::GetGlobalTime();
			instanceMap[id] = archive;
		}
		else
		{
			if (!reader.parse(sdata, (it->second).data))
			{
				error_log("[parse json fail][path=%s]", path.c_str());
				DB_ERROR_RETURN_MSG("init_instance_fail");
			}
			(it->second).updateTime = Time::GetGlobalTime();
		}
	}
	data = instanceMap[id].data;
	return 0;
}

int CLogicInstance::GetInstanceInfoJson(unsigned uid, Json::Value &data)
{
	//CDataInstance instanceDb;
	vector<DataInstance> datas;
	int ret = GetInstanceLimit(uid, datas);
	if (ret != 0)
	{
		error_log("[GetInstanceLimit fail][uid=%u,ret=%d]",uid,ret);
		DB_ERROR_RETURN_MSG("get_instance_fail");
	}
	data.resize(datas.size());
	for (unsigned i = 0; i < datas.size(); i++)
	{
		data[i]["id"] = datas[i].id;
		data[i]["win"] = datas[i].win;
		data[i]["destroy"] = datas[i].damage;
		if (CTime::GetDayInterval(datas[i].lastts, Time::GetGlobalTime()) > 0)
		{
			data[i]["count"] = 0;
		}
		else
		{
			data[i]["count"] = datas[i].count;
		}
	}
	return 0;
}

int CLogicInstance::Load(unsigned instanceId, unsigned uidBy, bool restart, Json::Value &result)
{
	if (!IsValidInstanceId(instanceId))
	{
		REFUSE_RETURN_MSG("invalid_instance_id");
	}
	//CDataInstance instanceDb;
	DataInstance instance;
	int ret;
	if (restart)
	{
		ret = GetInstanceArchive(instanceId, result);
		if (ret != 0)
			return ret;
		ret = GetInstanceLimit(uidBy, instanceId, instance);
		if (ret != 0 && ret != R_ERR_NO_DATA)
		{
			error_log("[GetInstance fail][uid=%u,instanceId=%u,ret=%d]",uidBy,instanceId,ret);
			DB_ERROR_RETURN_MSG("get_instance_fail");
		}
		Json::FastWriter writer;
		if (ret == R_ERR_NO_DATA)
		{
			instance.uid = uidBy;
			instance.id = instanceId;
			instance.count = 1;
			instance.lastts = Time::GetGlobalTime();
			instance.damage = 0;
			instance.win = 0;
			instance.data = writer.write(result);
			ret = AddInstance(instance);
			if (ret != 0)
			{
				error_log("[AddInstance fail][uid=%u,instanceId=%u,ret=%d]",uidBy,instanceId,ret);
				DB_ERROR_RETURN_MSG("add_instance_fail");
			}
		}
		else
		{
			if (CTime::GetDayInterval(instance.lastts, Time::GetGlobalTime()) > 0)
			{
				instance.count = 0;
			}
			instance.count++;
			instance.lastts = Time::GetGlobalTime();
			instance.damage = 0;
			instance.data = writer.write(result);
			ret = SetInstance(uidBy, instanceId, instance);
			if (ret != 0)
			{
				error_log("[SetInstance fail][uid=%u,instanceId=%u,ret=%d]",uidBy,instanceId,ret);
				DB_ERROR_RETURN_MSG("set_instance_fail");
			}
		}
	}
	else
	{
		ret = GetInstance(uidBy, instanceId, instance);
		if (ret != 0)
		{
			error_log("[GetInstance fail][uid=%u,instanceId=%u,ret=%d]",uidBy,instanceId,ret);
			DB_ERROR_RETURN_MSG("get_instance_fail");
		}
		if (CTime::GetDayInterval(instance.lastts, Time::GetGlobalTime()) > 0)
		{
			instance.count = 0;
		}
		instance.count++;
		instance.lastts = Time::GetGlobalTime();
		ret = SetInstanceLimit(uidBy, instanceId, instance);
		if (ret != 0)
		{
			error_log("[SetInstanceLimit fail][uid=%u,instanceId=%u,ret=%d]",uidBy,instanceId,ret);
			DB_ERROR_RETURN_MSG("set_instance_fail");
		}
		Json::Reader reader;
		if (!reader.parse(instance.data, result))
		{
			error_log("[parse instance fail][uid=%u,instanceId=%u]",uidBy,instanceId);
			DB_ERROR_RETURN_MSG("parse_instance_fail");
		}
	}
	result["userid"] = instanceId;
	return 0;
}

int CLogicInstance::Save(unsigned instanceId, DataUser &userBy, Json::Value &data, Json::Value &result)
{
	//CDataInstance instanceDb;
	DataInstance instance;
	int ret = GetInstance(userBy.uid, instanceId, instance);
	if (ret != 0)
	{
		error_log("[GetInstance fail][uid=%u,instanceId=%u,ret=%d]",userBy.uid,instanceId,ret);
		DB_ERROR_RETURN_MSG("get_instance_fail");
	}
	Json::Value archive;
	Json::Reader reader;
	if (!instance.data.empty() && !reader.parse(instance.data, archive))
	{
		error_log("[parse instance fail][uid=%u,instanceId=%u]",userBy.uid,instanceId);
		DB_ERROR_RETURN_MSG("parse_instance_fail");
	}

	int destroy = 0;
	Json::GetInt(data, "destroy", destroy);
	archive["lasttime"] = Time::GetGlobalTime();
	if (data.isMember("tech")) archive["tech"] = data["tech"];
	if (data.isMember("barrackQ")) archive["barrackQ"] = data["barrackQ"];
	if (data.isMember("soldier")) archive["soldier"] = data["soldier"];
	if (data.isMember("soldierlevel")) archive["soldierlevel"] = data["soldierlevel"];
	if (data.isMember("buildQ")) archive["buildQ"] = data["buildQ"];
	if (data.isMember("skillQ")) archive["skillQ"] = data["skillQ"];
	if (data.isMember("trainQ")) archive["trainQ"] = data["trainQ"];
	if (Json::IsArray(data, "baseop"))
	{
		const Json::Value &baseop = data["baseop"];
		Json::Value &baseopOld = archive["baseop"];
		unsigned size = baseop.size();
		unsigned oldSize = baseopOld.size();
		for (unsigned i = 0; i < size; i++)
		{
			if (!baseop[i].isMember("id") || !baseop[i]["id"].isIntegral())
				continue;
			unsigned tempId = baseop[i]["id"].asUInt();
			for (unsigned j = 0; j < oldSize; j++)
			{
				if (baseopOld[j].get("id", 0).asUInt() == tempId)
				{
					baseopOld[j] = baseop[i];
				}
			}
		}
	}
	if (Json::IsArray(data, "hero"))
	{
		const Json::Value &hero = data["hero"];
		Json::Value &heroOld = archive["hero"];
		unsigned size = hero.size();
		unsigned oldSize = heroOld.size();
		for (unsigned i = 0; i < size; i++)
		{
			if (!hero[i].isMember("ud") || !hero[i]["ud"].isIntegral())
				continue;
			unsigned tempId = hero[i]["ud"].asUInt();
			for (unsigned j = 0; j < oldSize; j++)
			{
				if (heroOld[j].get("ud", 0).asUInt() == tempId)
				{
					heroOld[j] = hero[i];
				}
			}
		}
	}
	if (Json::IsArray(data, "equipment"))
	{
		const Json::Value &equipment = data["equipment"];
		Json::Value &equipmentOld = archive["equipment"];
		unsigned size = equipment.size();
		unsigned oldSize = equipmentOld.size();
		for (unsigned i = 0; i < size; i++)
		{
			if (!equipment[i].isMember("ud") || !equipment[i]["ud"].isIntegral())
				continue;
			unsigned tempId = equipment[i]["ud"].asUInt();
			for (unsigned j = 0; j < oldSize; j++)
			{
				if (equipmentOld[j].get("ud", 0).asUInt() == tempId)
				{
					equipmentOld[j] = equipment[i];
				}
			}
		}
	}
	if (instance.win != 1 && destroy == 1)
		instance.win = 1;
	instance.damage = destroy;
	Json::FastWriter writer;
	instance.data = writer.write(archive);
	ret = SetInstance(userBy.uid, instanceId, instance);
	if (ret != 0)
	{
		error_log("[SetInstance fail][uid=%u,instanceId=%u,ret=%d]",userBy.uid,instanceId,ret);
		DB_ERROR_RETURN_MSG("set_instance_fail");
	}

	if (Json::IsObject(data, "attackinfo"))
	{
		CLogicArchive logicArchive;
		ret = logicArchive.ProcessAttackInfo(userBy.uid, data["attackinfo"]);
		if (ret != 0)
			return ret;
	}

	return 0;
}

int CLogicInstance::ExportInstanceData(unsigned uid, unsigned instanceId, const string &dir)
{
	string file(dir);
	if (file[file.length()-1] != '/')
		file.append("/");
	file.append(CTrans::UTOS(instanceId)).append(".dat");

	Json::FastWriter writer;
	Json::Value result;

	int ret = GetMinInstance(uid, result);
	if (ret != 0)
		return ret;

	result["userid"] = instanceId;

	string sdata = writer.write(result);
	ret = File::Write(file, sdata);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("write file fail");
	}

	return 0;
}

int CLogicInstance::GetMinInstance(unsigned uid, Json::Value &data)
{
	Json::Reader reader;

	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUser(uid, user);
	if (ret != 0)
		return ret;
	data["lasttime"] = Time::GetGlobalTime();
	data["world_pos"]["world_pos"] = user.mainpos;
	data["world_pos"]["type"] = USER_MAIN_CITY;
	if (!user.barrackQ.empty() && !reader.parse(user.barrackQ, data["barrackQ"]))
	{
		DB_ERROR_RETURN_MSG("parse_brrackQ_fail");
	}
	if (!user.soldier.empty() && !reader.parse(user.soldier, data["soldier"]))
	{
		DB_ERROR_RETURN_MSG("parse_soldier_fail");
	}
	if (!user.soldierlevel.empty() && !reader.parse(user.soldierlevel, data["soldierlevel"]))
	{
		DB_ERROR_RETURN_MSG("parse_soldierlevel_fail");
	}
	if (!user.user_tech.empty() && !reader.parse(user.user_tech, data["tech"]))
	{
		DB_ERROR_RETURN_MSG("parse_user_tech_fail");
	}
	if (!user.buildQ.empty() && !reader.parse(user.buildQ, data["buildQ"]))
	{
		error_log("[buildQ_parse_error][uid=%u]",uid);
	}
	if (!user.skillQ.empty() && !reader.parse(user.skillQ, data["skillQ"]))
	{
		error_log("[skillQ_parse_error][uid=%u]",uid);
	}
	if (!user.trainQ.empty() && !reader.parse(user.trainQ, data["trainQ"]))
	{
		error_log("[trainQ_parse_error][uid=%u]",uid);
	}

	CLogicBuilding logicBuiding;
	ret = logicBuiding.GetBuilding(uid,0, data["baseop"],false);
	if (ret != 0)
		return ret;

	CLogicHero logicHero;
	//ret = logicHero.GetHero(uid, data["hero"]);
	ret = logicHero.GetHeroUsed(uid, data["hero"]);
	if (ret != 0)
		return ret;

	CLogicEquipment logicEquip;
	ret = logicEquip.GetEquipmentUsed(uid, data["equipment"]);
	if (ret != 0)
		return ret;

	return 0;
}
int CLogicInstance::ReplaceInstance(const DataInstance &instance)
{
	if (IsValidUid(instance.uid))
	{
		CDataInstance instanceDb;
		int ret = instanceDb.ReplaceInstance(instance);
		if (ret != 0)
		{
			error_log("[ReplaceInstance fail][uid=%u,instanceId=%u,ret=%d]",instance.uid,instance.id,ret);
			return ret;
		}
		return 0;
	}
	else if(BASEMATCH_INST_UID == instance.uid || PERSONMATCH_INST_UID == instance.uid)
	{
		CDataMatchInstance instanceDb;
		int ret = instanceDb.ReplaceInstance(instance);
		if (ret != 0)
		{
			error_log("[ReplaceInstance fail][uid=%u,instanceId=%u,ret=%d]",instance.uid,instance.id,ret);
			return ret;
		}
		return 0;
	}
	return R_ERR_DATA;
}

int CLogicInstance::AddInstance(const DataInstance &data)
{
	if (IsValidUid(data.uid))
	{
		CDataInstance instanceDb;
		int ret = instanceDb.AddInstance(data);
		if (ret != 0)
		{
			error_log("[AddInstance fail][uid=%u,instanceId=%u,ret=%d]",data.uid,data.id,ret);
			return ret;
		}
		return 0;
	}
	else if(BASEMATCH_INST_UID == data.uid || PERSONMATCH_INST_UID == data.uid)
	{
		CDataMatchInstance instanceDb;
		int ret = instanceDb.AddInstance(data);
		if (ret != 0)
		{
			error_log("[AddInstance fail][uid=%u,instanceId=%u,ret=%d]",data.uid,data.id,ret);
			return ret;
		}
		return 0;
	}
	return R_ERR_DATA;
}
int CLogicInstance::SetInstance(unsigned uid, unsigned id, const DataInstance &data)
{
	if (IsValidUid(uid))
	{
		CDataInstance instanceDb;
		int ret = instanceDb.SetInstance(uid,id,data);
		if (ret != 0)
		{
			error_log("[SetInstance fail][uid=%u,instanceId=%u,ret=%d]",uid,id,ret);
			return ret;
		}
		return 0;
	}
	else if(BASEMATCH_INST_UID == uid || PERSONMATCH_INST_UID == uid)
	{
		CDataMatchInstance instanceDb;
		int ret = instanceDb.SetInstance(uid,id,data);
		if (ret != 0)
		{
			error_log("[SetInstance fail][uid=%u,instanceId=%u,ret=%d]",uid,id,ret);
			return ret;
		}
		return 0;
	}
	return R_ERR_DATA;
}
int CLogicInstance::GetInstance(unsigned uid, unsigned id, DataInstance &data)
{
	if (IsValidUid(uid))
	{
		CDataInstance instanceDb;
		int ret = instanceDb.GetInstance(uid,id,data);
		if (ret != 0 && ret != R_ERR_NO_DATA)
		{
			error_log("[GetInstance fail][uid=%u,instanceId=%u,ret=%d]",uid,id,ret);
			return ret;
		}
		return 0;
	}
	else if(BASEMATCH_INST_UID == uid || PERSONMATCH_INST_UID == uid)
	{
		CDataMatchInstance instanceDb;
		int ret = instanceDb.GetInstance(uid,id,data);
		if (ret != 0 && ret != R_ERR_NO_DATA)
		{
			error_log("[GetInstance fail][uid=%u,instanceId=%u,ret=%d]",uid,id,ret);
			return ret;
		}
		return 0;
	}
	return R_ERR_NO_DATA;
}
int CLogicInstance::RemoveInstance(unsigned uid, unsigned id)
{
	if (IsValidUid(uid))
	{
		CDataInstance instanceDb;
		int ret = instanceDb.RemoveInstance(uid,id);
		if (ret != 0)
		{
			error_log("[RemoveInstance fail][uid=%u,instanceId=%u,ret=%d]",uid,id,ret);
			return ret;
		}
		return 0;
	}
	else if(BASEMATCH_INST_UID == uid || PERSONMATCH_INST_UID == uid)
	{
		CDataMatchInstance instanceDb;
		int ret = instanceDb.RemoveInstance(uid,id);
		if (ret != 0)
		{
			error_log("[RemoveInstance fail][uid=%u,instanceId=%u,ret=%d]",uid,id,ret);
			return ret;
		}
		return 0;
	}
	return R_ERR_DATA;
}
int CLogicInstance::RemoveInstance(unsigned uid)
{
	if (IsValidUid(uid))
	{
		CDataInstance instanceDb;
		int ret = instanceDb.RemoveInstance(uid);
		if (ret != 0)
		{
			error_log("[RemoveInstance fail][uid=%u,ret=%d]",uid,ret);
			return ret;
		}
		return 0;
	}
	else if(BASEMATCH_INST_UID == uid || PERSONMATCH_INST_UID == uid)
	{
		CDataMatchInstance instanceDb;
		int ret = instanceDb.RemoveInstance(uid);
		if (ret != 0)
		{
			error_log("[RemoveInstance fail][uid=%u,instanceId=%u,ret=%d]",uid,ret);
			return ret;
		}
		return 0;
	}
	return R_ERR_DATA;
}
int CLogicInstance::GetInstanceLimit(unsigned uid, unsigned id, DataInstance &data)
{
	if (IsValidUid(uid))
	{
		CDataInstance instanceDb;
		int ret = instanceDb.GetInstanceLimit(uid,id,data);
		if (ret != 0)
		{
			if(ret != R_ERR_NO_DATA)
				error_log("[GetInstanceLimit fail][uid=%u,instanceId=%u,ret=%d]",uid,id,ret);
			return ret;
		}
		return 0;
	}
	else if(BASEMATCH_INST_UID == uid || PERSONMATCH_INST_UID == uid)
	{
		CDataMatchInstance instanceDb;
		int ret = instanceDb.GetInstanceLimit(uid,id,data);
		if (ret != 0)
		{
			error_log("[GetInstanceLimit fail][uid=%u,instanceId=%u,ret=%d]",uid,id,ret);
			return ret;
		}
		return 0;
	}
	return R_ERR_NO_DATA;
}
int CLogicInstance::GetInstanceLimit(unsigned uid, vector<DataInstance> &datas)
{
	if (IsValidUid(uid))
	{
		CDataInstance instanceDb;
		int ret = instanceDb.GetInstanceLimit(uid,datas);
		if (ret != 0)
		{
			error_log("[GetInstanceLimit fail][uid=%u,instanceId=%u,ret=%d]",uid,ret);
			return ret;
		}
		return 0;
	}
	else if(BASEMATCH_INST_UID == uid || PERSONMATCH_INST_UID == uid)
	{
		CDataMatchInstance instanceDb;
		int ret = instanceDb.GetInstanceLimit(uid,datas);
		if (ret != 0)
		{
			error_log("[GetInstanceLimit fail][uid=%u,instanceId=%u,ret=%d]",uid,ret);
			return ret;
		}
		return 0;
	}
	return R_ERR_NO_DATA;
}
int CLogicInstance::SetInstanceLimit(unsigned uid, unsigned id, const DataInstance &data)
{
	if (IsValidUid(uid))
	{
		CDataInstance instanceDb;
		int ret = instanceDb.SetInstanceLimit(uid,id,data);
		if (ret != 0)
		{
			error_log("[SetInstanceLimit fail][uid=%u,instanceId=%u,ret=%d]",uid,id,ret);
			DB_ERROR_RETURN_MSG("set_instance_fail");
		}
		return 0;
	}
	else if(BASEMATCH_INST_UID == uid || PERSONMATCH_INST_UID == uid)
	{
		CDataMatchInstance instanceDb;
		int ret = instanceDb.SetInstanceLimit(uid,id,data);
		if (ret != 0)
		{
			error_log("[SetInstanceLimit fail][uid=%u,instanceId=%u,ret=%d]",uid,id,ret);
			DB_ERROR_RETURN_MSG("set_instance_fail");
		}
		return 0;
	}
	return R_ERR_DATA;
}
