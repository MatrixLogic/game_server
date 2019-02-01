/*
 * LogicMatchInstance.cpp
 *
 *  Created on: 2013-5-21
 *      Author: Administrator
 */

#include "LogicMatchInstance.h"

int CLogicMatchInstance::Load(unsigned instanceId, const string &filePath, Json::Value &data)
{
	string path = filePath;
	if (path.empty())
	{
		error_log("[match instance config error][npcId=%u]",instanceId);
		DB_ERROR_RETURN_MSG("db_init_mathc_instance_fail");
	}
	if (path[path.length()-1] == '/')
	{
		path.append(CTrans::UTOS(instanceId)).append(".dat");
	}
	else
	{
		path.append("/").append(CTrans::UTOS(instanceId)).append(".dat");
	}
	string sdata;
	int result = File::Read(path, sdata);
	if(result != 0)
	{
		error_log("[read match instance file fail][ret=%d,path=%s]", result, path.c_str());
		DB_ERROR_RETURN_MSG("db_init_mathc_instance_fail");
	}

	Json::Reader reader;
	if (!reader.parse(sdata, data))
	{
		error_log("[parse json fail][path=%s]", path.c_str());
		DB_ERROR_RETURN_MSG("db_init_mathc_instance_fail");
	}
	return 0;
}

int CLogicMatchInstance::Save(unsigned instanceId, const string &filePath, const Json::Value &data)
{
	string path = filePath;
	if (path.empty())
	{
		error_log("[match instance config error][npcId=%u]",instanceId);
		DB_ERROR_RETURN_MSG("db_init_mathc_instance_fail");
	}
	if (path[path.length()-1] == '/')
	{
		path.append(CTrans::UTOS(instanceId)).append(".dat");
	}
	else
	{
		path.append("/").append(CTrans::UTOS(instanceId)).append(".dat");
	}
	Json::FastWriter writer;
	string sdata = writer.write(data);
	if (sdata.empty())
	{
		error_log("[parse json fail][path=%s]", path.c_str());
		DB_ERROR_RETURN_MSG("db_init_mathc_instance_fail");
	}

	if(File::IsExist(path))
	{
		File::Clear(path);
	}
	int result = File::Write(path, sdata);
	if(result != 0)
	{
		error_log("[read match instance file fail][ret=%d,path=%s]", result, path.c_str());
		DB_ERROR_RETURN_MSG("db_init_mathc_instance_fail");
	}

	return 0;
}

int CLogicMatchInstance::GetLocalMatchInstance(unsigned instid,Json::Value &data)
{
	if (!IsValidBMatchInstId(instid) && !IsValidPersonMatchId(instid) )
	{
		error_log("[isn't local match instance id][instanceid=%u]",instid);
	}
	string path = Config::GetValue(CONFIG_MATCH_INSTANCE_PATH);
	return Load(instid,path,data);
}

int CLogicMatchInstance::SetLocalMatchInstance(unsigned instid,const Json::Value &data)
{
	if (!IsValidBMatchInstId(instid) && !IsValidPersonMatchId(instid) )
	{
		error_log("[isn't local match instance id][instanceid=%u]",instid);
	}
	string path = Config::GetValue(CONFIG_MATCH_INSTANCE_PATH);
	return Save(instid,path,data);
}

int CLogicMatchInstance::GetAllServerMatchInstance(unsigned instid,Json::Value &data)
{
	string path = Config::GetValue(CONFIG_ALLS_MATCH_INST_PATH);
	return Load(instid,path,data);
}

int CLogicMatchInstance::SetAllServerMatchInstance(unsigned instid,const Json::Value &data)
{
	string path = Config::GetValue(CONFIG_ALLS_MATCH_INST_PATH);
	return Save(instid,path,data);
}
