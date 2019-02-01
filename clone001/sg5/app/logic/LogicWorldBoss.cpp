#include "LogicWorldBoss.h"

struct WorldBoss_Archive
{
	unsigned updateTime;
	Json::Value data;
};

int CLogicWorldBoss::GetWorldBossArchive(unsigned bossId, Json::Value &data)
{
	static map<int, map<unsigned, WorldBoss_Archive> > DomainMap;
	map<unsigned, WorldBoss_Archive> bossMap;
	int domain = 0;
	bool bIsNew = false;
	Config::GetDomain(domain);
	map<int, map<unsigned, WorldBoss_Archive> >::iterator DomainMapItr = DomainMap.find(domain);
	if (DomainMapItr != DomainMap.end())
	{
		bossMap = DomainMapItr->second;
	}
	else
	{
		bIsNew = true;
	}

	map<unsigned, WorldBoss_Archive>::iterator it = bossMap.find(bossId);
	if (it == bossMap.end() || Time::GetGlobalTime() - (it->second).updateTime > 1800)
	{
		string path = Config::GetValue(CONFIG_WORLDBOSS_MAP);
		if (path.empty())
		{
			error_log("[config error][bossId=%u]",bossId);
			DB_ERROR_RETURN_MSG("read_worldboss_fail");
		}
		if (path[path.length()-1] == '/')
		{
			path.append(CTrans::UTOS(bossId)).append(".dat");
		}
		else
		{
			path.append("/").append(CTrans::UTOS(bossId)).append(".dat");
		}
		string sdata;
		int result = File::Read(path, sdata);
		if(result != 0)
		{
			error_log("[read boss fail][ret=%d,path=%s]", result, path.c_str());
			DB_ERROR_RETURN_MSG("read_worldboss_fail");
		}
		Json::Reader reader;
		if (it == bossMap.end())
		{
			WorldBoss_Archive archive;
			if (!reader.parse(sdata, archive.data))
			{
				error_log("[parse json fail][path=%s]", path.c_str());
				DB_ERROR_RETURN_MSG("read_worldboss_fail");
			}
			archive.updateTime = Time::GetGlobalTime();
			bossMap[bossId] = archive;
			if (bIsNew)
			{
				DomainMap[domain] = bossMap;
			}
		}
		else
		{
			if (!reader.parse(sdata, (it->second).data))
			{
				error_log("[parse json fail][path=%s]", path.c_str());
				DB_ERROR_RETURN_MSG("read_worldboss_fail");
			}
			(it->second).updateTime = Time::GetGlobalTime();
		}
	}
	data = bossMap[bossId].data;
	return 0;
}

CDataWorldBoss* CLogicWorldBoss::GetCDataWorldBoss()
{
	static map<int, CDataWorldBoss*> dataMap;
	int serverId = 0;
	Config::GetDomain(serverId);
	if (0 == serverId)
		return NULL;
	map<int, CDataWorldBoss*>::iterator itr = dataMap.find(serverId);
	if (itr != dataMap.end() && NULL != itr->second)
		return itr->second;

	string minBloodStr = Config::GetValue(CONFIG_MIN_WORLDBOSS_BLOOD);
	string maxBloodStr = Config::GetValue(CONFIG_MAX_WORLDBOSS_BLOOD);
	vector<int> vminBloods;
	vector<int> vmaxBloods;
	CBasic::StringSplit(minBloodStr, ",", vminBloods);
	CBasic::StringSplit(maxBloodStr, ",", vmaxBloods);
	CDataWorldBoss *pdata = new CDataWorldBoss();
	if (pdata->Init(Config::GetValue(CONFIG_WORLDBOSS_DIR), vminBloods,vmaxBloods) != 0)
	{
		delete pdata;
		pdata = NULL;
		return NULL;
	}
	dataMap[serverId] = pdata;
	return pdata;
}

int CLogicWorldBoss::GetWorldBossInfo(unsigned uid, Json::Value &data)
{
	CDataWorldBoss* pBoss = GetCDataWorldBoss();
	if (!pBoss)
	{
		DB_ERROR_RETURN_MSG("init_worldboss_fail");
	}
	int ret = 0;
	unsigned blood = 0;
	unsigned number = 0;
	int selfRank = 0;
	WorldBossChallenger self;
	WorldBossChallenger last;
	vector<WorldBossChallenger> top;
	unsigned fullBlood = 0;
	data.resize(WORLD_BOSS_ID_MAX - WORLD_BOSS_ID_MIN + 1);
	for (unsigned id = WORLD_BOSS_ID_MIN; id <= WORLD_BOSS_ID_MAX; id++)
	{
		blood = 0;
		number = 0;
		top.clear();
		ret = pBoss->LoadWorldBoss(uid, id, blood, number, selfRank, self, top, last,fullBlood);
		if (ret != 0)
		{
			DB_ERROR_RETURN_MSG("view_worldboss_fail");
		}
		unsigned temp = id - WORLD_BOSS_ID_MIN;
		data[temp]["uid"] = id;
		data[temp]["blood"] = blood;
		data[temp]["number"] = number;
		data[temp]["top"].resize(top.size());
		data[temp]["fullblood"] = fullBlood;
		for (unsigned i = 0; i < top.size(); i++)
		{
			data[temp]["top"][i]["uid"] = top[i].uid;
			data[temp]["top"][i]["damage"] = top[i].damage;
			data[temp]["top"][i]["name"] = top[i].name;
		}
	}

	return 0;
}

int CLogicWorldBoss::Load(unsigned bossId, unsigned uidBy, Json::Value &result)
{
	if (!IsValidWorldBossId(bossId))
	{
		PARAM_ERROR_RETURN_MSG("param_error");
	}
	int ret = 0;
	ret = GetWorldBossArchive(bossId, result);
	if (ret != 0)
		return ret;
	if (!Json::IsArray(result, "hero") || result["hero"].size() == 0)
	{
		DB_ERROR_RETURN_MSG("worldboss_map_err");
	}
	CDataWorldBoss* pBoss = GetCDataWorldBoss();
	if (!pBoss)
	{
		DB_ERROR_RETURN_MSG("init_worldboss_fail");
	}
	unsigned blood = 0;
	unsigned number = 0;
	int selfRank = 0;
	unsigned fullBlood = 0;
	WorldBossChallenger self;
	WorldBossChallenger last;
	vector<WorldBossChallenger> top;
	ret = pBoss->LoadWorldBoss(uidBy, bossId, blood, number, selfRank, self, top, last,fullBlood);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("load_worldboss_fail");
	}
	result["worldboss"]["blood"] = blood;
	result["worldboss"]["fullblood"] = fullBlood;
	result["worldboss"]["number"] = number;
	result["worldboss"]["top"].resize(top.size());
	Json::Value &topJson = result["worldboss"]["top"];
	for (unsigned i = 0; i < top.size(); i++)
	{
		topJson[i]["uid"] = top[i].uid;
		topJson[i]["damage"] = top[i].damage;
		topJson[i]["name"] = top[i].name;
	}
	result["worldboss"]["self"]["damage"] = self.damage;
	result["worldboss"]["self"]["rank"] = selfRank;
	result["worldboss"]["last"]["uid"] = last.uid;
	result["worldboss"]["last"]["damage"] = last.damage;
	result["worldboss"]["last"]["name"] = last.name;

	result["hero"][(unsigned)0]["hp"] = blood;
	result["hero"][(unsigned)0]["fhp"] = fullBlood;

	result["userid"] = bossId;
	return 0;
}

int CLogicWorldBoss::Save(unsigned bossId, DataUser &userBy, Json::Value &data, Json::Value &result)
{
	if (!IsValidWorldBossId(bossId))
	{
		PARAM_ERROR_RETURN_MSG("param_error");
	}
	CDataWorldBoss* pBoss = GetCDataWorldBoss();
	if (!pBoss)
	{
		DB_ERROR_RETURN_MSG("init_worldboss_fail");
	}
	int ret = 0;
	string name;
	CDataUserBasic userBasicDb;
	ret = userBasicDb.GetUserName(userBy.uid, OpenPlatform::GetType(), name);
	if (ret != 0)
		return ret;

	unsigned damage = 0;
	Json::GetUInt(data, "hurt", damage);
	bool dying = false;
	unsigned blood = 0;
	unsigned number = 0;
	int selfRank = 0;
	WorldBossChallenger self;
	WorldBossChallenger last;
	vector<WorldBossChallenger> top;
	vector<WorldBossChallenger> lucks;
	vector<WorldBossChallenger> dam;
	ret = pBoss->AttackWorldBoss(userBy.uid, bossId, damage, name, dying, blood, number, selfRank, self, top, last, lucks, dam);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("update_worldboss_fail");
	}
	if (dying)	//boss died,reward
	{
		CLogicUpdates logicUpdates;
		CLogicPay logicPay;
		unsigned prosper[10] = {5000,4500,4000,3500,3000,2500,2000,1500,1200,1000};
		map<unsigned, Json::Value> updatesmap;
		for (unsigned i = 0; i < dam.size(); i++)
		{
			Json::Value updates;
			updates.resize(1);
			updates[(unsigned)0]["s"] = "WORLDBOSSTOP";
			updates[(unsigned)0]["bossid"] = bossId;
			updates[(unsigned)0]["damage"] = dam[i].damage;
			updates[(unsigned)0]["coins"] = 10;
			updates[(unsigned)0]["r"] = i + 1;
			updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
			updatesmap[dam[i].uid] = updates;
		}
		for (unsigned i = 0; i < top.size(); i++)
		{
			if (i >= 10) break;
			if(updatesmap.count(top[i].uid))
			{
				Json::Value& updates = updatesmap[top[i].uid];
				updates[(unsigned)0]["prosper1"] = prosper[i];
			}
			else
			{
				Json::Value updates;
				updates.resize(1);
				updates[(unsigned)0]["s"] = "WORLDBOSSTOP";
				updates[(unsigned)0]["bossid"] = bossId;
				updates[(unsigned)0]["damage"] = top[i].damage;
				updates[(unsigned)0]["prosper1"] = prosper[i];
				updates[(unsigned)0]["r"] = i+1;
				updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
				updatesmap[top[i].uid] = updates;
			}
		}
		for (unsigned i = 0; i < lucks.size(); i++)
		{
			if(updatesmap.count(lucks[i].uid))
			{
				Json::Value& updates = updatesmap[lucks[i].uid];
				updates[(unsigned)0]["prosper3"] = 800;
			}
			else
			{
				Json::Value updates;
				updates.resize(1);
				updates[(unsigned)0]["s"] = "WORLDBOSSTOP";
				updates[(unsigned)0]["bossid"] = bossId;
				updates[(unsigned)0]["damage"] = lucks[i].damage;
				updates[(unsigned)0]["prosper3"] = 800;
				updates[(unsigned)0]["r"] = 20 + i*10;
				updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
				updatesmap[lucks[i].uid] = updates;
			}
		}
		if(updatesmap.count(userBy.uid))
		{
			Json::Value& updates = updatesmap[userBy.uid];
			updates[(unsigned)0]["prosper2"] = 1500;
		}
		else
		{
			Json::Value updates;
			updates.resize(1);
			updates[(unsigned)0]["s"] = "WORLDBOSSTOP";
			updates[(unsigned)0]["bossid"] = bossId;
			updates[(unsigned)0]["damage"] = damage;
			updates[(unsigned)0]["prosper2"] = 1500;
			updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
			//logicUpdates.AddUpdates(userBy.uid, updates);
			updatesmap[userBy.uid] = updates;
		}
		for(map<unsigned, Json::Value>::iterator it=updatesmap.begin();it!=updatesmap.end();++it)
			logicUpdates.AddUpdates(it->first, it->second);
	}
	result["worldboss"]["blood"] = blood;
	result["worldboss"]["number"] = number;
	result["worldboss"]["top"].resize(top.size());
	Json::Value &topJson = result["worldboss"]["top"];
	for (unsigned i = 0; i < top.size(); i++)
	{
		topJson[i]["uid"] = top[i].uid;
		topJson[i]["damage"] = top[i].damage;
		topJson[i]["name"] = top[i].name;
	}
	result["worldboss"]["self"]["damage"] = self.damage;
	result["worldboss"]["self"]["rank"] = selfRank;
	result["worldboss"]["last"]["uid"] = last.uid;
	result["worldboss"]["last"]["damage"] = last.damage;
	result["worldboss"]["last"]["name"] = last.name;

	if (Json::IsObject(data, "attackinfo"))
	{
		CLogicArchive logicArchive;
		ret = logicArchive.ProcessAttackInfo(userBy.uid, data["attackinfo"]);
		if (ret != 0)
			return ret;
	}
	return 0;
}

int CLogicWorldBoss::ViewWorldBoss(unsigned uid, unsigned bossId, Json::Value &data)
{
	if (!IsValidWorldBossId(bossId))
	{
		PARAM_ERROR_RETURN_MSG("param_error");
	}
	CDataWorldBoss* pBoss = GetCDataWorldBoss();
	if (!pBoss)
	{
		DB_ERROR_RETURN_MSG("init_worldboss_fail");
	}
	int ret = 0;
	unsigned blood = 0;
	unsigned fullBlood = 0;
	unsigned number = 0;
	int selfRank = 0;
	WorldBossChallenger self;
	WorldBossChallenger last;
	vector<WorldBossChallenger> top;
	ret = pBoss->ViewWorldBoss(uid, bossId, blood,fullBlood, number, selfRank, self, top, last);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("view_worldboss_fail");
	}
	data["uid"] = bossId;
	data["blood"] = blood;
	data["fullblood"] = fullBlood;
	data["number"] = number;
	data["top"].resize(top.size());
	for (unsigned i = 0; i < top.size(); i++)
	{
		data["top"][i]["uid"] = top[i].uid;
		data["top"][i]["damage"] = top[i].damage;
		data["top"][i]["name"] = top[i].name;
	}
	data["self"]["damage"] = self.damage;
	data["self"]["rank"] = selfRank;
	data["last"]["uid"] = last.uid;
	data["last"]["damage"] = last.damage;
	data["last"]["name"] = last.name;

	return 0;
}

int CLogicWorldBoss::ExportWorldBossData(unsigned uid, unsigned bossId, const string &dir)
{
	string file(dir);
	if (file[file.length()-1] != '/')
		file.append("/");
	file.append(CTrans::UTOS(bossId)).append(".dat");

	Json::Reader reader;
	Json::FastWriter writer;
	Json::Value result;
	result["userid"] = bossId;

	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUser(uid, user);
	if (ret != 0)
		return ret;
	result["resources"][(unsigned)0]["c"] = 0;
	result["resources"][(unsigned)0]["m"] = user.r1_max;
	result["resources"][(unsigned)1]["c"] = 0;
	result["resources"][(unsigned)1]["m"] = user.r2_max;
	result["resources"][(unsigned)2]["c"] = 0;
	result["resources"][(unsigned)2]["m"] = user.r3_max;
	result["resources"][(unsigned)3]["c"] = 0;
	result["resources"][(unsigned)3]["m"] = user.r4_max;
	result["resources"][(unsigned)4]["c"] = 0;
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
