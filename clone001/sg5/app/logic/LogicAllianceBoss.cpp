#include "LogicAllianceBoss.h"
#include "DataUser.h"

struct AllianceBoss_Archive
{
	unsigned updateTime;
	Json::Value data;
};


int CLogicAllianceBoss::GetAllianceBossArchive(unsigned bossId, Json::Value &data)
{
	static map<int, map<unsigned, AllianceBoss_Archive> > DomainMap;
	map<unsigned, AllianceBoss_Archive> bossMap;
	int domain = 0;
	bool bIsNew = false;
	Config::GetDomain(domain);
	map<int, map<unsigned, AllianceBoss_Archive> >::iterator DomainMapItr = DomainMap.find(domain);
	if (DomainMapItr != DomainMap.end())
	{
		bossMap = DomainMapItr->second;
	}
	else
	{
		bIsNew = true;
	}
	map<unsigned, AllianceBoss_Archive>::iterator it = bossMap.find(bossId);
	if (it == bossMap.end() || Time::GetGlobalTime() - (it->second).updateTime > 1800)
	{
		string path = Config::GetValue(CONFIG_WORLDBOSS_MAP);
		if (path.empty())
		{
			error_log("[config error][bossId=%u]",bossId);
			DB_ERROR_RETURN_MSG("read_allianceboss_fail_1");
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
			DB_ERROR_RETURN_MSG("read_allianceboss_fail_2");
		}
		Json::Reader reader;
		if (it == bossMap.end())
		{
			AllianceBoss_Archive archive;
			if (!reader.parse(sdata, archive.data))
			{
				error_log("[parse json fail][path=%s]", path.c_str());
				DB_ERROR_RETURN_MSG("read_allianceboss_fail_3");
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
				DB_ERROR_RETURN_MSG("read_allianceboss_fail_4");
			}
			(it->second).updateTime = Time::GetGlobalTime();
		}
	}
	data = bossMap[bossId].data;
	return 0;
}


CDataAllianceBoss* CLogicAllianceBoss::GetCDataAllianceBoss()
{
	static map<int, CDataAllianceBoss*> dataMap;
	int serverId = 0;
	Config::GetDomain(serverId);
	if (0 == serverId)
		return NULL;
	map<int, CDataAllianceBoss*>::iterator itr = dataMap.find(serverId);
	if (itr != dataMap.end() && NULL != itr->second)
		return itr->second;

	static CDataAllianceBoss* pAllianceBoss = NULL;
	if (!pAllianceBoss)
	{
		string minBloodStr = Config::GetValue(CONFIG_MIN_WORLDBOSS_BLOOD);
		string maxBloodStr = Config::GetValue(CONFIG_MAX_WORLDBOSS_BLOOD);
		vector<int> vminBloods;
		vector<int> vmaxBloods;
		CBasic::StringSplit(minBloodStr, ",", vminBloods);
		CBasic::StringSplit(maxBloodStr, ",", vmaxBloods);
		pAllianceBoss = new CDataAllianceBoss();
		if (pAllianceBoss->Init(Config::GetValue(CONFIG_WORLDBOSS_DIR), vminBloods,vmaxBloods,sem_alliance) != 0)
		{
			delete pAllianceBoss;
			pAllianceBoss = NULL;
			return NULL;
		}
	}
	dataMap[serverId] = pAllianceBoss;
	return pAllianceBoss;
}


int CLogicAllianceBoss::GetAllianceBossInfo1(unsigned uid, Json::Value &data,unsigned vip_gr,unsigned ts)
{
	unsigned alliance_id;  //通过uid获取其帮会id
	CDataUser datauser;
	DataUser user;
	int ret = 0;
	ret = datauser.GetUser(uid,user);
	if(ret !=0 )
	{
		return ret;
	}
	alliance_id = user.alliance_id;

	CDataAllianceBoss* pBoss = GetCDataAllianceBoss();
	if (!pBoss)
	{
		DB_ERROR_RETURN_MSG("init_allianceboss_fail_GetAllianceBossInfo1");
	}

	unsigned blood = 0;
	unsigned fullBlood = 0;
	data.resize(1);
	unsigned id = ALLIANCE_BOSS_ID;

	ret = pBoss->LoadAllianceBossOpener(uid, id, blood,fullBlood,vip_gr,alliance_id,ts);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("Alliance_boss_Get_Failed");
	}

	//updates
	Json::Value value;
	ret = GetAllianceJsonUpdates(uid, alliance_id,value,ts,vip_gr);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("Alliance_boss_Updatas_Failed");
	}
	unsigned temp = id - ALLIANCE_BOSS_ID;
	data[temp]["uid"] = uid;
	data[temp]["blood"] = blood;
	data[temp]["fullblood"] = fullBlood;
	data[temp]["vip_gr"] = vip_gr;
	return 0;
}

int CLogicAllianceBoss::GetAllianceBossInfo(unsigned uid, Json::Value &data)
{
	unsigned alliance_id = 0;  //通过uid获取其帮会id
	CDataUser datauser;
	DataUser user;
	int ret = datauser.GetUser(uid,user);
	if(ret != 0)
		return ret;
	alliance_id = user.alliance_id;

	CDataAllianceBoss* pBoss = GetCDataAllianceBoss();
	if (!pBoss)
	{
		DB_ERROR_RETURN_MSG("init_allianceboss_fail_GetAllianceBossInfo");
	}


	unsigned blood = 0;
	unsigned number = 0;
	int selfRank = 0;
	unsigned fullBlood = 0;
	AllianceBossChallenger self;   //自己给boss伤害
	AllianceBossChallenger last;   //最后一击
	vector<AllianceBossChallenger> top;   //排名
	data.resize(1);

	unsigned id = ALLIANCE_BOSS_ID;
	blood = 0;
	number = 0;
	top.clear();
	ret = pBoss->LoadAllianceBoss(uid, id, blood, number, selfRank, self, top, last,fullBlood,alliance_id);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("Alliance_boss_need_wait  ");
	}

	unsigned temp = id - ALLIANCE_BOSS_ID;
	data[temp]["uid"] = uid;
	data[temp]["blood"] = blood;
	data[temp]["fullblood"] = fullBlood;
	data[temp]["number"] = number;
	data[temp]["top"].resize(top.size());
	for (unsigned i = 0; i < top.size(); i++)
	{
		data[temp]["top"][i]["uid"] = top[i].uid;
		data[temp]["top"][i]["damage"] = top[i].damage;
		data[temp]["top"][i]["name"] = top[i].name;
	}

	debug_log("logic_allianceboss:uid=%u|blood=%u  | fullblood=%u",uid,blood,fullBlood);

	return 0;
}

//获取昨天屠魔排行榜
int CLogicAllianceBoss::GetLastAllianceBossInfo(unsigned uid, Json::Value &data)
{
	unsigned alliance_id;  //通过uid获取其帮会id
	CDataUser datauser;
	DataUser user;
	int ret = datauser.GetUser(uid,user);
	if(ret != 0)
	{
		return ret;
	}
	alliance_id = user.alliance_id;

	CDataAllianceBoss* pBoss = GetCDataAllianceBoss();
	if (!pBoss)
	{
		DB_ERROR_RETURN_MSG("init_allianceboss_fail_Load");
	}


	unsigned blood = 0;
	unsigned number = 0;
	int selfRank = 0;
	unsigned fullBlood = 0;
	AllianceBossChallenger self;   //自己给boss伤害
	AllianceBossChallenger last;   //最后一击
	vector<AllianceBossChallenger> top;   //排名
	data.resize(1);
	unsigned id = ALLIANCE_BOSS_ID;
	blood = 0;
	number = 0;
	top.clear();
	ret = pBoss->LoadLastAllianceBoss(uid, id, blood, number, selfRank, self, top, last,fullBlood,alliance_id);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("Alliance_boss_need_wait  ");
	}

	unsigned temp = id - ALLIANCE_BOSS_ID;
	data[temp]["uid"] = uid;
//	data[temp]["blood"] = blood;
	data[temp]["fullblood"] = fullBlood;
	data[temp]["number"] = number;
	data[temp]["top"].resize(top.size());
	for (unsigned i = 0; i < top.size(); i++)
	{
		data[temp]["top"][i]["uid"] = top[i].uid;
		data[temp]["top"][i]["damage"] = top[i].damage;
		data[temp]["top"][i]["name"] = top[i].name;
	}
	data[temp]["last"]["uid"] = last.uid;
	data[temp]["last"]["damage"] = last.damage;
	data[temp]["last"]["name"] = last.name;

	return 0;
}

int CLogicAllianceBoss::Load(unsigned bossId, unsigned uidBy, Json::Value &result)
{

	unsigned alliance_id;  //通过uid获取其帮会id
	CDataUser datauser;
	DataUser user;
	int ret = 0;
	ret = datauser.GetUser(uidBy,user);
	if (ret != 0)
		return ret;
	alliance_id = user.alliance_id;

	if (!IsValidAllianceBossId(bossId))
	{
		PARAM_ERROR_RETURN_MSG("param_error");
	}

	ret = GetAllianceBossArchive(bossId, result);
	if (ret != 0)
	{
		return ret;
	}
	if (!Json::IsArray(result, "hero") || result["hero"].size() == 0)
	{
		DB_ERROR_RETURN_MSG("allianceboss_map_err");
	}
	CDataAllianceBoss* pBoss = GetCDataAllianceBoss();
	if (!pBoss)
	{
		DB_ERROR_RETURN_MSG("init_allianceboss_fail_Load");
	}
	unsigned blood = 0;
	unsigned fullBlood = 0;
	unsigned number = 0;
	int selfRank = 0;
	AllianceBossChallenger self;
	AllianceBossChallenger last;
	vector<AllianceBossChallenger> top;
	ret =pBoss->LoadAllianceBoss(uidBy, bossId, blood, number, selfRank, self, top, last,fullBlood,alliance_id);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("load_allianceboss_fail_11");
	}
	debug_log("logic_allianceboss11:uid=%u|blood=%u  | fullblood=%u",uidBy,blood,fullBlood);
	result["allianceboss"]["blood"] = blood;
	result["allianceboss"]["fullblood"] = fullBlood;
	result["allianceboss"]["number"] = number;
	result["allianceboss"]["top"].resize(top.size());
	Json::Value &topJson = result["allianceboss"]["top"];
	for (unsigned i = 0; i < top.size(); i++)
	{
		topJson[i]["uid"] = top[i].uid;
		topJson[i]["damage"] = top[i].damage;
		topJson[i]["name"] = top[i].name;
	}
	result["allianceboss"]["self"]["damage"] = self.damage;
	result["allianceboss"]["self"]["rank"] = selfRank;
	result["allianceboss"]["last"]["uid"] = last.uid;
	result["allianceboss"]["last"]["damage"] = last.damage;
	result["allianceboss"]["last"]["name"] = last.name;

	result["hero"][(unsigned)0]["hp"] = blood;
	result["hero"][(unsigned)0]["fhp"] = fullBlood;

	result["userid"] = bossId;
	return 0;
}

int CLogicAllianceBoss::Save(unsigned bossId, DataUser &userBy, Json::Value &data, Json::Value &result)
{
	if (!IsValidAllianceBossId(bossId))
	{
		PARAM_ERROR_RETURN_MSG("param_error");
	}
	unsigned alliance_id;  //通过uid获取其帮会id

	alliance_id = userBy.alliance_id;

	CDataAllianceBoss* pBoss = GetCDataAllianceBoss();
	if (!pBoss)
	{
		DB_ERROR_RETURN_MSG("init_allianceboss_fail_Save");
	}
	int ret = 0;
	string name;
	CDataUserBasic userBasicDb;
	ret = userBasicDb.GetUserName(userBy.uid, OpenPlatform::GetType(), name);
	if (ret != 0)
		return ret;

	unsigned damage = 0;
	Json::GetUInt(data, "hurt", damage);    //获取个人对boss伤害值
	if(damage > 500000)
	{
		debug_log("hurt_is_large: hurt=%u,uid=%u",damage,userBy.uid);
	}
	bool dying = false;
	unsigned blood = 0;
	unsigned number = 0;
	int selfRank = 0;
	unsigned vip = 0;
	AllianceBossChallenger self;
	AllianceBossChallenger last;
	vector<AllianceBossChallenger> top;
	vector<AllianceBossChallenger> lucks;
	vector<AllianceBossChallenger> dam;
	ret = pBoss->AttackAllianceBoss(userBy.uid, bossId, damage, name, dying, blood, number, selfRank, self, top, last, lucks, dam, alliance_id, vip);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("update_Allianceboss_fail_save");
	}
	if (dying)	//boss died,reward
	{
		CLogicUpdates logicUpdates;
		unsigned prosper[10] = {3000,2000,1000,500,500,500,500,500,500,500};
		map<unsigned, Json::Value> updatesmap;
		for (unsigned i = 0; i < dam.size(); i++)
		{
			Json::Value updates;
			updates.resize(1);
			updates[(unsigned)0]["s"] = "ALLIANCEBOSSTOP"; //
			updates[(unsigned)0]["bossid"] = bossId;
			updates[(unsigned)0]["damage"] = dam[i].damage;
			updates[(unsigned)0]["coins"] = 5;
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
				updates[(unsigned)0]["s"] = "ALLIANCEBOSSTOP";
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
				updates[(unsigned)0]["prosper3"] = 400;
			}
			else
			{
				Json::Value updates;
				updates.resize(1);
				updates[(unsigned)0]["s"] = "ALLIANCEBOSSTOP";
				updates[(unsigned)0]["bossid"] = bossId;
				updates[(unsigned)0]["damage"] = lucks[i].damage;
				updates[(unsigned)0]["prosper3"] = 400;
				updates[(unsigned)0]["r"] = 20 + i*10;
				updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
				updatesmap[lucks[i].uid] = updates;
			}
		}
		if(updatesmap.count(userBy.uid))
		{
			Json::Value& updates = updatesmap[userBy.uid];
			updates[(unsigned)0]["prosper2"] = 800;
		}
		else
		{
			Json::Value updates;
			updates.resize(1);
			updates[(unsigned)0]["s"] = "ALLIANCEBOSSTOP";
			updates[(unsigned)0]["bossid"] = bossId;
			updates[(unsigned)0]["damage"] = damage;
			updates[(unsigned)0]["prosper2"] = 800;
			updates[(unsigned)0]["ts"] = Time::GetGlobalTime();
			//logicUpdates.AddUpdates(userBy.uid, updates);
			updatesmap[userBy.uid] = updates;
		}
		for(map<unsigned, Json::Value>::iterator it=updatesmap.begin();it!=updatesmap.end();++it)
			logicUpdates.AddUpdates(it->first, it->second);

	}
	result["allianceboss"]["blood"] = blood;
	result["allianceboss"]["number"] = number;
	result["allianceboss"]["top"].resize(top.size());
	Json::Value &topJson = result["allianceboss"]["top"];
	for (unsigned i = 0; i < top.size(); i++)
	{
		topJson[i]["uid"] = top[i].uid;
		topJson[i]["damage"] = top[i].damage;
		topJson[i]["name"] = top[i].name;
	}
	result["allianceboss"]["self"]["damage"] = self.damage;
	result["allianceboss"]["self"]["rank"] = selfRank;
	result["allianceboss"]["last"]["uid"] = last.uid;
	result["allianceboss"]["last"]["damage"] = last.damage;
	result["allianceboss"]["last"]["name"] = last.name;

	if (Json::IsObject(data, "attackinfo"))
	{
		CLogicArchive logicArchive;
		ret = logicArchive.ProcessAttackInfo(userBy.uid, data["attackinfo"]);
		if (ret != 0)
			return ret;
	}
	return 0;
}

int CLogicAllianceBoss::ViewAllianceBoss(unsigned uid, unsigned bossId, Json::Value &data)
{
	if (!IsValidAllianceBossId(bossId))
	{
		PARAM_ERROR_RETURN_MSG("param_error");
	}
	unsigned alliance_id;  //通过uid获取其帮会id
	CDataUser datauser;
	DataUser user;
	datauser.GetUser(uid,user);
	alliance_id = user.alliance_id;
	//CDataWorldBoss* pBoss = GetCDataWorldBoss();
	CDataAllianceBoss* pBoss = GetCDataAllianceBoss();
	if (!pBoss)
	{
		DB_ERROR_RETURN_MSG("init_allianceboss_fail");
	}
	int ret = 0;
	unsigned blood = 0;
	unsigned number = 0;
	int selfRank = 0;
	unsigned fullBlood = 0;
	AllianceBossChallenger self;
	AllianceBossChallenger last;
	vector<AllianceBossChallenger> top;
	ret = pBoss->ViewAllianceBoss(uid, bossId, blood,fullBlood, number, selfRank, self, top, last,alliance_id);
	if (ret != 0)
	{
		DB_ERROR_RETURN_MSG("view_allianceboss_fail");
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

//通知帮会在线人员屠魔已开启
int CLogicAllianceBoss::GetAllianceJsonUpdates(unsigned uid, unsigned allianceId, Json::Value &value,unsigned ts,unsigned vip_gr)
{
	int ret;
	DataUser userBy;
	CLogicUser logicUser;
	ret = logicUser.GetUserLimit(uid, userBy);
	if (ret != 0)
		return ret;

	DataAlliance alliance;
	CLogicAlliance logcialliance;
	ret = logcialliance.GetAlliance(allianceId, alliance);
	if(ret != 0)
	{
		return ret;
	}
	map<unsigned, int> interacts;
	CLogicUserInteract logicInteract;
	ret = logicInteract.GetFriendInteracts(uid, interacts);

	Json::Value &membersData = value["members"];
	membersData = Json::Value(Json::arrayValue);
	vector<DataAllianceMember> members;
	ret = logcialliance.GetMembers(allianceId, members);
	for(vector<DataAllianceMember>::iterator itr = members.begin(); itr != members.end(); itr++)
	{
		DataUser user;
		ret = logicUser.GetUser(itr->uid, user);
		if(0 != ret)
		{
			continue;
		}

		Json::Value &member = membersData[membersData.size()];

		member["online"] = (Time::GetGlobalTime() - ONLINE_TIMEOUT < user.last_active_time) ? 1 : 0;
		if(member["online"] == 1)
		{
			if(uid != itr->uid)
			{
				DataUserBasic datauserbasic;
				CDataUserBasic datausr;
				string name;
				int ret = datausr.GetUserBasic(itr->uid,OpenPlatform::GetType(),datauserbasic);
				if(ret != 0)
				{
					return ret;
				}
				name = datauserbasic.name;
				unsigned now = Time::GetGlobalTime();
				Json::Value updates;
				updates.resize(1);
				updates[(unsigned)0]["s"] = "ALLIANCEBOSSSTART";
				updates[(unsigned)0]["ts"] = now;
				updates[(unsigned)0]["name"] = name;
				updates[(unsigned)0]["uid"] = itr->uid;
				updates[(unsigned)0]["vip"] = vip_gr;
				updates[(unsigned)0]["starttime"] = ts;

				CLogicUpdates logicUpdates;
				int r = logicUpdates.AddUpdates(itr->uid,updates);
				if(0 != r)
				{
					error_log("add updates failed ,uid=%u",itr->uid);
				}
			}
		}
	}


	return 0;
}


