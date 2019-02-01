#ifndef LOGICMAP_H_
#define LOGICMAP_H_

#include "LogicInc.h"

struct MapUserData
{
	unsigned uid;
	PlatformType platform;
	string open_id;
	string name;
	int gender;
	string figure_url;
	bool is_friend;
	int vip_type;
	int vip_level;
	int game_vip_level;
	int level;
	unsigned last_active_time;
	int attacksfrom;
	int attacksto;
	int helpsfrom;
	int helpsto;
	int attack_permit;
	int attackable_in;
	int retaliate_count;
	unsigned alliance_id;
	string alliance_name;
};

enum AttackPermitType
{
	APT_ALLOW					= 1,	//可以进攻，客户端判断自己保护期提醒和好友攻打提醒
	APT_ALLOW_LEVEL_HIGH		= 2,	//不可以进攻，对方等级高
	APT_BAN_LEVEL_LOW			= 3,	//对方等级低
	APT_ALLOW_REVENGE			= 4,	//可以进攻，复仇
	APT_BAN_DAMAGE_PROTECT		= 5,	//破坏保护
	APT_BAN_NEW_PROTECT			= 6,	//新手保护
	APT_BAN_BEING_ATTACK		= 7,	//对方被攻击
	APT_BAN_ONLINE				= 8,	//对方在线
	APT_BAN_TRUCE				= 9, 	//讲和
	APT_BAN_ADMIN				= 10,	//管理员不可攻击
	APT_BAN_SELF				= 11,
	APT_ALLOW_ALLIANCE_REVENGE	= 12,	//可以进攻，联盟复仇
	APT_BAN_ALLIANCE			= 13,	//同联盟
	APT_BAN_TOO_MANY_ATTACK		= 14,
	APT_BAN_OTHER				= 15,
	APT_BAN_WORLD_PROTECT       = 16
};
#define IsAllowAttack(permit) (permit==APT_ALLOW||permit==APT_ALLOW_REVENGE||permit==APT_ALLOW_ALLIANCE_REVENGE)

class CLogicMap
{

public:

	int GetMapUsersJson(unsigned uid, PlatformType platform, Json::Value &data, bool reset = false);
	int GetMapUsers(unsigned uid, PlatformType platform, vector<MapUserData> &userList, bool reset = false);
	int GetUserData(unsigned uid, PlatformType platform, const DataUser& userQuery, bool isFriend, MapUserData & userData);
	int GetUserByType(CDataMap *pDbMap, int group, unsigned &uidNew, int type, int level);
	int GetChallenger(unsigned uid, unsigned &challenger, PlatformType platform);

	//返回attack permit userQuery 进攻者 , user 被攻击者
	int GetAttackPermit(const DataUser &user, const DataUser& userQuery, AttackPermitType &permit);

};

#endif /* LOGICMAP_H_ */
