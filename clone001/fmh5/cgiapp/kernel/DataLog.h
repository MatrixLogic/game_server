#ifndef __DATA_LOG_H__
#define __DATA_LOG_H__

#include <string>
#include <vector>
#include <map>
using namespace std;

struct DataLogCtrl
{
	int fd;
	int write_times;
	int last_day;
};

#define DATA_LOG(action,ip,type,fmt,args...) CDataLog::DataLog(action,ip,type,fmt,##args)
#define RECV_LOG(action,ip,fmt,args...) CDataLog::DataLog(action,ip,"recv",fmt,##args)
#define SEND_LOG(action,ip,fmt,args...) CDataLog::DataLog(action,ip,"send",fmt,##args)
#define DATA_INFO_LOG(action, ...) CDataLog::DataLog(action, "", "info", __VA_ARGS__)

class CDataLog
{
public:
	//记录cgi的收发情况
	static void DataLog(const char *action, const char *ip, const char * type, const char* format, ...);

	static std::string& ParseIP( const std::string pyIp, std::string& realIp );

public:
	//static char log_dir[100];
	static void SetDomain(int domain);

private:
	static map<int,DataLogCtrl> datalog_map;
	static int fdall;	//gosling特殊需求，所有服务器的日志都写在同一个文件中
	static bool init( struct tm **p );
	static int m_domain;
};

#endif
