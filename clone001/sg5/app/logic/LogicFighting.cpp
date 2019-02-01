#include "LogicFighting.h"

struct FIGHTING_Archive
{
	unsigned updateTime;
	Json::Value data;
};

int CLogicFighting::GetFightingArchive(unsigned id, Json::Value &data)
{
	static map<unsigned, FIGHTING_Archive> fightingMap;
	map<unsigned, FIGHTING_Archive>::iterator it = fightingMap.find(id);
	if (it == fightingMap.end() || Time::GetGlobalTime() - (it->second).updateTime > 1800)
	{
		string path = Config::GetValue(CONFIG_FIGHTING_DATA);
		if (path.empty())
		{
			error_log("[fighting config error][id=%u]",id);
			DB_ERROR_RETURN_MSG("init_fighting_fail");
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
			error_log("[init fighting fail][ret=%d,path=%s]", result, path.c_str());
			DB_ERROR_RETURN_MSG("init_fighting_fail");
		}
		Json::Reader reader;
		if (it == fightingMap.end())
		{
			FIGHTING_Archive archive;
			if (!reader.parse(sdata, archive.data))
			{
				error_log("[parse json fail][path=%s]", path.c_str());
				DB_ERROR_RETURN_MSG("init_fighting_fail");
			}
			archive.updateTime = Time::GetGlobalTime();
			fightingMap[id] = archive;
		}
		else
		{
			if (!reader.parse(sdata, (it->second).data))
			{
				error_log("[parse json fail][path=%s]", path.c_str());
				DB_ERROR_RETURN_MSG("init_fighting_fail");
			}
			(it->second).updateTime = Time::GetGlobalTime();
		}
	}
	data = fightingMap[id].data;
	return 0;
}

int CLogicFighting::Load(unsigned fightingId, unsigned uidBy, Json::Value &result)
{
	if (!IsValidFightingId(fightingId))
	{
		REFUSE_RETURN_MSG("invalid_fighting_id");
	}
	int ret = GetFightingArchive(fightingId, result);
	if (ret != 0)
		return ret;
	if (!result.isMember("userid"))
		result["userid"] = fightingId;
	return 0;
}

int CLogicFighting::Save(unsigned fightingId, DataUser &userBy, const Json::Value &data, Json::Value &result)
{
	return 0;
}

int CLogicFighting::ExportFightingData(unsigned uid, unsigned fightingId, const string &dir)
{
	string file(dir);
	if (file[file.length()-1] != '/')
		file.append("/");
	file.append(CTrans::UTOS(fightingId)).append(".dat");

	Json::Reader reader;
	Json::FastWriter writer;
	Json::Value result;
	result["userid"] = fightingId;

	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUser(uid, user);
	if (ret != 0)
		return ret;
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
	ret = File::Write(file, sdata);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("write file fail");
	}

	return 0;
}
