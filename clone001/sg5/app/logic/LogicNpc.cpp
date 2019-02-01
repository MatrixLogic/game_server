#include "LogicNpc.h"

struct NPC_Archive
{
	unsigned updateTime;
	Json::Value data;
};

int CLogicNpc::Load(unsigned npcId, unsigned uidBy, Json::Value &result)
{

	int ret = 0;
	if (!IsNPCUid(npcId))
	{
		REFUSE_RETURN_MSG("invalid_npc_id");
	}
	if(IsGateNPCUid(npcId) || IsWorldNPCUid(npcId))
	{
		ret = GetNpcArchive(npcId, result);
		if (ret != 0)
			return ret;
		result["userid"] = npcId;
		return 0;
	}
//	unsigned lastts = 0;
//	int ret = logicInstance.Load(npcId,uidBy,false,result,lastts);
//	if(ret != 0 || CTime::GetDayInterval(lastts, Time::GetGlobalTime()) > 0)
//	{
//		ret = logicInstance.Load(npcId,uidBy,true,result);
//	}
//	//ret = GetNpcArchive(npcId, result);
//	if (ret != 0)
//		return ret;
//	result["userid"] = npcId;
	CLogicInstance instanceDb;
	DataInstance instance;

	bool restart = false;
	ret = instanceDb.GetInstanceLimit(uidBy, npcId, instance);
	if (!(ret == 0 || ret == R_ERR_NO_DATA))
	{
		error_log("[GetInstance fail][uid=%u,instanceId=%u,ret=%d]",uidBy,npcId,ret);
		DB_ERROR_RETURN_MSG("get_instance_fail");
	}
	if(ret == R_ERR_NO_DATA || CTime::GetDayInterval(instance.lastts, Time::GetGlobalTime()) > 0)
	{
		restart = true;
	}
	if (restart)
	{
		int retnpc = GetNpcArchive(npcId, result);
		if (retnpc != 0)
			return retnpc;

		Json::FastWriter writer;
		if (ret == R_ERR_NO_DATA)
		{
			instance.uid = uidBy;
			instance.id = npcId;
			instance.count = 1;
			instance.lastts = Time::GetGlobalTime();
			instance.damage = 0;
			instance.win = 0;
			instance.data = writer.write(result);
			//lastts = instance.lastts;
			if (IsValidUid(uidBy))
			{
				ret = instanceDb.AddInstance(instance);
				if (ret != 0)
				{
					error_log("[AddInstance fail][uid=%u,instanceId=%u,ret=%d]",uidBy,npcId,ret);
					DB_ERROR_RETURN_MSG("add_instance_fail");
				}
			}
		}
		else
		{
			//lastts = instance.lastts;
			if (CTime::GetDayInterval(instance.lastts, Time::GetGlobalTime()) > 0)
			{
				instance.count = 0;
			}
			instance.count++;
			instance.lastts = Time::GetGlobalTime();
			instance.damage = 0;
			instance.data = writer.write(result);
			if (IsValidUid(uidBy))
			{
				ret = instanceDb.SetInstance(uidBy, npcId, instance);
				if (ret != 0)
				{
					error_log("[SetInstance fail][uid=%u,instanceId=%u,ret=%d]",uidBy,npcId,ret);
					DB_ERROR_RETURN_MSG("set_instance_fail");
				}
			}
		}
	}
	else
	{
		//lastts = instance.lastts;
		ret = instanceDb.GetInstance(uidBy, npcId, instance);
		if (ret != 0)
		{
			error_log("[GetInstance fail][uid=%u,instanceId=%u,ret=%d]",uidBy,npcId,ret);
			DB_ERROR_RETURN_MSG("get_instance_fail");
		}

		if (CTime::GetDayInterval(instance.lastts, Time::GetGlobalTime()) > 0)
		{
			instance.count = 0;
		}
		instance.count++;
		instance.lastts = Time::GetGlobalTime();
		ret = instanceDb.SetInstanceLimit(uidBy, npcId, instance);
		if (ret != 0)
		{
			error_log("[SetInstanceLimit fail][uid=%u,instanceId=%u,ret=%d]",uidBy,npcId,ret);
			DB_ERROR_RETURN_MSG("set_instance_fail");
		}
		Json::Reader reader;
		if (!reader.parse(instance.data, result))
		{
			error_log("[parse instance fail][uid=%u,instanceId=%u]",uidBy,npcId);
			DB_ERROR_RETURN_MSG("parse_instance_fail");
		}
	}
	result["userid"] = npcId;
	return 0;
}

int CLogicNpc::Save(unsigned npcId, DataUser &userBy, Json::Value &data, Json::Value &result)
{
	if (!IsNPCUid(npcId))
	{
		REFUSE_RETURN_MSG("invalid_npc_id");
	}

	int ret = 0;
	int destroy = 0;
	int damage = 0;
	Json::GetInt(data, "destroy", destroy);
	Json::GetInt(data, "damage", damage);
	if (destroy == 1 )
	{
		CLogicUser logicUser;
		if (userBy.wf_status.empty())
			userBy.wf_status = "[]";
		Json::Reader reader;
		Json::Value json;
		if (!reader.parse(userBy.wf_status, json) || !json.isArray())
		{
			error_log("[data error][uid=%u,npcId=%u]",userBy.uid,npcId);
			DB_ERROR_RETURN_MSG("wf_status_data_error");
		}
		bool append = true;
		for (unsigned i = 0; i < json.size(); i++)
		{
			if (json[i].isIntegral() && json[i].asInt() == (int)npcId)
			{
				append = false;
				break;
			}
		}
		if (append)
		{
			json.append(npcId);
		}
		Json::FastWriter writer;
		userBy.wf_status = writer.write(json);
		ret = logicUser.SetUser(userBy.uid, userBy);
		if (ret != 0)
			return ret;
	}
	if(IsGateNPCUid(npcId) || IsWorldNPCUid(npcId))
	{
		if (Json::IsObject(data, "attackinfo"))
		{
			CLogicArchive logicArchive;
			ret = logicArchive.ProcessAttackInfo(userBy.uid, data["attackinfo"]);
			if (ret != 0)
				return ret;
		}
		return 0;
	}

	CLogicInstance logicInstanc;
	ret = logicInstanc.Save(npcId,userBy,data,result);
	return ret;
//	if (Json::IsObject(data, "attackinfo"))
//	{
//		CLogicArchive logicArchive;
//		ret = logicArchive.ProcessAttackInfo(userBy.uid, data["attackinfo"]);
//		if (ret != 0)
//			return ret;
//	}
	//return 0;
}

int CLogicNpc::ExportNpcData(unsigned uid, unsigned npcId, const string &dir)
{
	string file(dir);
	if (file[file.length()-1] != '/')
		file.append("/");
	file.append(CTrans::UTOS(npcId)).append(".dat");

	Json::Reader reader;
	Json::FastWriter writer;
	Json::Value result;
	result["userid"] = npcId;

	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUser(uid, user);
	if (ret != 0)
		return ret;
	result["level"] = user.level;
	result["resources"][(unsigned)0]["c"] = user.r1;
	result["resources"][(unsigned)0]["m"] = user.r1_max;
	result["resources"][(unsigned)1]["c"] = user.r2;
	result["resources"][(unsigned)1]["m"] = user.r2_max;
	result["resources"][(unsigned)2]["c"] = user.r3;
	result["resources"][(unsigned)2]["m"] = user.r3_max;
	result["resources"][(unsigned)3]["c"] = user.r4;
	result["resources"][(unsigned)3]["m"] = user.r4_max;
	result["resources"][(unsigned)4]["c"] = user.r5;
	result["resources"][(unsigned)4]["m"] = user.r5_max;
	result["lasttime"] = Time::GetGlobalTime();
	if (!user.barrackQ.empty() && !reader.parse(user.barrackQ, result["barrackQ"]))
	{
		DB_ERROR_RETURN_MSG("parse brrackQ fail");
	}
	if (!user.soldier.empty() && !reader.parse(user.soldier, result["soldier"]))
	{
		DB_ERROR_RETURN_MSG("parse soldier fail");
	}
	if (!user.soldierlevel.empty() && !reader.parse(user.soldierlevel, result["soldierlevel"]))
	{
		DB_ERROR_RETURN_MSG("parse soldierlevel fail");
	}
	if (!user.user_tech.empty() && !reader.parse(user.user_tech, result["tech"]))
	{
		DB_ERROR_RETURN_MSG("parse user_tech fail");
	}
	if (!user.buildQ.empty() && !reader.parse(user.buildQ, result["buildQ"]))
	{
		error_log("[buildQ parse error][uid=%u]",uid);
	}
	if (!user.skillQ.empty() && !reader.parse(user.skillQ, result["skillQ"]))
	{
		error_log("[skillQ parse error][uid=%u]",uid);
	}
	if (!user.trainQ.empty() && !reader.parse(user.trainQ, result["trainQ"]))
	{
		error_log("[trainQ parse error][uid=%u]",uid);
	}

	CLogicBuilding logicBuiding;
	ret = logicBuiding.GetBuilding(uid, result["baseop"]);
	if (ret != 0)
		return ret;

	CLogicHero logicHero;
	ret = logicHero.GetHero(uid, result["hero"]);
	if (ret != 0)
		return ret;

	CLogicEquipment logicEquip;
	ret = logicEquip.GetEquipmentUsed(uid, result["equipment"]);
	if (ret != 0)
		return ret;

	string sdata = writer.write(result);
	int fd = open(file.c_str(), O_CREAT | O_WRONLY | O_TRUNC );
	if (write(fd, sdata.data(), sdata.size()) == -1)
	{
		DB_ERROR_RETURN_MSG("write file fail");
	}

	return 0;
}

int CLogicNpc::GetNpcArchive(unsigned npcId, Json::Value &data)
{
	static map<unsigned, NPC_Archive> npcMap;
	map<unsigned, NPC_Archive>::iterator it = npcMap.find(npcId);
	if (it == npcMap.end() || Time::GetGlobalTime() - (it->second).updateTime > 1800)
	{
		string path = Config::GetValue(CONFIG_NPC_DATA);
		if (path.empty())
		{
			error_log("[npc config error][npcId=%u]",npcId);
			DB_ERROR_RETURN_MSG("db_init_npc_fail");
		}
		if (path[path.length()-1] == '/')
		{
			path.append(CTrans::UTOS(npcId)).append(".dat");
		}
		else
		{
			path.append("/").append(CTrans::UTOS(npcId)).append(".dat");
		}
		string sdata;
		int result = File::Read(path, sdata);
		if(result != 0)
		{
			error_log("[init npc fail][ret=%d,path=%s]", result, path.c_str());
			DB_ERROR_RETURN_MSG("db_init_npc_fail");
		}
		Json::Reader reader;
		if (it == npcMap.end())
		{
			NPC_Archive npcArchive;
			if (!reader.parse(sdata, npcArchive.data))
			{
				error_log("[parse json fail][path=%s]", path.c_str());
				DB_ERROR_RETURN_MSG("db_init_npc_fail");
			}
			npcArchive.updateTime = Time::GetGlobalTime();
			npcMap[npcId] = npcArchive;
		}
		else
		{
			if (!reader.parse(sdata, (it->second).data))
			{
				error_log("[parse json fail][path=%s]", path.c_str());
				DB_ERROR_RETURN_MSG("db_init_npc_fail");
			}
			(it->second).updateTime = Time::GetGlobalTime();
		}
	}
	data = npcMap[npcId].data;
	return 0;
}
