#include "LogicMap.h"

#define MAX_MAP_NUMBER 10
#define MAP_GROUP_RANGE 10
#define LEVEL_2_GROUP(x) (x)

#define MAP_ROW 10
#define MAP_COLUMN 9

//20-30级
static int typeMap[MAP_ROW][MAP_COLUMN] =
{
	{0,0,0,0,0,0,0,0,0},
	//大R
	{US_CLASS_PAY_ONE,US_CLASS_NOT_REAL,US_CLASS_PAY_TOW,US_CLASS_PAY_THREE,US_CLASS_LIKE_WAR,US_CLASS_ACTIVE,US_CLASS_IN_ALLIANCE,US_CLASS_OTHER,US_CLASS_UNKNOWN},
	//中R
	{US_CLASS_PAY_TOW,US_CLASS_NOT_REAL,US_CLASS_PAY_THREE,US_CLASS_LIKE_WAR,US_CLASS_PAY_ONE,US_CLASS_ACTIVE,US_CLASS_IN_ALLIANCE,US_CLASS_OTHER,US_CLASS_UNKNOWN},
	//小R
	{US_CLASS_PAY_THREE,US_CLASS_NOT_REAL,US_CLASS_LIKE_WAR,US_CLASS_PAY_TOW,US_CLASS_PAY_ONE,US_CLASS_ACTIVE,US_CLASS_IN_ALLIANCE,US_CLASS_OTHER,US_CLASS_UNKNOWN},
	//战争
	{US_CLASS_NOT_REAL,US_CLASS_LIKE_WAR,US_CLASS_PAY_THREE,US_CLASS_ACTIVE,US_CLASS_IN_ALLIANCE,US_CLASS_OTHER,US_CLASS_PAY_TOW,US_CLASS_PAY_ONE,US_CLASS_UNKNOWN},
	//活跃
	{US_CLASS_NOT_REAL,US_CLASS_LIKE_WAR,US_CLASS_ACTIVE,US_CLASS_PAY_THREE,US_CLASS_PAY_TOW,US_CLASS_PAY_ONE,US_CLASS_IN_ALLIANCE,US_CLASS_OTHER,US_CLASS_UNKNOWN},
	//有联盟
	{US_CLASS_NOT_REAL,US_CLASS_LIKE_WAR,US_CLASS_ACTIVE,US_CLASS_IN_ALLIANCE,US_CLASS_OTHER,US_CLASS_PAY_THREE,US_CLASS_PAY_TOW,US_CLASS_PAY_ONE,US_CLASS_UNKNOWN},
	//假用户
	{0,0,0,0,0,0,0,0,0},
	//20以下小用户
	{US_CLASS_NOT_REAL,US_CLASS_LIKE_WAR,US_CLASS_ACTIVE,US_CLASS_IN_ALLIANCE,US_CLASS_OTHER,US_CLASS_PAY_THREE,US_CLASS_PAY_TOW,US_CLASS_PAY_ONE,US_CLASS_LOW_LEVEL},
	//其他
	{US_CLASS_NOT_REAL,US_CLASS_OTHER,US_CLASS_IN_ALLIANCE,US_CLASS_ACTIVE,US_CLASS_LIKE_WAR,US_CLASS_PAY_THREE,US_CLASS_PAY_TOW,US_CLASS_PAY_ONE,US_CLASS_UNKNOWN},
};

static int typeMap2[MAP_ROW][MAP_COLUMN] =
{
	{0,0,0,0,0,0,0,0,0},
	//大R
	{US_CLASS_PAY_ONE,US_CLASS_PAY_TOW,US_CLASS_PAY_THREE,US_CLASS_LIKE_WAR,US_CLASS_ACTIVE,US_CLASS_IN_ALLIANCE,US_CLASS_OTHER,US_CLASS_NOT_REAL,US_CLASS_UNKNOWN},
	//中R
	{US_CLASS_PAY_TOW,US_CLASS_PAY_THREE,US_CLASS_PAY_ONE,US_CLASS_LIKE_WAR,US_CLASS_ACTIVE,US_CLASS_IN_ALLIANCE,US_CLASS_OTHER,US_CLASS_NOT_REAL,US_CLASS_UNKNOWN},
	//小R
	{US_CLASS_PAY_THREE,US_CLASS_LIKE_WAR,US_CLASS_PAY_TOW,US_CLASS_PAY_ONE,US_CLASS_ACTIVE,US_CLASS_IN_ALLIANCE,US_CLASS_OTHER,US_CLASS_NOT_REAL,US_CLASS_UNKNOWN},
	//战争
	{US_CLASS_LIKE_WAR,US_CLASS_PAY_THREE,US_CLASS_ACTIVE,US_CLASS_PAY_TOW,US_CLASS_PAY_ONE,US_CLASS_IN_ALLIANCE,US_CLASS_OTHER,US_CLASS_NOT_REAL,US_CLASS_UNKNOWN},
	//活跃
	{US_CLASS_LIKE_WAR,US_CLASS_PAY_THREE,US_CLASS_ACTIVE,US_CLASS_PAY_TOW,US_CLASS_PAY_ONE,US_CLASS_IN_ALLIANCE,US_CLASS_OTHER,US_CLASS_NOT_REAL,US_CLASS_UNKNOWN},
	//有联盟
	{US_CLASS_LIKE_WAR,US_CLASS_ACTIVE,US_CLASS_IN_ALLIANCE,US_CLASS_OTHER,US_CLASS_PAY_THREE,US_CLASS_PAY_TOW,US_CLASS_PAY_ONE,US_CLASS_NOT_REAL,US_CLASS_UNKNOWN},
	//假用户
	{0,0,0,0,0,0,0,0,0},
	//20以下小用户
	{0,0,0,0,0,0,0,0,0},
	//其他
	{US_CLASS_LIKE_WAR,US_CLASS_ACTIVE,US_CLASS_IN_ALLIANCE,US_CLASS_OTHER,US_CLASS_PAY_THREE,US_CLASS_PAY_TOW,US_CLASS_PAY_ONE,US_CLASS_NOT_REAL,US_CLASS_UNKNOWN},
};


int CLogicMap::GetMapUsersJson(unsigned uid, PlatformType platform, Json::Value &data, bool reset)
{
	if (uid == ADMIN_UID)
	{
		data.resize(0);
		return 0;
	}

	vector<MapUserData> userList;
	int ret = GetMapUsers(uid, platform, userList, reset);
	if(ret != 0)
		return ret;

	data.resize(userList.size());
	for(unsigned i = 0; i < userList.size(); i++)
	{
		data[i]["userid"]			= userList[i].uid;
		data[i]["pt"]				= userList[i].platform;
		data[i]["name"]				= userList[i].name;
		data[i]["pic"]				= userList[i].figure_url;
		data[i]["friend"]			= userList[i].is_friend ? 1 : 0;
		data[i]["vt"]				= userList[i].vip_type;
		data[i]["vl"]				= userList[i].vip_level;
		data[i]["level"]			= userList[i].level;
		data[i]["online"]			= IsOnlineUser(userList[i].last_active_time)?1:0;
		data[i]["attacksfrom"]		= userList[i].attacksfrom;
		data[i]["attacksto"]		= userList[i].attacksto;
		data[i]["helpsfrom"]		= userList[i].helpsfrom;
		data[i]["helpsto"]			= userList[i].helpsto;
		data[i]["attackpermitted"]	= userList[i].attack_permit;
		data[i]["allianceid"]		= userList[i].alliance_id;
		data[i]["coalition"]		= userList[i].alliance_name;
		data[i]["retaliate"]		= (userList[i].attacksfrom-userList[i].attacksto>0?userList[i].attacksfrom-userList[i].attacksto:0);
	}
	return 0;
}

int CLogicMap::GetMapUsers(unsigned uid, PlatformType platform, vector<MapUserData> &userList, bool reset)
{
	//get friend and neighbour
	int ret = 0;
	int userType = 0;
	UidList friends;
	CLogicUserBasic logicUserBasic;
	ret = logicUserBasic.GetFriends(uid, platform, friends);
	if(ret != 0)
		return ret;

	UidList neighbours;
	CLogicNeighbour logicNeighbour;
	if (!reset)
	{
		ret = logicNeighbour.GetNeighbours(uid, neighbours);
		if(ret != 0)
			return ret;
	}

	//calc count
	DataUser user;
	CLogicUser logicUser;
	ret = logicUser.GetUserLimit(uid, user);
	if(ret != 0)
		return ret;
	CLogicAlliance logicAlliance;
	vector<DataAllianceMember> amembers;
	UidList membersUid;
	amembers.clear();
	membersUid.clear();
	if (IsAllianceId(user.alliance_id))
	{
		ret = logicAlliance.GetMembers(user.alliance_id, amembers);
		if (0 == ret)
		{
			//remove alliance member from neighbours
			for (vector<DataAllianceMember>::iterator itr=amembers.begin(); itr!=amembers.end(); ++itr)
			{
				membersUid.push_back(itr->uid);
				UidList::iterator itrFind = find(neighbours.begin(), neighbours.end(), itr->uid);
				if(itrFind != neighbours.end())
				{
					neighbours.erase(itrFind);
				}

				itrFind = find(friends.begin(), friends.end(), itr->uid);
				if (itrFind != friends.end())
				{
					friends.erase(itrFind);
				}
			}
		}
	}

	int group = LEVEL_2_GROUP(user.level);
	userList.clear();
	int groupCount[MAP_GROUP_RANGE * 2 + 1];
	memset(groupCount, 0, sizeof(groupCount));

	//remove friend from neighbour
	UidList tempFriend;
	tempFriend.clear();
	for(UidList::iterator itr = friends.begin(); itr != friends.end(); itr++)
	{
		UidList::iterator itrFind = find(neighbours.begin(), neighbours.end(), *itr);
		if(itrFind != neighbours.end())
		{
			neighbours.erase(itrFind);
		}

		if (*itr == ADMIN_UID)
			continue;
		MapUserData userData;
		ret = GetUserData(*itr, platform, user, true, userData);
		if(ret == 0)
		{
			if (   userData.attack_permit != APT_BAN_DAMAGE_PROTECT
				&& userData.attack_permit != APT_BAN_TOO_MANY_ATTACK
				&& userData.level > group - MAP_GROUP_RANGE
				&& userData.level < group + MAP_GROUP_RANGE)
			{
				userList.push_back(userData);
				tempFriend.push_back(*itr);
				groupCount[userData.level + MAP_GROUP_RANGE - group]++;
			}
		}
	}
	friends = tempFriend;

	int playTime = Time::GetGlobalTime() - user.register_time;
	int timeCount = playTime / (3 * 24 * 60 * 60);
	if(timeCount > 15)
	{
		timeCount = 15;
	}
	int count = 5 + user.level + timeCount;
	if(count > 60)
	{
		count = 60;
	}
	if(count / 2 < (int)friends.size())
	{
		count = count / 2;
	}
	else
	{
		count = count - friends.size();
	}

	//refresh neighbour
	for(UidList::iterator itr = neighbours.begin(); itr != neighbours.end();)
	{
		MapUserData userData;
		ret = GetUserData(*itr, platform, user, false, userData);
		if(ret == 0)
		{
			int neighbourGroup = LEVEL_2_GROUP(userData.level);
			if(    neighbourGroup < group - MAP_GROUP_RANGE
				|| neighbourGroup > group + MAP_GROUP_RANGE
				|| userData.attack_permit == APT_BAN_DAMAGE_PROTECT
				|| userData.attack_permit == APT_BAN_TOO_MANY_ATTACK)
			{
				itr = neighbours.erase(itr);
			}
			else
			{
				userList.push_back(userData);
				groupCount[neighbourGroup + MAP_GROUP_RANGE - group]++;
				itr++;
			}
		}
		else
		{
			itr++;
		}
	}

	//get map db
	CDataMap *pDbMap = NULL;
	static map<int, map<int, CDataMap*> > s_domainMap;
	map<int, map<int, CDataMap*> >::iterator domainMapItr;
	map<int, CDataMap*> mapTable;
	map<int, CDataMap*>::iterator itrMapTable;

	bool bNewMapp = false;
	int domain = 0;
	Config::GetDomain(domain);
	domainMapItr = s_domainMap.find(domain);
	if ( domainMapItr != s_domainMap.end())
	{
		mapTable = domainMapItr->second;
	}
	else
	{
		bNewMapp = true;
	}

	//static map<int, CDataMap*> s_mapTable;
	//map<int, CDataMap*>::iterator itrMapTable;
	itrMapTable = mapTable.find(platform);
	if(itrMapTable != mapTable.end())
	{
		pDbMap = itrMapTable->second;
	}
	else
	{
		if(    OpenPlatform::GetPlatform() != NULL
			&& OpenPlatform::GetPlatform()->GetConfig(CONFIG_INDEPEND) == "1")
		{
			string mapDataFile;
			String::Format(mapDataFile, "%s.%d", Config::GetValue(CONFIG_MAP_DATA).c_str(), platform);
			pDbMap = new CDataMap;
			ret = pDbMap->Init(mapDataFile);
			if(ret != 0)
			{
				delete pDbMap;
				DB_ERROR_RETURN_MSG("map_init_fail");
			}
			mapTable[platform] = pDbMap;
		}
		else
		{
			itrMapTable = mapTable.find(0);
			if(itrMapTable != mapTable.end())
			{
				pDbMap = itrMapTable->second;
				mapTable[platform] = pDbMap;
			}
			else
			{
				pDbMap = new CDataMap;
				ret = pDbMap->Init(Config::GetValue(CONFIG_MAP_DATA));
				if(ret != 0)
				{
					delete pDbMap;
					DB_ERROR_RETURN_MSG("map_init_fail");
				}
				mapTable[0] = pDbMap;
				mapTable[platform] = pDbMap;
				if (bNewMapp)
				{
					s_domainMap[domain] = mapTable;
				}
			}
		}
	}

	logicUser.GetUserClass(uid, userType);
	//set map user data
	ret = pDbMap->SetUser(uid, group, userType);
	if(ret != 0)
	{
		DB_ERROR_RETURN_MSG("map_set_user_fail");
	}

	if (userType == US_CLASS_NOT_REAL)
	{
		return 0;
	}

	//add neighbour
	int countAdd = count - (int)neighbours.size() - 1;
	while(countAdd > 0)
	{
		int groupStart = 0;
		if(group < MAP_GROUP_RANGE)
		{
			groupStart = MAP_GROUP_RANGE - group;
		}
		int groupEnd = COUNT_OF(groupCount);
		while(countAdd > 0)
		{
			//get new user
			int groupAdd = group;
			int minCount = 0x7fffffff;
			for(int i = groupStart; i < groupEnd; i++)
			{
				if(groupCount[i] < minCount)
				{
					minCount = groupCount[i];
					groupAdd = i + group - MAP_GROUP_RANGE;
				}
			}
			unsigned uidNew;
			//ret = pDbMap->GetUser(groupAdd, uidNew, *rankRule);
			ret = GetUserByType(pDbMap, groupAdd, uidNew, userType, user.level);
			if(ret == 0)
			{
				if (   uidNew != uid
					&& uidNew != ADMIN_UID
					&& find(friends.begin(), friends.end(), uidNew) == friends.end()
					&& find(neighbours.begin(), neighbours.end(), uidNew) == neighbours.end()
					&& find(membersUid.begin(), membersUid.end(), uidNew) == membersUid.end()
					)
				{
					MapUserData userData;
					ret = GetUserData(uidNew, platform, user, false, userData);
					if(    ret == 0
						&& userData.attack_permit != APT_BAN_DAMAGE_PROTECT
						&& userData.attack_permit != APT_BAN_TOO_MANY_ATTACK)
					{
						userList.push_back(userData);
						neighbours.push_back(uidNew);
					}
				}
			}
			groupCount[groupAdd + MAP_GROUP_RANGE - group]++;
			countAdd--;
		}
	}

	//update neighbour
	ret = logicNeighbour.SetNeighbours(uid, neighbours);
	if(ret != 0)
	{
		return ret;
	}

	return ret;
}

int CLogicMap::GetUserData(unsigned uid, PlatformType platform, const DataUser& userQuery, bool isFriend, MapUserData &userData)
{
	int ret = 0;
	DataUserBasic userBasic;
	CLogicUserBasic logicUserBasic;
	ret = logicUserBasic.GetUserBasicLimitSmart(uid, platform, userBasic);
	if(ret != 0)
	{
		return ret;
	}

	DataUser user;
	CLogicUser logicUser;
	ret = logicUser.GetUserLimit(uid, user);
	if(ret != 0)
	{
		return ret;
	}

	if (IsOnlineUser(user.last_active_time))
	{
		return R_ERR_DATA;
	}

	userData.uid = uid;
	userData.platform = (PlatformType)userBasic.platform;
	userData.open_id = userBasic.open_id;
	userData.name = userBasic.name;
	userData.gender = userBasic.gender;
	userData.figure_url = userBasic.figure_url;
	userData.is_friend = isFriend;
	userData.vip_type = userBasic.vip_type;
	userData.vip_level = userBasic.vip_level;
	userData.game_vip_level = user.viplevel;
	userData.level = user.level;
	userData.last_active_time = user.last_active_time;
	AttackPermitType permit = APT_BAN_OTHER;
	GetAttackPermit(user, userQuery, permit);
	userData.attack_permit = permit;
	userData.attackable_in = 0;

	userData.attacksfrom = 0;
	userData.attacksto = 0;
	userData.helpsfrom = 0;
	userData.helpsto = 0;
	userData.retaliate_count = 0;

	DataUserInteract interact;
	CLogicUserInteract logicInteract;
	ret = logicInteract.GetInteract(userQuery.uid, uid, interact);
	if (ret == 0)
	{
		userData.attacksfrom = interact.attack_from;
		userData.attacksto = interact.attack_to;
		userData.helpsfrom = interact.help_from;
		userData.helpsto = interact.help_to;
		userData.retaliate_count = interact.retaliate_count;
	}

	userData.alliance_id = 0;
	userData.alliance_name.clear();
	if(IsAllianceId(user.alliance_id))
	{
		CLogicAlliance logicAlliance;
		DataAlliance alliance;
		ret = logicAlliance.GetAllianceLimit(user.alliance_id, alliance);
		if(ret == 0)
		{
			userData.alliance_id = alliance.alliance_id;
			userData.alliance_name = alliance.name;
		}
	}

	return 0;
}

int CLogicMap::GetAttackPermit(const DataUser &user, const DataUser& userQuery, AttackPermitType &permit)
{
	if (user.uid == ADMIN_UID)
	{
		permit = APT_BAN_ADMIN;
		return 0;
	}
	if (user.uid == userQuery.uid)
	{
		permit = APT_BAN_SELF;
		return 0;
	}
	if(IsAllianceId(user.alliance_id) && user.alliance_id == userQuery.alliance_id)
	{
		permit = APT_BAN_ALLIANCE;
		return 0;
	}
	if(CLogicUser::GetFlag(user.bit_info, UF_BE_ATTACKED)
	//20131107 Ralf last_save_uid -> last_load_save_uid : fix for bugs, can save only last load archieve
			//&& user.last_save_uid != userQuery.uid
	//20131107 Ralf last_breath_time -> last_attacked_time : fix for online attacking bugs
			&& Time::GetGlobalTime() < IsBeingAttacked(user.last_breath_time))
	{
		//debug_log("[load attack be attacked,uid=%u,userid=%u,last_breath_time=%u]",user.uid,userQuery.uid,user.last_breath_time);
		permit = APT_BAN_BEING_ATTACK;
		return 0;
	}
	if(IsOnlineUser(user.last_active_time) && Time::GetGlobalTime() < ATTACK_PRO_TIME + user.last_breath_time)
	{
		//debug_log("[load attack be protected,uid=%u,userid=%u,last_breath_time=%u]",user.uid,userQuery.uid,user.last_breath_time);
		permit = APT_BAN_DAMAGE_PROTECT;
		return 0;
	}
	CLogicUserInteract logicInteract;
	DataUserInteract interact;
	memset(&interact, 0, sizeof(DataUserInteract));
	int ret = logicInteract.GetInteract(userQuery.uid, user.uid, interact);
	if (ret != 0 && ret != R_ERR_NO_DATA)
	{
		permit = APT_BAN_OTHER;
		return ret;
	}
	if (Time::GetGlobalTime() < interact.truce_time)
	{
		permit = APT_BAN_TRUCE;
		return 0;
	}
	if (interact.attack_to >= 10 + interact.attack_from)
	{
		permit = APT_BAN_TOO_MANY_ATTACK;
		return 0;
	}
	permit = APT_ALLOW;
	if(IsInProtect(user.protected_time))
	{
		if(Time::GetGlobalTime() < NEW_USER_PROTECT + user.register_time)
			permit = APT_BAN_NEW_PROTECT;
		else
			permit = APT_BAN_DAMAGE_PROTECT;
	}
	else if (userQuery.level > user.level + 10)
	{
		if (userQuery.level<50 || user.level<50)
		{
			permit = APT_BAN_LEVEL_LOW;
			if (IsAllianceId(userQuery.alliance_id))
			{
				DataUserInteract allianceInteract;
				ret = logicInteract.GetInteract(userQuery.alliance_id, user.uid, allianceInteract);
				if(ret == 0)
				{
					if(allianceInteract.retaliate_count > 0)
					{
						permit = APT_ALLOW_ALLIANCE_REVENGE;
					}
				}
			}
		}
	}
	else if(userQuery.level + 10 < user.level)
	{
		if (userQuery.level<50 || user.level<50)
		{
			permit = APT_ALLOW_LEVEL_HIGH;
			if (IsAllianceId(userQuery.alliance_id))
			{
				DataUserInteract allianceInteract;
				ret = logicInteract.GetInteract(userQuery.alliance_id, user.uid, allianceInteract);
				if(ret == 0)
				{
					if(allianceInteract.retaliate_count > 0)
					{
						permit = APT_ALLOW_ALLIANCE_REVENGE;
					}
				}
			}
		}
	}
	if (!IsAllowAttack(permit))
	{
		if (interact.attack_from > interact.attack_to)
		{
			permit = APT_ALLOW_REVENGE;
		}
	}
	if (IsAllowAttack(permit))
	{
		if(IsOnlineUser(user.last_active_time))
		{
			permit = APT_BAN_ONLINE;
		}
	}
	return 0;
}

int CLogicMap::GetUserByType(CDataMap *pDbMap, int group, unsigned &uidNew, int type, int level)
{
	int ret = 0;
	const int *rankRule = NULL;
	if (level <= 30)
	{
		rankRule = typeMap[type];
	}
	else
	{
		rankRule = typeMap2[type];
	}
	int rankRuleNum = 0;
	while(rankRuleNum < MAP_COLUMN)
	{
		if (rankRule == 0)
		{
			rankRule++;
			rankRuleNum++;
			continue;
		}
		if (group < 20)
		{
			ret = pDbMap->GetUser(group, uidNew, US_CLASS_LOW_LEVEL);
		}
		else
		{
			ret = pDbMap->GetUser(group, uidNew, *rankRule);
		}
		if (0 == ret)
		{
			return R_SUCCESS;
		}
		rankRuleNum++;
		rankRule++;
	}
	return R_ERR_DATA;
}

int CLogicMap::GetChallenger(unsigned uid, unsigned &challenger, PlatformType platform)
{
	CLogicUser logicUser;
	DataUser user;
	int ret = logicUser.GetUser(uid, user);
	if (0 != ret)
	{
		return ret;
	}

	vector<MapUserData> userList;
	ret = GetMapUsers(uid, platform, userList, false);
	if(ret != 0)
		return ret;
	vector<MapUserData>::iterator itr = userList.begin();
	for (; itr!=userList.end(); ++itr)
	{
		if (itr->level >= user.level || itr->game_vip_level >= user.viplevel || itr->game_vip_level > 0)
		{
			challenger = itr->uid;
			return R_SUCCESS;
		}
	}
	return R_ERR_DATA;
}

