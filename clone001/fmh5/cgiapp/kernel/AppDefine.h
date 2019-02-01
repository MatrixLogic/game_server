//AppDefine.h
//20110525	dada	create

#ifndef __APPDEFINE_H__
#define __APPDEFINE_H__

//all config

//server
//dat or xml or dir
#define APP_CONFIG_PATH						"../conf/app_config.xml"
#define APP_DOMAIN_CONFIG_PATH  			"../../pi/conf/domai_config.xml"
#define DEFAULT_APP_PATH					"/data/release/sg17/"
#define CONFIG_QQ_ITEM_INFO					"pi/conf/qq_item_info.xml"
#define CONFIG_PLATFORM_MAPPING 			"pi/conf/platform/platform_mapping.xml"
#define CONFIG_ALL_DATA_LOG_PATH			"datalog/"
#define CONFIG_STRING_FILTER_DATA 			"data/stringfilter.dat"
#define CONFIG_DBC_SERVER					"pi/conf/dbc_server.xml"
#define CONFIG_BARBARIANKING_PATH 			"data/"

//config
#define CONFIG_CLIENT_DOMAIN	"client_domain"
#define CONFIG_PLATFORM			"platform_config"

#define DEFAULT_APP_CONFIG_PATH			"conf/app_config.xml"
#define CONFIG_DATA_LOG_PATH			"datalog/"
#define CONFIG_BUSINESS_LOG_DIR 		"log/business/"
#define CONFIG_LOG_PATH					"log/"
#define ADMIN_OP_LOG_DIR				"adminlog/"
#define CONFIG_DIR_TOOL   				"tools/"

//not use
#define CONFIG_WHITE_LIST		"white_list"
#define CONFIG_ADMIN_LIST		"admin_list"
#define CONFIG_AUTO_LOGIN_URL	"auto_login_url"
//config
#define CONFIG_LOG_LEVEL		"log_level"
#define CONFIG_LOG_NAME			"log_name"
#define CONFIG_LOG_REWIND		"log_rewind"
#define CONFIG_LOG_SIZE			"log_size"
#define CONFIG_LOG_RECORD_SIZE	"log_record_size"
#define CONFIG_TCP_SERVER		"tcp_server"
#define CONFIG_TCP_HOST			"tcp_host"
#define CONFIG_ACCESS_SERVER	"access_server"
#define CONFIG_ZONE_ID			"zoneid"
#define CONFIG_SERVER_ID       "server_id"
#define CONFIG_FULL "full"
//activity ts & type & version
#define CONFIG_OPEN_TS         "open_ts"

//DB
//对应DBC端口号和共享内存号为39100+ID
#define DB_USER_MAPPING		1
#define DB_ID_CTRL			2
#define DB_BLACK_USER       3
#define DB_EXCHANGE_CODE    4
#define DB_GOODS            5
#define DB_MARKET_TASK	    6
#define DB_PAY_HISTORY		51

#define MEM_SESSION			91
//DB other
#define KEY_UID_CTRL		1
#define KEY_PAY_ID_CTRL 	2
#define KEY_ATTACK_ID_CTRL	3
#define KEY_MESSAGE_ID_CTRL	4
#define KEY_ALLIANCE_ID_CTRL 5

//app define
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
#define IsLegalUid(uid)		(uid >= UID_MIN)
#define SESSION_TIME_OUT	3600	//会话超时
typedef vector<unsigned>	UidList;

//session
#define SESSION_DATA		"data"

//error
#define KEY_RESULT			"error"
#define KEY_ERROR_MESSAGE	"errMsg"
#define KEY_PARAMS			"params"

//exchange code
#define CODE_SIZE 12
#define CODE_TYPE 32//0-31

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

#endif	//__APPDEFINE_H__
