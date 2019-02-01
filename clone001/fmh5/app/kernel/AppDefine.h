//AppDefine.h
//20110525	dada	create

#ifndef __APPDEFINE_H__
#define __APPDEFINE_H__

//config
#define APP_CONFIG_PATH						"../conf/app_config.xml"
#define APP_DOMAIN_CONFIG_PATH  			"../../pi/conf/domai_config.xml"

#define DEFAULT_APP_PATH					"/data/release/sg17/"
#define CONFIG_ALL_DATA_LOG_PATH			"datalog/"
#define CONFIG_DBC_SERVER					"pi/conf/dbc_server.xml"
#define CONFIG_STRING_FILTER_DATA 			"data/stringfilter.dat"
#define CONFIG_JSON_PATH		 			"pi/conf/json/"
#define CONFIG_MAP_PATH			"pi/conf/home.map"

#define DEFAULT_APP_CONFIG_PATH				"conf/app_config.xml"
#define CONFIG_DATA_LOG_PATH				"datalog/"
#define CONFIG_BUSINESS_LOG_DIR 			"log/business/"
#define CONFIG_LOG_PATH						"log/"
#define CONFIG_DATA_PATH 					"data/"
#define CONFIG_SHM_PATH 					"shm/"
#define CONFIG_ROOM_PATH 					"room/"
#define CONFIG_SAVE_PATH 					"save/"

#define CONFIG_LOG_LEVEL		"log_level"
#define CONFIG_LOG_NAME			"log_name"
#define CONFIG_LOG_REWIND		"log_rewind"
#define CONFIG_LOG_SIZE			"log_size"
#define CONFIG_LOG_RECORD_SIZE	"log_record_size"

#define CONFIG_WHITE_LIST		"white_list"
#define CONFIG_ADMIN_LIST		"admin_list"
#define CONFIG_SRVID "server_id"
#define CONFIG_OPENTIME "open_ts"
#define CONFIG_BASE "base"

#define BASE_FORBID_REASON_LEN 32
#define BASE_NAME_LEN 32
#define BASE_FIG_LEN 128

//延迟下线时间(秒)
#define OFFLINE_DELAY 60
#define TUTOR_MAX 55555

enum PlatformType
{
	PT_UNKNOW 		= -1,	//未知
	PT_TEST 		= 0,	//测试平台
	PT_FACEBOOK 	= 1,	//Facebook平台中文版
	PT_PENGYOU		= 2, 	//朋友平台
	PT_RENREN 		= 3,	//人人平台
	PT_QZONE 		= 4,	//QZONE平台
	PT_SINAWB 		= 5,	//新浪微博
	PT_WEIYOUXI 	= 6,	//新浪微游戏
	PT_KUAIWAN 		= 7,	//快玩平台
	PT_MIXI 		= 8,	//Mixi平台
	PT_TXWEIBOAPP 	= 9,	//腾讯微博应用平台
	PT_MOBAGE 		= 10,	//日本Yahoo!Mobage平台
	PT_TXWEIBO 		= 11,	//腾讯微博
	PT_KAIXIN 		= 12,	//开心网
	PT_FETION 		= 13,	//飞信
	PT_VN 			= 14,	//英文版自建平台
	PT_3366 		= 15,	//3366
	PT_qqgame		= 16,	//qq游戏大厅
	PT_TW			= 17, 	//台湾BSG
	PT_EN			= 18,	//FB英文版
	PT_DO			= 19,	//英文版联运
	PT_4399			= 20,	//4399联运
	PT_C9			= 21,	//C9
	PT_SOGOU		= 22,	//搜狗游戏联运平台
	PT_7477			= 23,   //7477联运
	PT_TX_C9		= 24,   //tx-c9
	PT_KW			= 25,  	//mix for PT_7477
	PT_7K			= 26,	//mix for PT_7477
	PT_360UU		= 27,	//mix for PT_7477
	PT_QD			= 28,	//mix for PT_7477
	PT_V1			= 29,	//mix for PT_7477
	PT_66YOU		= 30,	//mix for PT_7477
	PT_51IGAME		= 31,	//mix for PT_7477
	PT_HUANLEYUAN	= 32,	//mix for PT_7477
	PT_BAIDU	    = 33,	//baidu联运
	PT_51WAN	    = 34,	//
	PT_YUNQU	    = 35,	//
	PT_XUNLEI	    = 36,	//
	PT_TIEXUE	    = 37,	//
	PT_FENGHUANG    = 38,	//
	PT_7k7k		    = 39,	//
	PT_360		    = 40,	//
	PT_YLT		    = 41,	//
	PT_CYXC		    = 42,	//

	PT_MAX,
};

#define Is_QQ_Platform(Platform) (Platform == PT_PENGYOU || Platform == PT_QZONE || Platform == PT_3366 || Platform == PT_qqgame || Platform == PT_TX_C9)

//农场地图
#define INT_BITS ((1<<3) * sizeof(int))
#define CHAR_BITS ((1<<3) * sizeof(char))
#define MAP_LENGTH (35)
#define MAP_WIDTH (35)
#define MAP_MAX_GRID (MAP_LENGTH * MAP_WIDTH)

//障碍物长度，向上取整
#define BARRIER_LENGTH ((MAP_MAX_GRID + CHAR_BITS - 1) / CHAR_BITS)
#define GRID_LENGTH ((MAP_MAX_GRID + INT_BITS - 1) / INT_BITS)
#define IsValidGridId(id) ( (id >= 1) && (id <= MAP_MAX_GRID))

//DB
//对应DBC端口号和共享内存号为18000+ID
#define DB_EXCHANGE_CODE    4
#define DB_NAME				7

#define DB_BASE             101
#define DB_BASE_BUFF        5000
#define DB_BASE_FULL        1

#define DB_ITEM             110
#define DB_ITEM_FULL        200

//通用活动
#define DB_GAME_ACTIVITY 102
#define DB_GAME_ACTIVITY_FULL 50
//通用活动中活动数据的个数
#define DB_GAME_DATA_NUM 15
//充值历史
#define DB_CHARGE_HISTORY 103
#define DB_CHARGE_HISTORY_FULL 15

//建筑
#define DB_BUILD 121
#define DB_BUILD_FULL  (MAP_LENGTH * MAP_WIDTH)

#define DB_CROPLAND  122
#define DB_CROPLAND_FULL 200

#define DB_PRODUCEEQUIP 123
#define DB_PRODUCEEQUIP_FULL 13

#define DB_ANIMAL 124
#define DB_ANIMAL_FULL 63

#define DB_EQUIPMENT_STAR 125
#define DB_EQUIPMENT_STAR_FULL 20

#define MEM_SESSION			91
#define SESSION_DATA		"data"


#define MEMORY_RESOURCE       301
#define MEMORY_PROPERTY_NUM DB_BASE_BUFF*10

#define MEMORY_ASYN       302
#define MEMORY_ASYN_NUM   5

enum ASYN_TYPE
{
	e_asyn_cash				= 0,
	e_asyn_coin				= 1,
	e_asyn_accrecharge		= 2,
	e_asyn_exp				= 3,
	e_asyn_r1				= 4,
	e_asyn_r2				= 5,
	e_asyn_r3				= 6,
	e_asyn_r4				= 7,

	e_asyn_max				= 10000,
};

//通知系统
#define MEMORY_NOTIFY	303
#define NOTIFY_CONTENT_LEN 128
#define PER_USER_MAX_NOTIFY	 20

//@end

#define ADMIN_UID			10000000
#define UID_MIN				10000000
#define UID_MAX				2000000000
#define UID_ZONE			500000
#define IsValidUid(uid)		(uid > UID_MIN && uid < UID_MAX)
#define ALLIANCE_ID_NULL	0
#define ALLIANCE_ID_MIN		2000000000
#define ALLIANCE_ID_START	2010000000
#define AID_ZONE			500000
#define IsAllianceId(id)	(id >= ALLIANCE_ID_MIN)


//error
#define KEY_RESULT			"error"
#define KEY_ERROR_MESSAGE	"errMsg"
#define KEY_PARAMS			"params"


#define BUFF_SIZE 2048


#define U64_LOW(val) (val & 0X00000000FFFFFFFF)
#define U64_HIGH(val) ((val >> 32) & 0X00000000FFFFFFFF)
#define U32_LOW(val) (val & 0X0000FFFF)
#define U32_HIGH(val) ((val >> 16) & 0X0000FFFF)
#define LOW_8(val) (val & 0X000000FF)
#define MAKE64(a,b,c) 	(*((uint64_t*)(&(a)))) = ((uint64_t)(b) << 32) | (c)

#define AddtoVct(filed)  if (0 != filed) {vctsubs.push_back(filed);}

//内部通讯命令字
#define PROTOCOL_EVENT_SECOND_TIMER 101

enum ActivityID
{
	e_Activity_max
};
enum SigNum
{
	e_Sig_Save			= 64,
	e_Sig_data_log		= 63,
	e_Sig_try_clear		= 62,
	e_Sig_print_info	= 61, //打印resource_manager的信息
};
enum CurCMD
{
	e_CMD_none			= 0,
	e_CMD_new			= 1,
	e_CMD_login			= 2,

	e_CMD_max
};

enum NotifyID
{
	e_Notify_max
};

enum Build_Direct
{
	direct_right = 0,  //朝右
	direct_down  = 1,  //朝下
};

enum BuildType
{
	build_type_corpland = 0,   //农地
	build_type_animal_residence = 1,  //动物住所
	build_type_animal			= 2,  //动物
	build_type_produce_equipment = 3,  //生产设备
	build_type_fruit_tree = 4,  //果树
	build_type_decorate = 5,   //装饰
	build_type_storage = 10,  //仓库
	build_type_house = 11,  //房子
};

enum StorageType
{
	type_granary = 1, //粮仓
	type_warehouse = 2, //货仓
};

//玩家属性类型
enum PropertyType
{
	type_coin	= 1,
	type_cash	= 2, //钻石
	type_exp	= 3,  //如果用户升级了会额外推送升级协议
	type_level	= 4,  //等级
};

//生产线状态
enum ProduceStatus
{
	status_free 	= 0,  //空闲
	status_hungry 	= 0,  //饥饿
	status_growup 	= 1,  //成长
	status_procing	= 1, //生产中
	status_harvest 	= 2,  //收获
	status_suspend	= 2, //暂停生产
	status_full		= 2, //饱腹
};

enum EquipmentStarProperty
{
	property_type_coin = 1,  //金币加成
	property_type_exp = 2,  //经验加成
	property_type_time = 3,  //生产时间缩短
};

//exchange code
#define CODE_SIZE 12
#define CODE_TYPE 32//0-31

#endif	//__APPDEFINE_H__
