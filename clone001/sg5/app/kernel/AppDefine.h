//AppDefine.h
//20110525	dada	create

#ifndef __APPDEFINE_H__
#define __APPDEFINE_H__

//config

#define APP_CONFIG_PATH			"../conf/app_config.xml"
#define APP_DOMAIN_CONFIG_PATH  "../../pi/conf/domai_config.xml"
#define CONFIG_PLATFORM_MAPPING "platform_mapping"
#define CONFIG_PLATFORM			"platform_config"
#define CONFIG_WHITE_LIST		"white_list"
#define CONFIG_ADMIN_LIST		"admin_list"
#define CONFIG_AUTO_LOGIN_URL	"auto_login_url"
#define CONFIG_DATA_LOG_PATH	"data_log_path"
#define CONFIG_ALL_DATA_LOG_PATH	"all_data_log"
#define CONFIG_DBC_SERVER		"dbc_server"
#define CONFIG_LOG_LEVEL		"log_level"
#define CONFIG_LOG_PATH			"log_path"
#define CONFIG_LOG_NAME			"log_name"
#define CONFIG_LOG_REWIND		"log_rewind"
#define CONFIG_LOG_SIZE			"log_size"
#define CONFIG_LOG_RECORD_SIZE	"log_record_size"
#define CONFIG_STRING_FILTER_DATA "string_filter_data"
#define CONFIG_INIT_ARCHIVE		"init_archive"
#define CONFIG_CLIENT_DOMAIN	"client_domain"
#define CONFIG_GATE_RAND_DIR	"gate_rank_dir"
#define CONFIG_INDEPEND			"independ"
#define CONFIG_MAP_DATA			"map_data"
#define CONFIG_NPC_DATA			"npc_data"
#define CONFIG_INSTANCE_DATA	"instance_data"
#define CONFIG_BROADCAST_DATA	"broadcast_data"
#define CONFIG_ARENA_DIR		"arena_dir"
#define CONFIG_VEDIO_DIR		"vedio_dir"
#define CONFIG_FIGHTING_DATA	"fighting_data"
#define CONFIG_TCP_SERVER		"tcp_server"
#define CONFIG_TCP_HOST			"tcp_host"
#define ADMIN_OP_LOG_DIR		"admin_log_dir"
#define CONFIG_WORLDBOSS_DIR	"worldboss_dir"
#define CONFIG_MIN_WORLDBOSS_BLOOD	"min_worldboss_blood"
#define CONFIG_MAX_WORLDBOSS_BLOOD	"max_worldboss_blood"
#define CONFIG_WORLDBOSS_MAP	"worldboss_map"
#define CONFIG_MATCH_DAT_PATH	"match_path"
#define CONFIG_BASE_MATCH_PATH	"base_match_path"
#define CONFIG_ALL_SERVER_BASE_MATCH_PATH	"all_server_base_match_path"
#define CONFIG_PERSON_MATCH_PATH	"person_match_path"
#define CONFIG_ALL_SERVER_PERSON_MATCH_PATH	"all_server_person_match_path"
#define CONFIG_DRAGONBALL_DATA	"dragonball_data"
//#define CONFIG_DRAGONBALL_ON	"dragonball_on"		//龙珠活动开关，1为打开，否则关闭
#define CONFIG_QQ_ITEM_INFO		"qq_item_info"
#define CONFIG_ZONE_ID			"zoneid"
#define CONFIG_LADDER_PATH		"ladder_path"
#define CONFIG_ALL_SERVER_LADDER_PATH		"all_server_ladder_path"
#define CONFIG_WORLD_DATA        "world_data"
#define CONFIG_FORBID_USER      "forbid_user"
#define POINTS_RANK              "points_rank"
#define HUNDREDDAYSACTIVITY_RANK  "hundreddaysactivity_rank"   //百日消费活动
#define QIXIJIE_PAY_RANK        "qixijiepay_rank"   //七夕节
#define NATIONAL_WAR_RGT        "nationalwar_rgt"   //国战报名
#define GROUPS_FIGHTING         "groups_fighting"   //国战战斗
#define CONFIG_AID_START        "aid_start"
#define CONFIG_UID_MIN          "uid_min"
#define CONFIG_UID_MAX          "uid_max"
#define CONFIG_NOMENAT_FRIEND_UID "nomenat_friend_uid"
#define CONFIG_ADMIN_UID      "admin_uid"
#define CONFIG_ALLIANCE_EXPENSES "alliance_expenses"
#define CONFIG_PRIZEMAILTEXT_PATH "prizeText"
#define CONFIG_NOMINATE_ALLIANCE_PATH "nominate_alliance_path"
#define CONFIG_BARBARIANINVASION_PATH "barbarian_invasion_path"
#define CONFIG_BARBARIAN_BEGIN_TS "barbarian_begin_ts"
#define CONFIG_BARBARIAN_END_TS   "barbarian_end_ts"
#define CONFIG_WORLD_DATA   "world_data"
#define CONFIG_WORLDNPC_DATA   "worldnpc_data"
#define CONFIG_OPEN_TS         "open_ts"
#define CONFIG_PAY_TOTAL_BEGIN_TS "paytotal_begin_ts"
#define CONFIG_PAY_TOTAL_END_TS "paytotal_end_ts"
#define CONFIG_PAY_OF_HUNDRED_DAYS_BEGIN_TS "payofhundreddays_begin_ts"    //百日消费活动
#define CONFIG_PAY_OF_HUNDRED_DAYS_END_TS "payofhundreddays_end_ts"
#define CONFIG_PAY_OF_QIXIJIE_BEGIN_TS "payofqixijie_begin_ts"    //七夕节
#define CONFIG_PAY_OF_QIXIJIE_END_TS "payofqixijie_end_ts"
#define CONFIG_MERRYSOULS_BEGIN_TS "merrysoulsbegin_ts"    
#define CONFIG_MERRYSOULS_END_TS "merrysoulsend_ts"
#define CONFIG_MERRYSOULS_VERSION "merrysouls_version"

//活动平台配置
#define CONFIG_MERRYSOULS_PF "merrysouls_pf"
#define CONFIG_PAY_OF_HUNDRED_DAYS_PF "payofhundreddays_pf"
#define CONFIG_PAY_TOTAL_PF "paytotal_pf"
#define CONFIG_POINTS_PF "points_pf"
#define CONFIG_PAYRANK_PF "payrank_pf"


#define CONFIG_NEW_SERVER_TS    "new_server_ts"
#define CONFIG_TOP_TEN_USER_PATH "top_ten_user"
#define CONFIG_POINTS_BEGIN_TS "points_begin_ts"
#define CONFIG_POINTS_END_TS   "points_end_ts"
#define CONFIG_POINTS_VERSION   "points_rank_version"
#define CONFIG_MYSTICALSHOP_RANGE_TS "mysticalshop_range_time"
#define CONFIG_MYSTICALSHOP_PATH "mysticalshop_path"
#define CONFIG_RIVER_PATH "river_path"
#define CONFIG_PAYRANK          "payrank"
#define CONFIG_PAYRANK_BEGIN_TS "payrank_begin_ts"
#define CONFIG_PAYRANK_END_TS   "payrank_end_ts"
#define CONFIG_PAYRANK_VERSION   "payrank_version"
#define CONFIG_GM_LIST_FILE      "/data/release/sgonline/data/game_manager_list.dat"
#define CONFIG_TH_LIST_FILE      "/data/release/sgonline/data/th_manager_list.dat"
#define CONFIG_GM_TH_SHM      "/data/release/sgonline/data/gm_th.dat"
#define CONFIG_NOISE_SHM      "/data/release/sgonline/data/noise.dat"
#define CONFIG_SERVER_ID       "server_id"
#define CONFIG_DRAGONBALL_ACTIVITY_TS "dragon_activity_ts"
#define CONFIG_MATCH_INSTANCE_PATH "match_instance_path"
#define CONFIG_ALLS_MATCH_INST_PATH "all_server_match_instance_path"
#define CONFIG_ALLS_MATCH_SERVER_PATH "all_server_match_server_path"
#define CONFIG_ALL_SERVER "all_server"
#define CONFIG_SUNCE_BEGIN_TS "sunce_begin_ts"
#define CONFIG_SUNCE_END_TS "sunce_end_ts"
#define CONFIG_DEFAULT_KINGDOM "default_kingdom"
#define CONFIG_NEW_USER_REC_LIST_PATH "new_user_rec_list_path"
#define CONFIG_ZONE_ID "zoneid"
#define CONFIG_FULL "full"
#define CONFIG_DOUBLE_BARBARIAN_BEGIN "double_barbarian_begin"
#define CONFIG_DOUBLE_BARBARIAN_END   "double_barbarian_end"

#define CONFIG_LEVEL_RESTRICT 60
//DB
//对应DBC端口号和共享内存号为39100+ID
#define DB_PAY				1
#define DB_PAY_HISTORY		2
#define DB_USER_BASIC		3
#define DB_USER				4
#define DB_QUEST			5
#define DB_BUILDING			6
#define DB_EQUIPMENT		7
#define DB_GATE				8
#define DB_HERO				9
#define DB_TRIBUTE			10
#define DB_UPDATES			11
#define DB_NEIGHBOUR		12
#define DB_USER_INTERACT	13
#define DB_ATTACK			14
#define DB_USER_ATTACK		15
#define DB_MESSAGE			16
#define DB_USER_MESSAGE		17
#define DB_USER_DATA		18
#define DB_USER_TEXT		19
#define DB_INVENTORY		20
#define DB_PLATFORM_USER	21
#define DB_ALLIANCE			22
#define DB_ALLIANCE_MEMBER	23
#define DB_ALLIANCE_MAPPING	24
#define DB_INSTANCE			25
#define DB_SECINC			26
#define DB_EMAIL			27
#define DB_INVITE_MAPPING   28
#define DB_BASE_EXTRA       29
#define DB_HELP_HISTORY     30
#define DB_EXCHANGE_CODE     31
#define DB_PROMOTION_MAPPINT 32
#define DB_ALLIANCE_PAY      33

#define DB_USER_RANK      	 34
#define DB_GOODS             35
#define DB_MARKET_TASK	     36
#define DB_WEAPON	         37
#define DB_BLACK_USER        38
#define DB_ALLIANCE_MSG      39
#define DB_ALLIANCE_INTERACT 40
#define DB_MATCH_INSTANCE    41


#define MEM_SESSION			91
#define DB_USER_MAPPING		92
#define DB_ID_CTRL			93
#define DB_USER_NOMENATE	95
#define DB_USER_ACCCHARGE	96

//DB other
#define KEY_UID_CTRL		1
#define KEY_PAY_ID_CTRL 	2
#define KEY_ATTACK_ID_CTRL	3
#define KEY_MESSAGE_ID_CTRL	4
#define KEY_ALLIANCE_ID_CTRL 5
#define KEY_SECINC_ID_CTRL	6
#define KEY_PROMOTION_ID_CTRL     7
#define KEY_EXCHANGE_CODE_ID_CTRL 8

//app define
#define ADMIN_UID			10000000
#define UID_MIN				10000000
#define UID_MAX				2000000000
#define IsValidUid(uid)		(uid >= UID_MIN && uid <= UID_MAX)
#define NPC_UID_MIN			1
#define NPC_UID_MAX			500
#define IsNPCUid(uid)		((uid >= NPC_UID_MIN && uid <= NPC_UID_MAX) || (uid >= WORLD_NPC_UID_MIN && uid <= WORLD_NPC_UID_MAX))
#define IsGateNPCUid(uid)		(uid >= 1 && uid <= 192 )
#define WORLD_NPC_UID_MIN	70001
#define WORLD_NPC_UID_MAX	90000
#define IsWorldNPCUid(uid)	(uid >= WORLD_NPC_UID_MIN && uid <= WORLD_NPC_UID_MAX)
#define ALLIANCE_ID_NULL	0
#define ALLIANCE_ID_MIN		4000000000
#define ALLIANCE_ID_START	4010000000
#define IsAllianceId(id)	(id >= ALLIANCE_ID_MIN)
#define IsLegalUid(uid)		(uid >= UID_MIN)
#define ARENA_ID_MIN		10001
#define ARENA_ID_MAX		10015
#define IsValidArenaId(id) (id >= ARENA_ID_MIN && id <= ARENA_ID_MAX)
#define INSTANCE_ID_MIN		20001
#define INSTANCE_ID_MAX		20025
#define IsValidInstanceId(id) (id >= INSTANCE_ID_MIN && id <= INSTANCE_ID_MAX)
#define FIGHTING_ID_MIN		30001
#define FIGHTING_ID_MAX		30001
#define IsValidFightingId(id) (id >= FIGHTING_ID_MIN && id <= FIGHTING_ID_MAX)
#define FIGHT_ARENA_ID_MIN	40001    //擂台
#define FIGHT_ARENA_ID_MAX	40015
#define IsValidFightArenaId(id) (id >= FIGHT_ARENA_ID_MIN && id <= FIGHT_ARENA_ID_MAX)
#define WORLD_BOSS_ID_MIN	50001
#define WORLD_BOSS_ID_MAX	50004
#define IsValidWorldBossId(id)	(id >= WORLD_BOSS_ID_MIN && id <= WORLD_BOSS_ID_MAX)
#define ALLIANCE_BOSS_ID	50005   //帮会屠魔
#define IsValidAllianceBossId(id)	(id == ALLIANCE_BOSS_ID)
#define SESSION_TIME_OUT	3600	//会话超时
typedef vector<unsigned>	UidList;
#define MULTI_BATTLE_ID_MIN 60001
#define MULTI_BATTLE_ID_MAX 60015
#define IsValidMultiBattleId(id) (id >= MULTI_BATTLE_ID_MIN && id <= MULTI_BATTLE_ID_MAX)
#define BASEMATCH_INST_UID	100000
#define BMATCH_INST_ID_MIN	100001
#define BMATCH_INST_ID_MAX	200000
#define IsValidBMatchInstId(id)	(id >= BMATCH_INST_ID_MIN && id <= BMATCH_INST_ID_MAX)
#define PERSONMATCH_INST_UID 200000
#define PERSONMATCH_INST_ID_MIN  200001
#define PERSONMATCH_INST_ID_MAX	 220000
#define IsValidPersonMatchId(id) (id >= PERSONMATCH_INST_ID_MIN && id <= PERSONMATCH_INST_ID_MAX)
#define ALL_SERVER_BMATCH_INST_ID_MIN	300001
#define ALL_SERVER_BMATCH_INST_ID_MAX	400000
#define IsValidAllServerBMatchInstId(id)	(id >= ALL_SERVER_BMATCH_INST_ID_MIN && id <= ALL_SERVER_BMATCH_INST_ID_MAX)
#define ALL_SERVER_PERSONMATCH_INST_ID_MIN	400001
#define ALL_SERVER_PERSONMATCH_INST_ID_MAX	500000
#define IsValidAllServerPersonMatchId(id)	(id >= ALL_SERVER_PERSONMATCH_INST_ID_MIN && id <= ALL_SERVER_PERSONMATCH_INST_ID_MAX)


#define IsValidWorldType(type) (type >0 && type < 20)
#define MAX_WORLD_NPC_COUNT 10000

#define NEW_USER_CASH		0	//新用户钻石数
#define NEW_USER_COINS		200  //新用户金币数
#define NEW_USER_R1			7000
#define NEW_USER_R1_MAX		7000
#define NEW_USER_R2			7000
#define NEW_USER_R2_MAX		7000
#define NEW_USER_R3			7000
#define NEW_USER_R3_MAX		7000
#define NEW_USER_R4			7000
#define NEW_USER_R4_MAX		7000
#define NEW_USER_R5			7000
#define NEW_USER_R5_MAX		7000
#define TOTORIAL_FINISH		10000
#define TRIBUTE_INTERVAL	(3*3600)
#define NEW_DAILY_GCBASE	200
#define DAILY_GCBASE		0
#define DAILY_NEW_GCBASE	40
#define GCBASE_LIMIT		300
#define ONLINE_TIMEOUT		60	//在线冷却时间
#define ATTACK_TIMEOUT		90	//攻击冷却时间
#define ATTACK_MAX_TIME		330	//最大进攻时间
#define ATTACK_PRO_TIME		600	//被攻击保护10分钟，别人不能攻击
#define  SUBBASE_PROTECTED_LEVEL_1 (3600 * 6)
#define  SUBBASE_PROTECTED_LEVEL_2 (3600 * 12)

#define ATTACK_HISTORY_TIME	(14*24*60*60)	//进攻日志时间
#define MAX_ATTACK_HISTORY	66	//最大进攻历史数
#define TRUCE_TIME			(7*24*3600)
#define NEW_USER_PROTECT	(3*60*60)	//新手保护时间
#define DAMAGE_PROTECT_LV1	(18*60*60)	//破坏保护时间1
#define DAMAGE_PROTECT_LV2	(36*60*60)	//破坏保护时间2
#define CON_PROTECT_TIME	3600		//连续被攻击保护时间
#define GET_ALLIANCE_COUNT	10	//每次取联盟数
#define WORLD_OCCUPT_NEW_PROTECT_TIME  (3600)
#define ALLIANCE_ACTIVE_COUNT 10	//联盟动态数
#define ALLIANCE_MEMBER_ATTACK_COUNT 10	//联盟成员被攻击记录数
#define ALLIANCE_ENEMY_TIME (1 * 7 * 24 * 60 * 60)	//联盟仇敌记录时间
#define GET_EMAIL_TIME_LIMIT (2 * 31 * 24 * 60 * 60)

#define IsOnlineUser(t)		(Time::GetGlobalTime()<ONLINE_TIMEOUT+(t))
#define IsInProtect(t)		(Time::GetGlobalTime()<(t))
#define IsBeingAttacked(t)	(Time::GetGlobalTime()<ATTACK_TIMEOUT+(t))
#define IsAttackFinished(t)	(Time::GetGlobalTime()>ATTACK_MAX_TIME+(t))
#define IsValidWorldPos(worldpos) (((worldpos / 1000) >=1) && ((worldpos / 1000) <=500) && ((worldpos % 1000 >=1) && ((worldpos % 1000) <= 500)))
#define CONFIG_BUSINESS_LOG_DIR "business_log_dir"
//session
#define SESSION_DATA		"data"

//error
#define KEY_RESULT			"error"
#define KEY_ERROR_MESSAGE	"errMsg"
#define KEY_PARAMS			"params"

#define SET_NPC_BIT(data, npcId)	data[(npcId-1)/8] |= (1<<(npcId-1)%8)
#define CLEAR_NPC_BIT(data, npcId)	data[(npcId-1)/8] &= ~(1<<(npcId-1)%8)
#define IS_SET_NPC_BIT(data, npcId)	data[(npcId-1)/8] & (1<<(npcId-1)%8)

//exchange code
#define CODE_SIZE 12


enum LoadType
{
	LT_BUILD,		//建造
	LT_VIEW, 		//查看
	LT_HELP,		//帮助朋友
	LT_ATTACK,		//攻击
	LT_SPORTS,
	LT_SUBBASE,
	LT_UNKNOW
};

#endif	//__APPDEFINE_H__
