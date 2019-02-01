/*
 * DataWorld.h
 *
 *  Created on: 2012-8-21
 *      Author: Administrator
 */

#ifndef DATAWORLD_H_
#define DATAWORLD_H_
#include "Kernel.h"
#define WORLD_LENGTH_OF_SIDE 500
#define MOVE_COUNT  100
#define MIN_CID 10000000
#define MAX_SUBARENA_COUNT 500
#define NEW_USER_INIT_SIZE 500
#define WORLD_INIT_RECYCLE (3600 * 24 * 7)
#define WORLD_SIZE (WORLD_LENGTH_OF_SIDE * WORLD_LENGTH_OF_SIDE)
#define IsValidIndex(x,y) (x >= 0 && x < WORLD_LENGTH_OF_SIDE && y >= 0 && y < WORLD_LENGTH_OF_SIDE)
enum TYPE
{
	USER_MAIN_CITY    = 1,  // 主城基地
	USER_BRANCH_CITY  = 2,  // 分基地
	NPC_TYPE_1        = 3,  // 资源矿
	NPC_TYPE_2        = 4,  // 资源矿
	NPC_TYPE_3        = 5,  // 资源矿
	NPC_TYPE_4        = 6,  // 资源矿
	NPC_TYPE_5        = 7,  // 资源矿
	NPC_TYPE_6        = 8,  // 金矿
//	XIAN_CITY         = 10, //
	JUN_CITY          = 11, // 郡
	ZHOU_CITY         = 12, // 州
	DU_CITY          = 13, // 都
};
enum STATE{
	CITY_STATE_NORMAL = 0,
	CITY_STATE_BATTLE = 1,
	CITY_STATE_NOBATTLE = 2,
	CITY_STATE_VACATION = 3,
	CITY_STATE_FREEZE = 4 ,
	CITY_STATE_FRESHMAN = 5
};
enum USER_STATE{
	USER_STATE_NORMAL = 1,
	USER_STATE_FRESHMAN = 2,
	USER_STATE_NOBATTLE = 3,
	USER_STATE_FREEZE = 4,
	USER_STATE_FORBID = 5 ,
};
enum FIELD_STATE{
	WORLD_FIGHT_STATE_FIGHTING = 1,
	WORLD_FIGHT_STATE_COMPLETE = 2
};
struct DataNpc{
	unsigned worldpos;
	unsigned npcId;
	short int npctype;
	string cityname;
};

struct WorldPoint{
	short int map_flag; // 0空地 1草地 2沙漠  3水 4森林 5待续
	short int level;
	short int user_state;
	short int state;
	short int x;
	short int y;
	int unsigned uid;
	int  cid;
	unsigned top_worldpos;
	int unsigned protect_time;
	int unsigned influence_zone;  // 影响力
	uint64_t defensive_power;  // 城防
	uint64_t last_collect_ts;
	int flags;
	char aname[32];
	char name[32];
	char cityname[32];
	WorldPoint &operator =(const WorldPoint &other)
	{
		if(this == &other)
		{
			return *this;
		}
		map_flag = other.map_flag;
		level = other.level;
		user_state = other.user_state;
		state = other.state;
		x = other.x;
		y = other.y;
		uid = other.uid;
		cid = other.cid;
		protect_time = other.protect_time;
		top_worldpos = other.top_worldpos;
		influence_zone = other.influence_zone;
		defensive_power = other.defensive_power;
		last_collect_ts = other.last_collect_ts;
		flags = other.flags;
		memcpy(name,other.name,sizeof(name) - 1);
		memcpy(aname,other.aname,sizeof(aname) - 1);
		memcpy(cityname,other.cityname,sizeof(cityname) - 1);
		return *this;
	}
};

struct World{
	unsigned curr_cid;
	WorldPoint worldPoints[WORLD_LENGTH_OF_SIDE][WORLD_LENGTH_OF_SIDE];
};
class CDataWorld {
public:
	CDataWorld();
	virtual ~CDataWorld();
	int Init(const std::string &path, semdat sem=sem_world);
	int Move(short int begin_x,short int begin_y,short int end_x, short int end_y,vector<WorldPoint> &vWorldPoints);
	int AddUser(const WorldPoint &worldpoint, int unsigned &world_pos);
	int AddUser(const WorldPoint &worldpoint, int unsigned &world_pos,short begin_x, short begin_y,short end_x,short end_y);
	int SetWorldPoint(unsigned uid, int unsigned &world_pos);
	int ChangeWorldPointType(short int begin_x,short int begin_y,short int end_x, short int end_y,short int map_flag);
	int ChangeWorldPointType(short int x, short int y,short int map_flag);
	int ReplaceUser(const WorldPoint &worldpoint, int x,int y);
	int MoveCity(unsigned uid,const unsigned oldWorldPos,const unsigned toWorldPos);
	int AddNpc(unsigned npcid,short x, short y,int npcType,int citysize,char name[32]);
	int AddNpc(unsigned npcid,short begin_x, short begin_y,short end_x,short end_y,int npcType,unsigned &worldpos,int citysize,const string &name );
	int RemoveNpc(unsigned npcid,short x,short y,int npcType,int citysize = 1);
	int RemoveNpc(unsigned npcid,short begin_x, short begin_y,short end_x,short end_y,int npcType,int citysize);
	int RemoveNpcByType(short x,short y,int npctype,int citysize = 1);
	int RemoveNpcByType(short begin_x,short begin_y,short end_x,short end_y,int npctype,int citysize = 1);
	int ViewMap();
	int GetWorldUser(const int world_pos,WorldPoint &worldPoint);
	int GetAllWorldUser(vector<WorldPoint> &worldPoint);
	int SetWorldPoint(const int world_pos,WorldPoint &worldPoint);
	int ReplaceUser(const WorldPoint &worldpoint,unsigned worldpos);
	int GetNpc(short begin_x,short begin_y,short end_x,short end_y,int npctype,vector<DataNpc> &vNpc);
private:
	bool m_init;
	CShareMemory m_sh;
};

#endif /* DATAWORLD_H_ */
